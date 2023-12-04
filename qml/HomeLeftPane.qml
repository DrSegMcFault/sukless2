import QtQuick
import QtQuick.Layouts
import BoardModel 1.0

Rectangle {
  id: base
  color: "#2C2C2C"
  property BoardModel baseModel

  ColumnLayout {
    id: leftCol
    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
      leftMargin: 4
      rightMargin: 4
    }

    spacing: 10

    Rectangle {
      color: "#2C2C2C"
      Layout.fillWidth: true
      Layout.preferredHeight: 70

      Text {
        anchors.centerIn: parent
        font.family: "helvetica"
        font.pixelSize: 48
        text: qsTr("Sukless2")
        font.bold: true
        color: "#00e5b0"
      }
    }

    Rectangle {
      color: "black"
      radius: 8
      Layout.fillWidth: true
      Layout.preferredHeight: 70

      Rectangle {
        anchors.fill:parent
        anchors.margins: 1
        color: "#00e5b0"
        radius: 8

        MouseArea {
          anchors.fill: parent
          onClicked: {
            navigate("MainMenu.qml")
          }
        }

        Text {
          anchors.centerIn: parent
          font.family: "helvetica"
          font.pixelSize: 18
          text: qsTr("Main Menu")
          font.bold: true
          color: "black"
        }
      }
    }
  }
}
