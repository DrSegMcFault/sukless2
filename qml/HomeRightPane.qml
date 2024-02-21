import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Sukless

import ".."
import "style"

Rectangle {
  id: base
  color: Style.background

  property var mModel

  component RowEntry : Rectangle {
    id: rowComponent
    property alias first: one.text
    property alias second: two.text
    property alias movenum: label.text
    property color textColor
    radius: 4

    RowLayout {
      anchors.fill: parent
      spacing: 10

      Item {}

      Item {
        Layout.preferredWidth: 15
        Layout.fillHeight: true
        CenteredText {
          id: label
          color: rowComponent.textColor
          anchors.centerIn: parent
        }
      }

      Item {
        Layout.preferredWidth: 30
        Layout.fillHeight: true
        CenteredText {
          id: one
          color: rowComponent.textColor
          anchors.centerIn: parent
          font.bold: false
        }
      }

      Item {
        Layout.preferredWidth: 30
        Layout.fillHeight: true
        CenteredText {
          id: two
          color: rowComponent.textColor
          font.bold: false
          anchors.centerIn: parent
        }
      }
    }
  }

  ColumnLayout {
    id: rightPane
    anchors {
      top: base.top
      left: base.left
      right: base.right
      leftMargin: 4
      rightMargin: 4
    }

    spacing: 0

    PillButton {
      id: title
      Layout.preferredHeight: 70
      Layout.fillWidth: true
      text: qsTr("Moves")
      color: base.color
      radius: 4
      pixelSize: 44
    }

    RowEntry {
      id: currentMove
      Layout.preferredHeight: 30
      Layout.fillWidth: true
      movenum: mModel.activeIndex + "."
      first: mModel.activeFirst
      second: mModel.activeSecond
      textColor: Style.textColor
      visible: rep.count > 1
      color: Qt.lighter(base.color, 1.7)
    }

    Item {
      id: temp
      Layout.preferredWidth: parent.width
      Layout.preferredHeight: base.height - (title.height + currentMove.height)

      Flickable {
        id: flick
        clip: true
        width: temp.width
        height: temp.height
        contentHeight: col.height
        interactive: true
        flickableDirection: Flickable.AutoFlickIfNeeded

        ColumnLayout {
          id: col
          anchors.top: parent.top
          anchors.left: parent.left
          anchors.right: parent.right
          spacing: 0

          Repeater {
            id: rep
            model: base.mModel

            delegate: RowEntry {
              Layout.fillWidth: true
              Layout.preferredHeight: 30
              textColor: Style.textColor
              color: model.selected ? Qt.lighter(base.color, 1.7) : Style.background
              movenum: model.index + 1 + "."
              first: model.first
              second: model.second
            }
          }
        }
      }
    }
  }
}
