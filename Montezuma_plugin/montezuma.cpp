#include <iostream>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>
#include <QTime>
#include "montezuma.h"
#include <QDebug>

Montezuma::Montezuma(QObject *parent): QAbstractListModel(parent) {
    QFile file("config.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    file.close();
    QJsonObject jsonObject = jsonDoc.object();
    QList<QVariant> list = jsonObject["colors"].toVariant().toList();
    if (list.size() == 0) {
        list.append("red");
        list.append("black");
    } else if (list.size() == 1) {
        QString temp = list.at(0).toString();
        if(temp == "red") {
            list.append("black");
        } else {
            list.append("red");
        }
    }
    auto it = list.begin();
    for (; it != list.end(); ++it ) {
        m_colors.append((*it).toString());
    }
    m_columns = jsonObject["columns"].toInt();
    m_rows = jsonObject["rows"].toInt();
    if (m_columns < 5) m_columns = 5;
    if (m_rows < 5) m_rows = 5;
    m_boardWidth = m_columns > 9 ? m_columns*50 : 500;
    m_boardHeight = m_columns > 9 ? m_rows*50+110 : 500/m_columns*m_rows+110;
    m_markIndex = -1;
    m_variant = false;
    m_move = 0;
    m_score = 0;
    m_level = 1;

    int m_size = m_rows * m_columns;
    QTime t = QTime::currentTime();
    QRandomGenerator random(t.msec());
    for (int i = 0; i < m_size; i++) {
        int j = random.generate() % m_colors.size();
        Ball ball{};
        ball.color = m_colors.at(j);
        ball.visible = true;
        ball.failor = false;
        ball.markStatus = false;
        m_balls.append(ball);
    }
    if (m_colors.size() == 2) {
        twoColorsStart();
    } else clearChunk();

    while (!m_variant) {
        for (int i = 0; i < m_balls.size(); i++) {
            if (checkHorizontalVariant(i) || checkVerticalVariant(i)) {
                m_variant = true;
                break;
            }
        }
        if (!m_variant) mixBalls();
    }
    qDebug() << m_boardWidth << m_boardHeight; //?
    timer = new QTimer(this);
    timer->setInterval(400);
    connect(timer, SIGNAL(timeout()) , this, SLOT(timerHit()));
}

int Montezuma::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return m_rows*m_columns;
}

QVariant Montezuma::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
        case Color:
            return m_balls.at(index.row()).color;
        case MarkStatus:
            return m_balls.at(index.row()).markStatus;
        case Failor:
            return m_balls.at(index.row()).failor;
        case Visible:
            return m_balls.at(index.row()).visible;
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> Montezuma::roleNames() const {
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[Color] = "ballColor";
    roles[MarkStatus] = "ballMarkStatus";
    roles[Failor] = "roleFailor";
    roles[Visible] = "ballVisible";
    return roles;
}

int Montezuma::getBoardWidth() const {
    return m_boardWidth;
}

int Montezuma::getBoardHeight() const {
    return m_boardHeight;
}

int Montezuma::getColumns() const {
    return m_columns;
}

int Montezuma::getMarkIndex() const {
    return m_markIndex;
}

void Montezuma::setMarkIndex(int index) {
    m_markIndex = index;
}

int Montezuma::getMove() const {
    return m_move;
}

void Montezuma::setMove(int move) {
    m_move = move;
    emit moveChanged();
}

int Montezuma::getScore() const {
    return m_score;
}

void Montezuma::setScore(int score) {
    m_score = score;
    emit scoreChanged();
}

void Montezuma::mark(int index) {
    if (m_markIndex >= 0) {
        if(m_markIndex == index) {
            m_balls[m_markIndex].markStatus = false;
            m_modelIndex = QAbstractItemModel::createIndex(index, 0);
            m_roles.append(Roles::MarkStatus);
            emit dataChanged(m_modelIndex, m_modelIndex, m_roles);
            m_markIndex = -1;
        } else {
            m_balls[index].markStatus = true;
            m_modelIndex = QAbstractItemModel::createIndex(index, 0);
            m_roles.append(Roles::MarkStatus);
            emit dataChanged(m_modelIndex, m_modelIndex, m_roles);
            move(index);
        }
    } else {
        m_markIndex = index;
        m_balls[m_markIndex].markStatus = true;
        m_modelIndex = QAbstractItemModel::createIndex(index, 0);
        m_roles.append(Roles::MarkStatus);
        emit dataChanged(m_modelIndex, m_modelIndex, m_roles);
    }
    m_roles.clear();
}

