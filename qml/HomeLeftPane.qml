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
    }

    spacing: 2
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
        color: "white"
      }
    }

    Rectangle {
      color: "black"
      Layout.preferredHeight: 4
      Layout.fillWidth: true
    }
  }
}
