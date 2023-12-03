import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
  id: base
  function navigate(url, properties) {
    stack.replace(url, properties)
  }

  ColumnLayout {
    id: col
    anchors.fill: parent

    StackView {
      id: stack
      Layout.fillWidth: true
      Layout.fillHeight: true
      initialItem: "MainMenu.qml"

      popEnter: Transition {
        XAnimator { from: 0; to: 0; duration: 0; easing.type: Easing.OutCubic }
      }

      popExit:Transition {
        XAnimator { from: 0; to: 0; duration: 0; easing.type: Easing.OutCubic }
      }

      pushEnter: Transition {
        XAnimator { from: 0; to: 0; duration: 0; easing.type: Easing.OutCubic }
      }

      pushExit: Transition {
        XAnimator { from: 0; to: 0; duration: 0; easing.type: Easing.OutCubic }
      }

      replaceEnter: Transition {
        XAnimator { from: 0; to: 0; duration: 0; easing.type: Easing.OutCubic }
      }

      replaceExit: Transition {
        XAnimator { from: 0; to: 0; duration: 0; easing.type: Easing.OutCubic }
      }
    }
  }

}
