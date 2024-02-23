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

    function onPromotionSelect(from, to, c) {
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
    anchors.fill: parent
    color: Style.backgroundLight
    radius: 6

    SplitView {
      id: splitView
      anchors.fill: parent
      anchors.margins: Style.borderThickness
      orientation: Qt.Horizontal
      spacing: 0
      handle: Rectangle {
        id: handleDelegate
        implicitWidth:  Style.borderThickness
        implicitHeight: Style.borderThickness
        color: (SplitHandle.pressed || SplitHandle.hovered) ? Style.buttonColor
                                                            : Style.backgroundLight

        containmentMask: Item {
          x: (handleDelegate.width - width) / 2
          width: Style.borderThickness * 8
          height: splitView.height
        }
      }

      HomeLeftPane {
        id: left
        SplitView.preferredHeight: parent.height
        SplitView.preferredWidth: parent.width * 1/5
        SplitView.maximumWidth: parent.width * 1/4
        radius: 4
      }

      Rectangle {
        id: center
        color: Style.background
        radius: 4

        SplitView.preferredHeight: parent.height
        SplitView.fillWidth: true
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
        SplitView.maximumWidth: parent.width * 1/4
        SplitView.minimumWidth: parent.width * 1/6
        radius: 4
      }
    }
  }
}
