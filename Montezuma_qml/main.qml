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
        height: root.height - header.height - 10
        width: root.width - 10
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

                    Rectangle {
                        id: ball
                        border.color: "lightsteelblue"
                        border.width: 2
                        width: wrapper.width
                        height: width
                        radius: width / 2
                        color: ballColor
                        anchors.horizontalCenter: wrapper.horizontalCenter
                        anchors.verticalCenter: wrapper.verticalCenter
                        property bool markStatus: ballMarkStatus
                        property bool failor: roleFailor
                        onFailorChanged: if (failor) fail.start();
                        property bool fader: ballVisible
                        onFaderChanged: if (!fader) fade.start();
                        Text {
                            id: name
                            text: index
                        }
                        states: [
                            State {
                                name: "big"
                                when: ball.markStatus == false
                                PropertyChanges { target: ball; width: wrapper.width }
                            },
                            State {
                                name: "small"
                                when: ball.markStatus == true
                                PropertyChanges { target: ball; width: wrapper.height - wrapper.height / 5 }
                            }
                        ]
                        Behavior on width {
                            NumberAnimation { duration: 200 }
                        }
                        OpacityAnimator {
                            id: fade
                            target: ball;
                            from: 1;
                            to: 0;
                            duration: 300
                            onRunningChanged: {
                               if (!running) {
                                   montezuma.downfallVertical(index);
                               }
                            }
                        }
                        SequentialAnimation {
                            id: fail
                            loops: 2
                            ScaleAnimator {
                                target: ball
                                from: 1
                                to: 0.5
                                duration: 300
                            }
                            ScaleAnimator {
                                target: ball
                                from: 0.5
                                to: 1
                                duration: 300
                            }
//                            onRunningChanged: {
//                               if (!running) {
//                                   ball.failor = false;
//                               }
//                            }
                        }
                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            onClicked: {
                                montezuma.mark(index);
//                                finish.visible = true
                           }
                      }
                 }
            }
            move: Transition {
                NumberAnimation { properties: "y"; duration: 300 }
            }
            displaced: Transition {
                NumberAnimation { properties: "x"; duration: 300 }
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
        Rectangle {
            id: finish
            width: parent.width - parent.width / 5
            height: width / 4
            anchors.centerIn: parent
            border.color: "#A4856C"
            border.width: 2
            radius: width / 5
            color: "#FFF0E5"
            visible: false
            Text {
                text: "NO VARIANT"
                anchors.centerIn: finish
                font.pixelSize: finish.height / 2
                color: "blue"
            }
        }
    }

    Montezuma {
        id: montezuma
        onGameOver: finish.visible = true;
        onGameStart: finish.visible = false;
    }

    Rectangle {
        id: header
        //z: 1
        width: root.width
        height: 100
        color: root.color

        Row {
            anchors.centerIn: parent
            spacing: 10

            Rectangle {
                width: header.width / 5
                height: header.height
                color: header.color
                Column {
                    anchors.centerIn: parent
                    spacing: 10
                    Text {
                        id: move
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: header.height / 5
                        color: "#805D43"
                        text: "move"
                    }
                    Text {
                        id: moveCount
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: move.font.pixelSize
                        color: move.color
                        text: montezuma.move
                    }
                }
            }

            Rectangle {
                width: header.width / 4 //48
                height: header.height //48
                color: header.color
                Button {
                    id: mix
                    width: parent.width
                    height: header.height / 3 * 2
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    Text {
                        font.pixelSize: move.font.pixelSize
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        text: "New game"
                        color: move.color
                    }
                    onClicked: {
                        montezuma.mixBalls();
                        finish.visible = false;
                    }
                    background: Rectangle {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: parent.width
                        height: parent.height
                        color: "#FFF0E5"
                        border.color: "#A4856C"
                        border.width: 2
                        radius: width / 5
                    }
                }
            }

            Rectangle {
                width: header.width / 5 //48
                height: header.height //48
                color: header.color
                Column {
                    anchors.centerIn: parent
                    spacing: 10
                    Text {
                        id: score
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: move.font.pixelSize
                        color: move.color
                        text: "score"
                    }
                    Text {
                        id: scoreCount
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: move.font.pixelSize
                        color: move.color
                        text: montezuma.score
                    }
                }
            }
        }
        Component.onCompleted: {
            console.log("header", width, height);
        }
    }
    Component.onCompleted: {
        console.log("window", width, height);
    }
}

