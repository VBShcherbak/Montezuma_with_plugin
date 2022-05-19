#ifndef MONTEZUMA_H
#define MONTEZUMA_H

#include <QObject>
#include <QAbstractListModel>
#include <QSet>
#include <QTimer>

struct Chunk {
    int begin;
    int end;
    bool found;
};

struct Ball {
    QString color;
    bool markStatus;
    bool failor;
    bool visible;
};

class Montezuma : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int boardWidth READ getBoardWidth CONSTANT);
    Q_PROPERTY(int boardHeight READ getBoardHeight CONSTANT);
    Q_PROPERTY(int columns READ getColumns CONSTANT);
    Q_PROPERTY(int markIndex READ getMarkIndex WRITE setMarkIndex NOTIFY markIndexChanged)
    Q_PROPERTY(int score READ getScore WRITE setScore NOTIFY scoreChanged);
    Q_PROPERTY(int move READ getMove WRITE setMove NOTIFY moveChanged)

public:
    Montezuma(QObject *parent = 0);
    enum Roles {
        Color = Qt::UserRole + 1,
        MarkStatus,
        Failor,
        Visible
    };
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
    int getBoardWidth() const;
    int getBoardHeight() const;
    int getColumns() const;
    int getMarkIndex() const;
    void setMarkIndex(int);
    int getMove() const;
    void setMove(int move);
    int getScore() const;
    void setScore(int score);
    bool validation(int index);
    Q_INVOKABLE void downfall();
    Q_INVOKABLE void downfallVertical(int index);
    Q_INVOKABLE void forward();

    friend std::ostream& operator<< (std::ostream &out, const Montezuma &fifteen);

public slots:
    void move(int index);
    void mark(int index);
    void mixBalls();
    void timerHit();

private:
    Chunk findHorizontalChunk(int index);
    Chunk findVerticalChunk(int index);
    bool checkHorizontalVariant(int index);
    bool checkVerticalVariant(int index);
    void clearChunk();
    void twoColorsStart();

signals:
    void gameOver();
    void gameStart();
    void markIndexChanged();
    void scoreChanged();
    void moveChanged();
    void failChanged();

private:
    QList<Ball> m_balls;
    QList<QString> m_colors;
    QVector<int> m_roles;
    QModelIndex m_modelIndex;
    QSet<int> m_allChunks;
    int m_columns;
    int m_rows;
    int m_boardWidth;
    int m_boardHeight;
    int m_markIndex;
    int m_move;
    int m_score;
    int m_level;
    Chunk m_horizontal;
    Chunk m_vertical;
    bool m_variant;
    QTimer *timer;
};

#endif // MONTEZUMA_H
