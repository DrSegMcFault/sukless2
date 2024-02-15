import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import ".."
import "style"

Rectangle {
  id: base
  color: Style.background

  property var mModel

  component RowEntry : Rectangle {
    property alias first: one.text
    property alias second: two.text
    property alias movenum: label.text

    ColumnLayout {
      anchors.fill: parent

      RowLayout {
        Layout.fillHeight: true
        Layout.fillWidth: true
        spacing: 10

        Item {
          Layout.preferredWidth: 15
          Layout.fillHeight: true
          CenteredText {
            id: label
            font.pixelSize: 20
            color: Style.teal
          }
        }

        Item {
          Layout.fillHeight: true
          Layout.preferredWidth: 20
        }

        Item {
          Layout.preferredWidth: 30
          Layout.fillHeight:true
          CenteredText {
            id: one
            font.pixelSize: 20
            anchors.centerIn: parent
            color: Style.teal
          }
        }

        Item { Layout.preferredWidth: 25; Layout.fillHeight: true }
        Item { Layout.preferredWidth: 25; Layout.fillHeight: true }

        Item {
          Layout.preferredWidth: 30
          Layout.fillHeight:true
          CenteredText {
            id: two
            font.pixelSize: 20
            anchors.centerIn: parent
            color: Style.teal
          }
        }
      }

      Rectangle {
        color: "black"
        Layout.preferredHeight: 2
        Layout.fillWidth: true
      }
    }
  }

  ColumnLayout {
    id: rightPane
    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
      margins: 4
    }
    spacing: 0

    PillButton {
      Layout.fillWidth: true
      Layout.preferredHeight: 70
      text: qsTr("Moves")
      color: base.color
      pixelSize: 44
      textColor: Style.teal
    }
    Item {
      id: temp
      Layout.fillWidth: true
      Layout.preferredHeight: base.height

      Flickable {
        id: flick
        clip: true
        width: temp.width
        height: temp.height
        contentHeight: col.height
        flickableDirection: Flickable.VerticalFlick

        ColumnLayout {
          id: col
          anchors.top: parent.top
          anchors.left:  parent.left
          anchors.right: parent.right
          spacing: 0

          Repeater {
            model: base.mModel

            delegate: RowEntry {
              Layout.preferredHeight: 30
              Layout.fillWidth: true
              color: base.color
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
