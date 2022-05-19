import QtQuick 2.15

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
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            montezuma.mark(index);
       }
    }
}
