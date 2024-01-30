import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import BoardModel 1.0

Rectangle {
  id: base
  color: "#2C2C2C"

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

        Item {}

        DefaultText {
          id: label
          Layout.preferredWidth: 70
          font.pixelSize: 22
          color: "#00e5b0"
        }

        Item {}

        Item {
          Layout.preferredWidth: 30
          DefaultText {
            id: one
            font.pixelSize: 22
            anchors.centerIn: parent
            color: "#00e5b0"
          }
        }

        Item { Layout.preferredWidth: 25 }
        Item { Layout.preferredWidth: 25 }

        Item {
          Layout.preferredWidth: 30
          DefaultText {
            id: two
            font.pixelSize: 22
            anchors.centerIn: parent
            color: "#00e5b0"
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
            model: boardModel.moveModel

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