void Montezuma::downfallVertical(int index) {
        for ( int i = index-m_columns; i >= 0; i-=m_columns) {
            beginMoveRows(QModelIndex(), i, i, QModelIndex(), i+m_columns+1);
            m_balls.move(i,i+m_columns);
            endMoveRows();
            beginMoveRows(QModelIndex(), i+m_columns-1, i+m_columns-1, QModelIndex(), i);
            m_balls.move(i+m_columns-1,i);
            endMoveRows();
        }

        int topBall = index % m_columns;
        beginRemoveRows(QModelIndex(), topBall, topBall);
        m_balls.removeAt(topBall);
        endRemoveRows();
        beginInsertRows(QModelIndex(), topBall, topBall);
        int random = rand() % m_colors.size();
        Ball ball{};
        ball.color = m_colors.at(random);
        ball.visible = true;
        m_balls.insert(topBall, ball);
        endInsertRows();
}

bool Montezuma::validation(int index) {
    m_balls.swapItemsAt(index, m_markIndex);
    m_horizontal = findHorizontalChunk(index);
    if (!m_horizontal.found) m_horizontal = findHorizontalChunk(m_markIndex);
    m_vertical = findVerticalChunk(index);
    if (!m_vertical.found) m_vertical = findVerticalChunk(m_markIndex);
    m_balls.swapItemsAt(index, m_markIndex);
    if (m_horizontal.found || m_vertical.found) {
        return true;
    }
    return false;
}

void Montezuma::move(int index) {
    m_level = 1;
    bool m_fail = false;
    if (index - m_markIndex == 1) {
        if (validation(index)) {
            beginMoveRows(QModelIndex(), index, index, QModelIndex(), index - 1);
            m_balls.move(index, m_markIndex);
            endMoveRows();
            m_move++;
        } else m_fail = true;
    } else if (m_markIndex - index == 1) {
        if (validation(index)) {
            beginMoveRows(QModelIndex(), index, index, QModelIndex(), index + 2);
            m_balls.move(index, m_markIndex);
            endMoveRows();
            m_move++;
        } else m_fail = true;
    } else if (index - m_markIndex == m_columns) {
        if (validation(index)) {
            beginMoveRows(QModelIndex(), index, index, QModelIndex(), m_markIndex);
            m_balls.move(index, m_markIndex);
            endMoveRows();
            beginMoveRows(QModelIndex(), m_markIndex+1, m_markIndex+1, QModelIndex(), index+1);
            m_balls.move(m_markIndex+1, index);
            endMoveRows();
            m_move++;
        } else m_fail = true;
    } else if (m_markIndex - index == m_columns) {
        if (validation(index)) {
            beginMoveRows(QModelIndex(), index, index, QModelIndex(), m_markIndex+1);
            m_balls.move(index, m_markIndex);
            endMoveRows();
            beginMoveRows(QModelIndex(), m_markIndex-1, m_markIndex-1, QModelIndex(), index);
            m_balls.move(m_markIndex-1, index);
            endMoveRows();
            m_move++;
        } else m_fail = true;
    } else {
        m_fail = true;
    }
    if (m_fail) {
        m_balls[index].failor = true;
        m_balls[m_markIndex].failor = true;
    }
    emit moveChanged();
    m_roles.append(Roles::MarkStatus);
    if (m_fail) m_roles.append(Roles::Failor);
    m_modelIndex = QAbstractItemModel::createIndex(index, 0);
    m_balls[index].markStatus = false;
    emit dataChanged(m_modelIndex, m_modelIndex, m_roles);
    m_balls[m_markIndex].markStatus = false;
    m_modelIndex = QAbstractItemModel::createIndex(m_markIndex, 0);
    emit dataChanged(m_modelIndex, m_modelIndex, m_roles);
    m_roles.clear();
    if (!m_fail) {
        timer->start();
    }
    m_balls[index].failor = false;
    m_modelIndex = QAbstractItemModel::createIndex(index, 0);
    emit dataChanged(m_modelIndex, m_modelIndex, m_roles);
    m_balls[m_markIndex].failor = false;
    m_modelIndex = QAbstractItemModel::createIndex(m_markIndex, 0);
    emit dataChanged(m_modelIndex, m_modelIndex, m_roles);
    m_markIndex = -1;
}

