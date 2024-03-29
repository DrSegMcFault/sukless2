import QtQuick
import QtQuick.Layouts
import Qt.labs.platform

import Sukless

import ".."
import "style"

Rectangle {
  id: base
  color: Style.background

  required property BoardModel board
  signal move(int from, int to)

  property bool gameEnd: false

  // container for a piece that has been picked up
  // by the user
  property Image draggedItem: null


  DefaultText {
    id: top
    anchors.bottom: grid.top
    anchors.bottomMargin: Style.borderThickness * 2
    anchors.right: grid.right
    anchors.rightMargin: 5
    text: board.topEval >= 0 ? qsTr("+%1").arg(board.topEval) : board.topEval
    font.pixelSize: 20
  }

  DefaultText {
    id: bottom
    anchors.top: grid.bottom
    anchors.topMargin: Style.borderThickness * 2
    anchors.right: grid.right
    anchors.rightMargin: Style.borderThickness * 2
    text: board.bottomEval >= 0 ? qsTr("+%1").arg(board.bottomEval) : board.bottomEval
    font.pixelSize: 20
  }

  GridLayout {
    id: colorSelect
    rows: 1
    columns: 2
    rowSpacing: 0
    columnSpacing: -Style.borderThickness
    anchors.bottom: grid.top
    anchors.left: grid.left
    anchors.leftMargin: -Style.borderThickness
    anchors.bottomMargin: 2
    width: parent.width * 1/16
    height: parent.height * 1/32

    Rectangle {
      Layout.fillHeight: true
      Layout.fillWidth: true
      color: base.board.color1
      border {
        width: Style.borderThickness
        color: "black"
      }

      MouseArea {
        anchors.fill: parent
        onClicked: first.open()
      }
    }

    Rectangle {
      Layout.fillHeight: true
      Layout.fillWidth: true
      color: base.board.color2
      border {
        width: Style.borderThickness
        color: "black"
      }

      MouseArea {
        anchors.fill: parent
        onClicked: second.open()
      }
    }
  }

  PillButton {
    id: flip
    anchors.left: colorSelect.right
    anchors.leftMargin: Style.borderThickness * 2
    anchors.bottom: grid.top
    anchors.bottomMargin: Style.borderThickness * 2
    width: parent.width * 1/8
    height: parent.height * 1/35
    pixelSize: 16
    onClicked: base.board.toggleRotation()
    text: qsTr("Flip Board")
  }

  ColorDialog {
    id: first
    color: base.board.color1
    onColorChanged: () => {
                      base.board.color1 = first.color
                      first.color = base.board.color1
                    }
  }

  ColorDialog {
    id: second
    color: base.board.color2
    onColorChanged: () => {
                      base.board.color2 = second.color
                      second.color = base.board.color2
                    }
  }

  GridLayout {
    id: grid
    anchors.fill: parent
    rows: 8
    columns: 8
    rowSpacing: 0
    columnSpacing: 0

    Repeater {
      id: rep
      model: base.board

      delegate: Rectangle {
        id: basedel

        Layout.fillWidth: true
        Layout.fillHeight: true

        color: ((Math.floor(index / 8) + index) % 2 === 0) ? base.board.color1 : base.board.color2

        property int test: model.index

        MouseArea {
          id: click
          anchors.fill: parent
          drag.target: image

          onPressed: {
            draggedItem = image;
            draggedItem.parent.z = 999;
          }

          onReleased: {
            if (draggedItem) {
              draggedItem.parent.z = 1

              // position of dragged item relative to grid layout
              var relativePos = draggedItem.mapToItem(grid, 0, 0);
              relativePos.x += draggedItem.width / 2;
              relativePos.y += draggedItem.height / 2;

              // Calculate the grid cell size
              var cellWidth = grid.width / 8;
              var cellHeight = grid.height / 8;

              // Iterate through the Repeater model to find the target rectangle
              for (var i = 0; i < rep.count; ++i) {
                var item = rep.itemAt(i);

                // center of item in question
                var itemX = item.x + grid.x + cellWidth / 2;
                var itemY = item.y + grid.y + cellHeight / 2;

                // if the center of the item is within the bounds of the grid box
                if (Math.abs(relativePos.x - itemX) <= cellWidth / 2
                    && Math.abs(relativePos.y - itemY) <= cellHeight / 2)
                {
                  // display user attempted move source, target
                  base.move(draggedItem.i, item.test);
                  break;
                }
              }
            }
          }

          onPositionChanged: (mouse) => {
                               if (draggedItem) {
                                 draggedItem.x = mouse.x - draggedItem.width / 2;
                                 draggedItem.y = mouse.y - draggedItem.height / 2;
                               }
                               mouse.accepted = false
                             }
        }

        Image {
          id: image
          readonly property int i: model.index
          width: parent.width * 9/10
          height: width
          source: model.icon
          anchors.centerIn: draggedItem ? undefined : parent
        }

        // file label
        CenteredText {
          id: fileLabel
          anchors.bottom: parent.bottom
          anchors.right: parent.right
          anchors.rightMargin: Style.borderThickness
          text: model.fileLabel
          color: ((Math.floor(index / 8) + index) % 2 === 0) ? base.board.color2 : base.board.color1
        }

        // rank label
        CenteredText {
          text: model.rankLabel
          color: ((Math.floor(index / 8) + index) % 2 === 0) ? base.board.color2 : base.board.color1
          anchors.top: parent.top
          anchors.left: parent.left
          anchors.topMargin: Style.borderThickness
          anchors.leftMargin: Style.borderThickness
        }
      }
    }
  }
}
