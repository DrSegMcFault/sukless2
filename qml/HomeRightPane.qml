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

    RowLayout {
      spacing: 10

      DefaultText {
        id: label
        Layout.preferredWidth: 70
        font.pixelSize: 22
        font.bold: false
        color: "white"
      }

      Item {}

      DefaultText {
        id: one
        font.pixelSize: 22
        color: "white"
        Layout.preferredWidth: 30
      }

      Item { Layout.preferredWidth: 20 }
      Item { Layout.preferredWidth: 20 }

      DefaultText {
        id: two
        font.pixelSize: 22
        color: "white"
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
    spacing: 8

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

    Repeater {
      model: boardModel.moveModel
      delegate: RowEntry {
        Layout.alignment: Qt.AlignCenter
        Layout.fillWidth: true
        Layout.preferredHeight: 20
        color: base.color
        movenum: model.index + 1 + "."
        first: model.first
        second: model.second
      }
    }
  }
}
