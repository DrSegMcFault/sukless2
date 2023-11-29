import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../"
import BoardModel 1.0

Item {
  id: base
  anchors.fill: parent

  BoardModel {
    id: boardModel
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

          Rectangle {
            radius: 8
            anchors.fill:parent
            anchors.margins: 1
            color: "#00e5b0"

            MouseArea {
              anchors.fill:parent
              onClicked: {
                boardModel.toggleRotation()
              }
            }

            Text {
              anchors.centerIn: parent
              text: qsTr("Flip Board")
              font {
                family: "helvetica"
                bold: true
                pixelSize: 18
              }
              color: "black"
            }
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
