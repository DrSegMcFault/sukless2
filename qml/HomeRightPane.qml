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

    border {
      width: Style.borderThickness
      color: Style.borderColor
    }

    RowLayout {
      anchors.fill: parent
      spacing: 10

      Item{}

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
        }
      }

      Item {
        Layout.preferredWidth: 30
        Layout.fillHeight:true
        CenteredText {
          id: two
          color: rowComponent.textColor
          anchors.centerIn: parent
        }
      }
    }
  }

  ColumnLayout {
    id: rightPane
    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
      leftMargin: -Style.borderThickness
    }
    spacing: -Style.borderThickness

    PillButton {
      id: title
      Layout.fillWidth: true
      Layout.preferredHeight: 70
      text: qsTr("Moves")
      color: base.color
      radius: 0
      pixelSize: 44
    }
    Item {
      id: temp
      Layout.preferredWidth: base.width
      Layout.preferredHeight: base.height - title.height

      Flickable {
        id: flick
        clip: true
        width: temp.width
        height: temp.height
        contentHeight: col.height
        interactive: true
        flickableDirection: Flickable.AutoFlickIfNeeded

        Connections {
          target: base.mModel
          function onItemAdded(i, f, s) {
            currentMove.movenum = i + "."
            currentMove.first = f
            currentMove.second = s
          }
        }

        ColumnLayout {
          id: col
          anchors.top: parent.top
          anchors.left: parent.left
          anchors.right: parent.right
          height: implicitHeight
          spacing: -Style.borderThickness

          RowEntry {
            id: currentMove
            Layout.preferredHeight: 40
            Layout.fillWidth: true
            textColor: Style.textColor
            visible: rep.count > 1
            color: Qt.lighter(base.color, 1.7)
          }

          Repeater {
            id: rep
            model: base.mModel

            delegate: RowEntry {
              Layout.preferredHeight: 40
              Layout.fillWidth: true
              textColor: Style.textColor
              color: !model.selected ? base.color : Qt.lighter(base.color, 1.7)
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
