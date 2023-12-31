import QtQuick

Item {
  id: base

  property color color: "#00e5b0"
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
      color: "black"
      anchors.centerIn: parent
    }

    MouseArea {
      anchors.fill: parent
      onClicked: base.clicked()
    }
  }
}
