import QtQuick
import QtQuick.Controls
import QtQuick.Window

ApplicationWindow {
  id: base
  title: qsTr("Sukless Chess 2")
  width: 1920
  height: 1080

  visible: true

  AppContent {
    focus: true
    clip: true
    anchors.centerIn: parent
    width: parent.width
    height: parent.height
  }
}
