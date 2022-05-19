import QtQuick 2.0

Rectangle {
    width: header.width / 5
    height: header.height
    color: header.color
    property alias counterName: title.text
    property alias counterValue: value.text

    Column {
        anchors.centerIn: parent
        spacing: 10
        Text {
            id: title
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 20
            color: "#805D43"
        }
        Text {
            id: value
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 20
            color: "#805D43"
        }
    }
}
