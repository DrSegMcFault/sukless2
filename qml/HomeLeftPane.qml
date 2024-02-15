import QtQuick
import QtQuick.Layouts
import Qt.labs.platform

import Sukless
import ".."
import "style"

Rectangle {
  id: base
  color: Style.background

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

    PillButton {
      Layout.fillWidth: true
      Layout.preferredHeight: 25
      text: qsTr("Change Board Color 1")
      onClicked: {
        first.open()
      }
    }

    PillButton {
      Layout.fillWidth: true
      Layout.preferredHeight: 25
      text: qsTr("Change Board Color 2")
      onClicked: {
        second.open()
      }
    }

    ColorDialog {
     id: first
     color: game.boardModel.color1
     onColorChanged: () => {
                 game.boardModel.setColor1(first.color)
               }
    }

    ColorDialog {
     id: second
     color: game.boardModel.color2
     onColorChanged: () => {
                 game.boardModel.setColor2(second.color)
               }
    }
  }
}
