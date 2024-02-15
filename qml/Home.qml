import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia

import Sukless

import ".."
import "Style"

Item {
  id: base
  anchors.fill: parent

  required property int userColor
  required property bool aiAssistEnabled
  required property bool aiEnabled
  required property int aiDifficulty
  property bool gameEnd: false

  Game {
    id: game
    Component.onCompleted: {
      game.init(base.userColor, base.aiAssistEnabled,
                     base.aiEnabled, base.aiDifficulty)
    }
  }

  Connections {
    target: game

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

    function onGameOver(text) {
      base.gameEnd = true;
      gameOver.text = text
    }

    function onMoveConfirmed(from, to) {
      console.log("Move confirmed: ", from, " ", to)
    }
  }

  GameOverPopup {
    id: gameOver

    visible: base.gameEnd === true
    onClicked: {
      base.gameEnd = false;
      game.reset();
    }
  }

  MediaPlayer {
    id: move_sound
    source: "qrc:/sounds/move.mp3"
    audioOutput: AudioOutput {
      volume: 100
    }
  }
  MediaPlayer {
    id: gameEndSound
    source: "qrc:/sounds/game_end.mp3"
    audioOutput: AudioOutput {
      volume: 100
    }
  }
  MediaPlayer {
    id: illegalMoveSound
    source: "qrc:/sounds/illegal_move.mp3"
    audioOutput: AudioOutput {
      volume: 100
    }
  }

  Rectangle {
    color: Style.background
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
        color: Style.background
        Layout.fillHeight: true
        Layout.preferredWidth: base.width * 3/5

        Board {
          id: board
          board: game.boardModel
          height: {
            if (parent.height * 9/10 <= parent.width) {
              return parent.height * 9/10
            }
            return parent.width

          }
          width: height
          anchors.centerIn: parent
          onMove: (from, to) => {
                    game.handleMove(from, to, -1)
                  }
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
        mModel: game.moveModel
      }
    }
  }
}