void Montezuma::downfall() {
    int score = 0;
    m_roles.append(Roles::Visible);
    QSet<int>::iterator it;
    for (it = m_allChunks.begin(); it != m_allChunks.end(); ++it) {
        int index = *it;
        m_balls[index].visible = false;
        m_modelIndex = QAbstractItemModel::createIndex(index, 0);
        emit dataChanged(m_modelIndex, m_modelIndex, m_roles);
        score++;
    }
    m_roles.clear();
    m_score += score * m_level;
    emit scoreChanged();
}

void Montezuma::forward() {
    m_allChunks.clear();
    m_variant = false;
    bool found = false;
    for (int i = 0; i < m_balls.size(); i++) {
        m_horizontal = findHorizontalChunk(i);
        if (m_horizontal.found) {
            for (int j = m_horizontal.begin; j <= m_horizontal.end; j++) {
                m_allChunks.insert(j);
            }
            found = true;
        }
        m_vertical = findVerticalChunk(i);
        if (m_vertical.found) {
            for (int j = m_vertical.begin; j <= m_vertical.end; j+=m_columns) {
                m_allChunks.insert(j);
            }
            found = true;
        }
        if (!m_variant) {
            if (checkHorizontalVariant(i) || checkVerticalVariant(i)) {
                m_variant = true;
            }
        }
    }
    if (found) {
        downfall();
        m_level++;
    } else if (!m_variant) {
        emit gameOver();
    }
}

void Montezuma::clearChunk() {
    for (int i = 0; i < m_balls.size(); i++) {
        m_horizontal = findHorizontalChunk(i);
        if (m_horizontal.found) {
            for (int j = m_horizontal.begin; j <= m_horizontal.end; j++) {
                int k = rand() % m_colors.size();
                m_balls[j].color = m_colors.at(k);
            }
        }
        m_vertical = findVerticalChunk(i);
        if (m_vertical.found) {
            for (int j = m_vertical.begin; j <= m_vertical.end; j+=m_columns) {
                int k = rand() % m_colors.size();
                m_balls[j].color = m_colors.at(k);
            }
        }
    }
}

Chunk Montezuma::findHorizontalChunk(int index) {
    Chunk chunk{};
    int begin = index - index % m_columns;
    int end = begin + m_columns;

    for (int i = begin; i < end; i++) {
        chunk.begin = i;
        for (int j = i+1; j < end; j++) {
            if (m_balls.at(i).color == m_balls.at(j).color) {
                chunk.end = j;
            } else {
                break;
            }
        }
        if (chunk.end-chunk.begin > 1) {
            chunk.found = true;
            return chunk;
        }
    }
    chunk.found = false;
    return chunk;
}

Chunk Montezuma::findVerticalChunk(int index) {
    Chunk chunk{};
    int begin = index % m_columns;
    int end = m_columns * m_rows;

    for (int i = begin; i < end; i+=m_columns) {
        chunk.begin = i;
        for (int j = i+m_columns; j < end; j+=m_columns) {
            if (m_balls.at(i).color == m_balls.at(j).color) {
                chunk.end = j;
            } else {
                break;
            }
        }
        if (chunk.end-chunk.begin > m_columns+1) {
            chunk.found = true;
            return chunk;
        }
    }
    chunk.found = false;
    return chunk;
}

void Montezuma::timerHit() {
    forward();
    timer->stop();
}

void Montezuma::twoColorsStart() {
    int evenRow = 0;
    for (int i = 0; i < m_balls.size(); i+=m_columns) {
        evenRow = evenRow ? 0 : 1;
        int index = evenRow;
        int count = 0;
        for (int j = i; j < i+m_columns; j++) {
            m_balls[j].color = m_colors.at(index);
            count++;
            if (count > 1) {
                index = index ? 0 : 1;
                count = 0;
            }
        }
    }
}

