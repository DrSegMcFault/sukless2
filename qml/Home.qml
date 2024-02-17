import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia

import Sukless

import ".."
import "style"

Item {
  id: base
  anchors.fill: parent
  property bool gameEnd: false
  property bool selectPromotion: false

  Component.onCompleted: splitView.restoreState(settings.splitView)
  Component.onDestruction: settings.splitView = splitView.saveState()

  Settings {
    id: settings
    property var splitView
  }

  Game {
    id: game
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

    function onPromotionSelect(from, to, c) {
      console.log("Promotion Select", from, " ", to, " ", c)
      promotePopup.moveFrom = from
      promotePopup.moveTo = to
      promotePopup.colorMoving = c
      base.selectPromotion = true
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

  PromotionPopup {
    id: promotePopup
    z: 10000
    anchors.centerIn: base
    visible: base.selectPromotion === true
    width: board.width / 2
    height: board.height / 8
    onSelected: (from, to, piece) => {
                  game.handleMove(from, to, piece)
                  base.selectPromotion = false
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

    SplitView {
      id: splitView
      anchors.fill: parent
      orientation: Qt.Horizontal
      spacing: 0

      HomeLeftPane {
        id: left
        SplitView.preferredHeight: parent.height
        SplitView.preferredWidth: parent.width * 1/5
      }

      Rectangle {
        id: center
        color: Style.background
        SplitView.fillHeight: true
        SplitView.fillWidth: parent.width * 3/5
        SplitView.minimumWidth: parent.width * 1/4

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
                    game.handleMove(from, to, 0)
                  }
        }
      }

      HomeRightPane {
        id: right
        mModel: game.moveModel
        SplitView.preferredHeight: parent.height
        SplitView.preferredWidth: parent.width * 1/5
        SplitView.maximumWidth: parent.width * 1/2
        SplitView.minimumWidth: 50
      }
    }
  }
}
