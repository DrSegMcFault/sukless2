import QtQuick
import "../"

Item {
  id: base

  property color color: Style.teal
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

    DefaultText {
      id: txt
      anchors.fill: parent
      color: "black"
    }

    MouseArea {
      anchors.fill: parent
      onClicked: base.clicked()
    }
  }
}