void Montezuma::mixBalls() {
    beginResetModel();
    if (m_colors.size() == 2) {
        twoColorsStart();
    } else {
        QTime t = QTime::currentTime();
        QRandomGenerator random(t.msec());
        for (int i = 0; i < m_balls.size(); i++) {
            int j = random.generate() % m_colors.size();
            m_balls[i].color = m_colors.at(j);
        }
    }
    clearChunk();
    endResetModel();
    emit gameStart();
    m_move = 0;
    m_score = 0;
    emit moveChanged();
    emit scoreChanged();
}

bool Montezuma::checkHorizontalVariant(int i) {
    //std::cout << i << ' ';
    if (i % m_columns == m_columns - 1) return false;
    int rowBegin = i - i % m_columns;
    int rowEnd = rowBegin + m_columns - 1;
    int topRow = m_columns;
    int bottomRow = m_columns * (m_rows - 1);
    if (m_balls.at(i).color == m_balls.at(i+1).color) {
        if (i > rowBegin) {
            if (i > topRow) {
                if (m_balls.at(i).color == m_balls.at(i-1-m_columns).color) {
                    return true;
                }
            }
            if (i < bottomRow) {
                if (m_balls.at(i).color == m_balls.at(i-1+m_columns).color) {
                    return true;
                }
            }
        }
        if (i < rowEnd-1) {
            if (i > topRow) {
                if (m_balls.at(i).color == m_balls.at(i+2-m_columns).color) {
                    return true;
                }
            }
            if (i < bottomRow) {
                if (m_balls.at(i).color == m_balls.at(i+2+m_columns).color) {
                    return true;
                }
            }
        }
        if (i > rowBegin + 1) {
            if (m_balls.at(i).color == m_balls.at(i-2).color) {
                return true;
            }
        }
        if (i < rowEnd - 2) {
            if (m_balls.at(i).color == m_balls.at(i+3).color) {
                return true;
            }
        }
    }
    if (i < rowEnd - 1) {
        if (m_balls.at(i).color == m_balls.at(i+2).color) {
            if (i > topRow) {
                if (m_balls.at(i).color == m_balls.at(i+1-m_columns).color) {
                    return true;
                }
            }
            if (i < bottomRow) {
                if (m_balls.at(i).color == m_balls.at(i+1+m_columns).color) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Montezuma::checkVerticalVariant(int i) {
    if (i >= m_columns * (m_rows - 1)) return false;
    int colBegin = m_columns;
    int colEnd = m_columns * (m_rows - 1);
    int leftCol = i - i % m_columns;
    int rightCol = leftCol + m_columns - 1;
    if (m_balls.at(i).color == m_balls.at(i+m_columns).color) {
        if (i >= colBegin) {
            if (i > leftCol) {
                if (m_balls.at(i).color == m_balls.at(i-1-m_columns).color) {
                    return true;
                }
            }
            if (i < rightCol) {
                if (m_balls.at(i).color == m_balls.at(i+1-m_columns).color) {
                    return true;
                }
            }
        }
        if (i < colEnd - m_columns) {
            if (i > leftCol) {
                if (m_balls.at(i).color == m_balls.at(i-1+m_columns*2).color) {
                    return true;
                }
            }
            if (i < rightCol) {
                if (m_balls.at(i).color == m_balls.at(i+1+m_columns*2).color) {
                    return true;
                }
            }
        }
        if (i >= colBegin + m_columns) {
            if (m_balls.at(i).color == m_balls.at(i-m_columns*2).color) {
                return true;
            }
        }
        if (i <= colEnd - m_columns*2) {
            if (m_balls.at(i).color == m_balls.at(i+m_columns*3).color) {
                return true;
            }
        }
    }
    if (i < colEnd - m_columns) {
        if (m_balls.at(i).color == m_balls.at(i+m_columns*2).color) {
            if (i > leftCol) {
                if (m_balls.at(i).color == m_balls.at(i-1+m_columns).color) {
                    return true;
                }
            }
            if (i < rightCol) {
                if (m_balls.at(i).color == m_balls.at(i+1+m_columns).color) {
                    return true;
                }
            }
        }
    }
    return false;
}

std::ostream& operator<< (std::ostream &out, const Montezuma &montezuma) {
    for (int i = 0; i < montezuma.m_balls.size(); ++i) {
        out << montezuma.m_balls.at(i).color.toStdString() << ' ';
    }
    return out;
}

