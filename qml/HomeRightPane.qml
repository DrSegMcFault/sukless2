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
    }

    spacing: 2
  }
}
