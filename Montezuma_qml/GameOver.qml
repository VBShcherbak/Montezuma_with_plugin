import QtQuick 2.0

Rectangle {
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
        anchors.centerIn: parent
        font.pixelSize: parent.height / 2
        color: "blue"
    }
}
