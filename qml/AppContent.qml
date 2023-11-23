import QtQuick

Item {
  id: base
  Loader {
    id:stack
    anchors.fill: parent
    sourceComponent: NavStack {}
  }
}
