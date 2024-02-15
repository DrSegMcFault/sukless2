import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Sukless

import "../"
import "Style"

Item {
  id: base
  anchors.fill: parent

  Rectangle {
    color: Style.background
    anchors.fill: parent

    Rectangle {
      color: "black"
      height: parent.height * 3/8
      width: parent.width * 3/8
      anchors.centerIn: parent

      radius: 8

      ColumnLayout {
        anchors {
          left: parent.left
          right: parent.right
          top: parent.top
          leftMargin: 12
          rightMargin: 12
          topMargin: 4
        }
        spacing: 4

        Item {
          Layout.preferredHeight: 60
          Layout.fillWidth: true
          Layout.alignment: Qt.AlignCenter

          DefaultText {
            text: qsTr("Sukless2")
            anchors.centerIn: parent
            font.pixelSize: 48
            color: Style.teal
          }
        }
        RowLayout {
          Layout.preferredHeight: 50
          Layout.fillWidth: true
          spacing: 10

          Item {
            Layout.fillHeight: true
            Layout.preferredWidth: 80
            Layout.alignment: Qt.AlignVCenter

            DefaultText {
              text: qsTr("Play as")
              anchors.centerIn: parent
              font.pixelSize: 24
              color: Style.teal
            }
          }

          PillButton {
            id: leftOption
            Layout.preferredWidth: 80
            Layout.preferredHeight: 35
            color: Style.background

            property bool selected: true

            text: qsTr("White")
            textColor: "white"
            pixelSize: 22

            border {
              color: selected ? Style.teal : "black"
              width: 2
            }

            onClicked: {
              if (rightOption.selected) {
                rightOption.selected = false
              }
              selected = true
            }
          }

          PillButton {
            id: rightOption
            Layout.preferredWidth: 80
            Layout.preferredHeight: 35

            color: Style.background
            text: qsTr("Black")
            textColor: "white"
            pixelSize: 22

            property bool selected: false

            border {
              color: selected ? Style.teal : "black"
              width: 2
            }

            onClicked: {
              if (leftOption.selected) {
                leftOption.selected = false
              }
              selected = true
            }
          }
        }

        Item {
          Layout.preferredHeight: 30
          Layout.fillWidth: true
          Layout.alignment: Qt.AlignLeft

          DefaultText {
            text: qsTr("AI Settings")
            anchors.left: parent.left
            font.pixelSize: 24
            color: Style.teal
          }
        }

        Item {
          id: aiEnabled
          Layout.fillWidth: true
          Layout.preferredHeight: 30
          property bool value: false

          RowLayout {
            spacing: 4
            anchors.left: parent.left
            anchors.leftMargin: 12
            height: parent.height
            DefaultText {
              text: qsTr("Enabled")
              color: Style.teal
              font.pixelSize: 20
            }

            PillButton {
              id: aiFalse
              Layout.fillHeight: true
              Layout.preferredWidth: 50

              color: Style.background
              property bool selected: true

              text: qsTr("No")
              textColor: "white"
              pixelSize: 20

              border {
                width: 2
                color: selected ? Style.teal : "black"
              }

              onClicked: {
                if (!selected) {
                  selected = true
                  aiTrue.selected = false
                  aiEnabled.value = false
                }
              }
            }

            PillButton {
              id: aiTrue
              Layout.fillHeight: true
              Layout.preferredWidth: 50

              color: Style.background
              property bool selected: false

              text: qsTr("Yes")
              textColor: "white"
              pixelSize: 20

              border {
                width: 2
                color: selected ? Style.teal : "black"
              }

              onClicked: {
                if (!selected) {
                  selected = true
                  aiFalse.selected = false
                  aiEnabled.value = true
                }
              }
            }
          }
        }

        Item {
          id: enabledAiAssist
          Layout.fillWidth: true
          Layout.preferredHeight: 30
          property bool value: false

          RowLayout {
            spacing: 4
            anchors.left: parent.left
            anchors.leftMargin: 12
            height: parent.height

            CenteredText {
              text: qsTr("Engine Assistance")
              color: Style.teal
              font.pixelSize: 20
            }

            PillButton {
              id: engineFalse

              Layout.fillHeight: true
              Layout.preferredWidth: 50
              color: Style.background
              text: qsTr("No")
              textColor: "white"
              pixelSize: 20

              property bool selected: true

              border {
                width: 2
                color: selected ? Style.teal : "black"
              }

              onClicked: {
                if (!selected) {
                  selected = true
                  engineTrue.selected = false
                  enabledAiAssist.value = false
                }
              }
            }


            PillButton {
              id: engineTrue

              Layout.fillHeight: true
              Layout.preferredWidth: 50
              color: Style.background

              text: qsTr("Yes")
              textColor: "white"
              pixelSize: 20

              property bool selected: false

              border {
                width: 2
                color: selected ? Style.teal : "black"
              }

              onClicked: {
                if (!selected) {
                  selected = true
                  engineFalse.selected = false
                  enabledAiAssist.value = true
                }
              }
            }
          }
        }

        Item {
          id: aiDifficulty
          Layout.fillWidth: true
          Layout.preferredHeight: 30
          property int value: BoardModel.Easy

          visible: aiEnabled.value === true

          RowLayout {
            spacing: 4
            anchors.left: parent.left
            anchors.leftMargin: 12
            height: 30
            property bool value: false

            DefaultText {
              text: qsTr("Difficulty")
              color: Style.teal
              font.pixelSize: 20
            }

            PillButton {
              id: aiEasy
              Layout.fillHeight: true
              Layout.preferredWidth: 75

              color: Style.background
              property bool selected: true
              text: qsTr("Easy")
              textColor: "white"
              pixelSize: 20

              border {
                width: 2
                color: selected ? Style.teal : "black"
              }

              onClicked: {
                if (!selected) {
                  selected = true
                  aiMedium.selected = false
                  aiHard.selected = false
                  aiDifficulty.value = BoardModel.Easy
                }
              }
            }

            PillButton {
              id: aiMedium
              Layout.fillHeight: true
              Layout.preferredWidth: 100

              color: Style.background
              property bool selected: false
              text: qsTr("Medium")
              textColor: "white"
              pixelSize: 20

              border {
                width: 2
                color: selected ? Style.teal : "black"
              }

              onClicked: {
                if (!selected) {
                  selected = true
                  aiEasy.selected = false
                  aiHard.selected = false
                  aiDifficulty.value = BoardModel.Medium
                }
              }
            }

            PillButton {
              id: aiHard
              Layout.fillHeight: true
              Layout.preferredWidth: 75

              color: Style.background
              property bool selected: false
              text: qsTr("Hard")
              textColor: "white"
              pixelSize: 20

              border {
                width: 2
                color: selected ? Style.teal : "black"
              }

              onClicked: {
                if (!selected) {
                  selected = true
                  aiEasy.selected = false
                  aiMedium.selected = false
                  aiDifficulty.value = BoardModel.Hard
                }
              }
            }
          }
        }
      }

      PillButton {
        height: 40

        anchors {
          left: parent.left
          right: parent.right
          bottom: parent.bottom
          leftMargin: 12
          rightMargin: 12
          bottomMargin: 8
        }

        text: qsTr("Go")
        pixelSize: 24

        onClicked: { navigate(
                       "Home.qml", {
                         "userColor": leftOption.selected ? 0 : 1,
                         "aiAssistEnabled": enabledAiAssist.value,
                         "aiEnabled": aiEnabled.value,
                         "aiDifficulty": aiDifficulty.value
                       })
        }
      }
    }
  }
}
