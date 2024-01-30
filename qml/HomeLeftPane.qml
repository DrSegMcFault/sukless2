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

    PillButton {
      color: "#2C2C2C"
      Layout.fillWidth: true
      Layout.preferredHeight: 70
      text: qsTr("Sukless2")
      textColor: "#00e5b0"
      pixelSize: 44
    }

    PillButton {
      Layout.fillWidth: true
      Layout.preferredHeight: 50
      text: qsTr("Main Menu")
      onClicked: {
        navigate("MainMenu.qml")
      }
    }
  }
}
