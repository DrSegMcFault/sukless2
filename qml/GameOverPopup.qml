import QtQuick
import Sukless

import ".."
import "style"

Rectangle {
  id: base

  property alias text: inner.text

  signal clicked()

  color: Style.borderColor
  width: 280
  height: 85
  radius: 4
  anchors.centerIn: parent
  z: 10000

  PillButton {
    id: inner
    anchors.fill: parent
    anchors.margins: 2
    radius: 4
    pixelSize: 36
  }

  PillButton {
    width: 120
    height: 20
    radius: 4
    text: qsTr("Reset Board")

    anchors {
      bottom: inner.bottom
      left: parent.left
      leftMargin: 80
      bottomMargin: 4
    }

    onClicked: base.clicked()
  }
}
