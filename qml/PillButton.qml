import QtQuick

import ".."
import "style"

Item {
  id: base

  property color color: Style.buttonColor
  property alias border: button.border
  property alias radius: button.radius
  property alias text: txt.text
  property alias textColor: txt.color
  property alias pixelSize: txt.font.pixelSize

  signal clicked()

  Rectangle {
    id: button
    anchors.fill: parent
    radius: 8
    color: base.color
    border.width: 0

    CenteredText {
      id: txt
      anchors.fill: parent
    }

    MouseArea {
      anchors.fill: parent
      onClicked: base.clicked()
    }
  }
}
