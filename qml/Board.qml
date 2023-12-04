import QtQuick
import QtQuick.Layouts
import BoardModel 1.0
import QtMultimedia

Rectangle {
  id: base
  color: "white"

  property bool checkmate: false
  property color color1: "white"
  property color color2: "#4284ed"

  // container for a piece that has been picked up
  // by the user
  property Image draggedItem: null

  Connections {
    target: boardModel

    // this is incredibly stupid,
    // if i dont have a MediaPlayer per sound,
    // the sound only plays once and will not play again
    // this appears to be a common issue online

    function onPlaySound(sound) {
      if (sound.toString() === move_sound.source.toString()) {
        move_sound.play();
      }
      else if (sound.toString() === gameEndSound.source.toString()) {
        gameEndSound.play()
      }
      else if (sound.toString() === illegalMoveSound.source.toString()) {
        illegalMoveSound.play()
      }
    }
  }

  Connections {
    target: boardModel
    function onCheckmate(text) {
      base.checkmate = true;
      gameover.gmtxt = text
    }
  }

  MediaPlayer {
    id: move_sound
    source: "qrc:/resources/move_sound.mp3"
    audioOutput: AudioOutput {
      volume: 100
    }
  }
  MediaPlayer {
    id: gameEndSound
    source: "qrc:/resources/game_end.mp3"
    audioOutput: AudioOutput {
      volume: 100
    }
  }
  MediaPlayer {
    id: illegalMoveSound
    source: "qrc:/resources/illegal_move.mp3"
    audioOutput: AudioOutput {
      volume: 100

    }
  }


  Rectangle {
    id: gameover
    property string gmtxt
    color: "black"
    width: 280
    height: 85
    radius: 4
    anchors.centerIn: parent
    z: 10000
    visible: base.checkmate === true

    Rectangle {
      id: inner
      color: "#00e5b0"
      anchors.fill: parent
      anchors.margins: 2
      radius: 4
      Text {
        color: "black"
        anchors.centerIn: parent
        font.family: "helvetica"
        font.pixelSize: 36
        font.bold: true
        text: qsTr(gameover.gmtxt)
      }
    }

    Rectangle {
      width: 120
      height: 20
      color: "black"
      radius: 4

      anchors {
        bottom: inner.bottom
        left: parent.left
        leftMargin: 80
        bottomMargin: 4
      }

      MouseArea {
        anchors.fill: parent
        onClicked: {
          base.checkmate = false;
          boardModel.reset();
        }
      }

      Text {
        anchors.centerIn: parent
        color: "#00e5b0"
        text: qsTr("Reset Board")
        font.family: "helvetica"
        font.bold: true
        font.pixelSize: 18
      }
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
      model: boardModel

      delegate: Rectangle {
        id: basedel

        Layout.fillWidth: true
        Layout.fillHeight: true

        color: ((Math.floor(index / 8) + index) % 2 === 0) ? base.color1 : base.color2

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

        Rectangle {
          color: "gray"
          opacity: 50
          radius: 180
          visible: model.possible
          height: parent.height / 3
          width: parent.width / 3
          anchors.centerIn: parent
        }


        // file label
        Text {
          text: model.fileLabel
          font.family: "helvetica"
          font.pixelSize: 18
          color: ((Math.floor(index / 8) + index) % 2 === 0) ? base.color2 : base.color1
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
          color: ((Math.floor(index / 8) + index) % 2 === 0) ? base.color2 : base.color1
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
