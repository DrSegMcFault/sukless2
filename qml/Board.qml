import QtQuick
import QtQuick.Layouts
import BoardModel 1.0

Rectangle {
  id: centerPane
  color: "white"

  // container for a piece that has been picked up
  // by the user
  property Image draggedItem: null

  GridLayout {
    id: grid
    anchors.fill: parent
    rows: 8
    columns: 8
    rowSpacing: 0
    columnSpacing: 0

    Repeater {
      id: rep
      model: boardModel

      delegate: Rectangle {
        id: basedel

        Layout.fillWidth: true
        Layout.fillHeight: true

        color: ((Math.floor(index / 8) + index) % 2 === 0) ? "white" : "#4284ed"

        property int test: model.index

        MouseArea {
          id: click
          anchors.fill: parent

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
                  boardModel.move(draggedItem.i, item.test);
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
                             }
        }

        Image {
          id: image
          readonly property int i: model.index
          source: model.icon
          anchors.centerIn: draggedItem ? undefined : parent
        }


        // pseudo-legal move indicator
        Rectangle {
          color: "black"
          radius: 180
          width: parent.width / 3
          height: parent.height / 3
          visible: model.possible
          anchors.centerIn: parent
          Rectangle {
            color: "grey"
            radius: 180
            anchors.fill: parent
            anchors.margins: 1

          }
        }

        // file label
        Text {
          text: model.fileLabel
          font.family: "helvetica"
          font.pixelSize: 18
          color: "black"
          font.bold: true
          anchors.bottom: parent.bottom
          anchors.right: parent.right
          anchors.rightMargin: 2
          anchors.bottomMargin: 2
        }

        // rank label
        Text {
          text: model.rankLabel
          font.family: "helvetica"
          color: "black"
          font.pixelSize: 18
          font.bold: true
          anchors.top: parent.top
          anchors.left: parent.left
          anchors.topMargin: 2
          anchors.leftMargin: 2
        }
      }
    }
  }
}
