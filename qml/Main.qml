import QtQuick
import QtQuick.Controls
import QtQuick.Window
import Sukless

ApplicationWindow {
  id: base
  title: qsTr("Sukless Chess 2")
  width: 1920
  height: 1080
  visible: true

  Game {
    id: game
  }

  AppContent {
    anchors.centerIn: parent
    focus: true
    clip: true
    width: parent.width
    height: parent.height

    Keys.onLeftPressed: {
      game.showPrevious()
    }

    Keys.onRightPressed: {
      game.showNext()
    }
  }
}
