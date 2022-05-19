import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import plugins.montezuma 1.0

Window {
    id: root
    width: montezuma.boardWidth
    height: montezuma.boardHeight
    color: "#FFECD5"
    visible: true
    title: qsTr("Montezuma")

    Rectangle {
        id: background
        x: 5
        width: root.width - 10
        height: root.height - 110
        color: root.color
        border.color: "#90674B"
        border.width: 2
        anchors.top: header.bottom

        GridView {
            id: board
            anchors.fill: parent
            cellWidth: width/montezuma.columns
            cellHeight: cellWidth
            anchors.margins: 4
            model: montezuma
            delegate:
                Rectangle {
                    id: wrapper
                    width: GridView.view.cellWidth - 2
                    height: width
                    color: "transparent"
                    Ball {}
            }
            move: Transition {
                NumberAnimation { properties: "x,y"; duration: 300 }
            }
            displaced: Transition {
                NumberAnimation { properties: "x,y"; duration: 300 }
            }
            populate: Transition {
                    NumberAnimation { properties: "x,y"; duration: 500 }
            }
            add: Transition {
                NumberAnimation {
                    property: "y"
                    duration: 300
                    from: -board.cellWidth
                }
                onRunningChanged: {
                   if (!running) {
                       montezuma.forward();
                   }
                }
            }
        }
        GameOver{
            id: finish
        }
    }

    Rectangle {
        id: header
        width: root.width
        height: 100
        color: root.color

        Row {
            anchors.centerIn: parent
            spacing: 10
            Count {
                counterName: "move"
                counterValue: montezuma.move
            }
            MixButton{}
            Count {
                counterName: "score"
                counterValue: montezuma.score
            }
        }
    }

    Montezuma {
        id: montezuma
        onGameOver: finish.visible = true;
        onGameStart: finish.visible = false;
    }
}

