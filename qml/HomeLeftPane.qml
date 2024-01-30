import QtQuick
import QtQuick.Layouts
import BoardModel 1.0
import "../"

Rectangle {
  id: base
  color: Style.background
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
      color: base.color
      Layout.fillWidth: true
      Layout.preferredHeight: 70
      text: qsTr("Sukless2")
      textColor: Style.teal
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
