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

    RowLayout {
      id: colorSelect

      Layout.alignment: Qt.AlignCenter
      Layout.fillWidth: true
      Layout.preferredHeight: 60
      spacing: 20
      property int value: Game.WHITE

      Rectangle {
        Layout.fillHeight: true
        Layout.preferredWidth: height
        color: Style.textColor
        radius: 8

        border {
          width: colorSelect.value === Game.WHITE ? Style.borderThickness *2 : Style.borderThickness
          color: colorSelect.value === Game.WHITE ? Style.buttonColor : Style.borderColor
        }

        Image {
          source: "qrc:/images/king_white.svg"
          width: parent.width * 9/10
          height: parent.height * 9/10
          anchors.centerIn: parent
        }
        MouseArea {
          anchors.fill: parent
          onClicked: {
            colorSelect.value = Game.WHITE
            game.boardModel.setRotation(Game.WHITE)
          }
        }
      }

      Rectangle {
        Layout.fillHeight: true
        Layout.preferredWidth: height
        color: Style.textColor
        radius: 8
        border {
          width: colorSelect.value === Game.BLACK ? Style.borderThickness *2 : Style.borderThickness
          color: colorSelect.value === Game.BLACK ? Style.buttonColor : Style.borderColor
        }
        Image {
          source: "qrc:/images/king_black.svg"
          width: parent.width * 9/10
          height: parent.height * 9/10
          anchors.centerIn: parent
        }
        MouseArea {
          anchors.fill: parent
          onClicked: {
            colorSelect.value = Game.BLACK
            game.boardModel.setRotation(Game.BLACK)
          }
        }
      }
    }

    PillButton {
      Layout.fillWidth: true
      Layout.preferredHeight: 40
      text: colorSelect.visible ? qsTr("Go") : qsTr("New Game")

      onClicked: {
        if (colorSelect.visible) {
          game.init(colorSelect.value, false, true, 0)
          colorSelect.visible = false
        } else {
          game.reset();
          colorSelect.visible = true
        }
      }
    }
  }
}
