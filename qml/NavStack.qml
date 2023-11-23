import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../"

Item {
  id: base
  function navigate() {

  }

  ColumnLayout {
    id: col
    anchors.fill: parent
    StackView {
      id: stack
      Layout.fillWidth: true
      Layout.fillHeight: true
      initialItem: "Home.qml"
    }
  }

}
