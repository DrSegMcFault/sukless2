import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import BoardModel 1.0

Rectangle {
  id: base
  color: "#2C2C2C"
  property BoardModel baseModel

  ColumnLayout {
    id: rightPane
    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
      margins: 4
    }
    spacing: 2

    Item {
      Layout.fillWidth: true
      Layout.preferredHeight: 60
      DefaultText {
        anchors.centerIn: parent
        text: qsTr("Moves")
        color: "#00e5b0"
        font.pixelSize: 44
      }
    }
  }
}
