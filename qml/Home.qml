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
        Layout.preferredWidth: base.width * 1/5
      }

      Rectangle {
        color: "black"
        Layout.preferredWidth: 2
        Layout.fillHeight: true
      }

      Rectangle {
        id: center
        color: "#2C2C2C"
        Layout.fillHeight: true
        Layout.preferredWidth: base.width * 3/5

        Board {
          id: board
          height: {
            if (parent.height * 9/10 <= parent.width) {
              return parent.height * 9/10
            }
            return parent.width

          }
          width: height
          anchors.centerIn: parent
        }
      }

      Rectangle {
        color: "black"
        Layout.preferredWidth: 2
        Layout.fillHeight: true
      }

      HomeRightPane {
        id: right
        Layout.fillHeight: true
        Layout.preferredWidth: base.width * 1/5
      }
    }
  }
}
