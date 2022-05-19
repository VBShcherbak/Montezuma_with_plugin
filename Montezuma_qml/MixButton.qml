import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    width: header.width / 4
    height: header.height
    color: header.color
    Button {
        id: mix
        width: parent.width
        height: header.height / 3 * 2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        Text {
            font.pixelSize: 20
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text: "New game"
            color: "#805D43"
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
