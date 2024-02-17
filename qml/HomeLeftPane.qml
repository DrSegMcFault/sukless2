import QtQuick
import QtQuick.Layouts

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
      Layout.fillWidth: true
      Layout.preferredHeight: 70
      text: qsTr("Sukless2")
      color: base.color
      pixelSize: 44
    }

    PillButton {
      Layout.fillWidth: true
      Layout.preferredHeight: 40
      text: qsTr("Go")

      onClicked: {
        game.init(Game.WHITE, 0, 0, 0)
        // game.init(leftOption.selected ? Game.WHITE : Game.BLACK,
        //           enabledAiAssist.value,
        //           aiEnabled.value,
        //           aiDifficulty.value)
      }
    }
  }
}
