import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import BoardModel 1.0

Item {
  id: base
  anchors.fill: parent
  required property int userColor
  required property bool aiAssistEnabled
  required property bool aiEnabled
  required property int aiDifficulty

  BoardModel {
    id: boardModel
    Component.onCompleted: {
      boardModel.init(base.userColor, base.aiAssistEnabled,
                      base.aiEnabled, base.aiDifficulty)
    }
  }

  Rectangle {
    color: "#2C2C2C"
    anchors.fill: parent

    RowLayout {
      id: row
      anchors.fill:parent
      spacing: 0

      HomeLeftPane {
        id: left
        Layout.fillHeight: true
        Layout.fillWidth: true
      }

      Rectangle {
        color: "black"
        Layout.preferredWidth: 2
        Layout.fillHeight: true
      }

      Rectangle {
        color: "#2C2C2C"
        Layout.fillHeight: true
        Layout.preferredWidth: 740

        Rectangle {
          color: "black"
          anchors.bottom: board.top
          anchors.right: parent.right
          anchors.rightMargin: 4
          anchors.bottomMargin: 10
          width: 120
          height: 25
          radius: 8

          PillButton {
            anchors.fill: parent
            anchors.margins: 1
            onClicked: boardModel.toggleRotation()
            text: qsTr("Flip Board")
          }
        }

        Board {
          id: board
          height: 740
          anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: (base.height - height) / 2
          }
        }
      }

      Rectangle {
        color: "black"
        Layout.preferredWidth: 2
        Layout.fillHeight: true
      }

      HomeRightPane {
        Layout.fillHeight: true
        Layout.fillWidth: true
        baseModel: boardModel
      }
    }
  }
}
