import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import BoardModel 1.0

Item {
  id: base
  anchors.fill: parent

  Rectangle {
    color: "#2C2C2C"
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

          Text {
            text: qsTr("Sukless2")
            anchors.centerIn: parent
            font {
              family: "helvetica"
              pixelSize: 48
              bold: true
            }
            color: "#00e5b0"
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

            Text {
              text: qsTr("Play as")
              anchors.centerIn: parent
              font {
                family: "helvetica"
                pixelSize: 24
                bold: true
              }
              color: "#00e5b0"
            }
          }

          Rectangle {
            id: leftOption
            Layout.preferredWidth: 80
            Layout.preferredHeight: 40
            Layout.alignment: Qt.AlignVCenter
            color: "#2C2C2C"
            radius: 8
            property bool selected: true
            border {
              color: selected ? "#00e5b0" : "black"
              width: 2
            }

            MouseArea {
              anchors.fill: parent
              onClicked: {
                if (rightOption.selected) {
                  rightOption.selected = false
                }
                parent.selected = true
              }
            }

            Text {
              text: qsTr("White")
              color: "white"
              anchors.centerIn: parent
              font.pixelSize: 22
              font.bold: true
              font.family: "helvetica"
            }
          }

          Rectangle {
            id: rightOption
            Layout.preferredWidth: 80
            Layout.preferredHeight: 40
            color: "#2C2C2C"
            radius: 8
            property bool selected: false
            border {
              color: selected ? "#00e5b0" : "black"
              width: 2
            }

            MouseArea {
              anchors.fill: parent
              onClicked: {
                if (leftOption.selected) {
                  leftOption.selected = false
                }
                parent.selected = true
              }
            }

            Text {
              text: qsTr("Black")
              color: "white"
              anchors.centerIn: parent
              font.pixelSize: 22
              font.bold: true
              font.family: "helvetica"
            }
          }
        }

        Item {
          Layout.preferredHeight: 30
          Layout.fillWidth: true
          Layout.alignment: Qt.AlignLeft

          Text {
            text: qsTr("AI Settings")
            anchors.left: parent.left

            font {
              family: "helvetica"
              pixelSize: 24
              bold: true
            }
            color: "#00e5b0"
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
            Text {
              text: qsTr("Enabled")
              color: "#00e5b0"
              font {
                family: "helvetica"
                pixelSize: 20
                bold: true
              }
            }
            Rectangle {
              id: aiFalse
              color: "#2C2C2C"
              property bool selected: true
              Layout.fillHeight: true
              Layout.preferredWidth: 50
              radius: 8
              border {
                width: 2
                color: selected ? "#00e5b0" : "black"
              }
              MouseArea {
                anchors.fill: parent
                onClicked: {
                  if (!parent.selected) {
                    parent.selected = true
                    aiTrue.selected = false
                    aiEnabled.value = false
                  }
                }
              }

              Text {
                text: qsTr("No")
                color: "white"
                anchors.centerIn: parent
                font {
                  family: "helvetica"
                  pixelSize: 20
                  bold: true
                }
              }
            }

            Rectangle {
              id: aiTrue
              color: "#2C2C2C"
              property bool selected: false
              Layout.fillHeight: true
              Layout.preferredWidth: 50
              radius: 8
              border {
                width: 2
                color: selected ? "#00e5b0" : "black"
              }

              MouseArea {
                anchors.fill: parent
                onClicked: {
                  if (!parent.selected) {
                    parent.selected = true
                    aiFalse.selected = false
                    aiEnabled.value = true
                  }
                }
              }
              Text {
                text: qsTr("Yes")
                color: "white"
                anchors.centerIn: parent
                font {
                  family: "helvetica"
                  pixelSize: 20
                  bold: true
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

            Text {
              text: qsTr("Engine Assistance")
              color: "#00e5b0"
              font {
                family: "helvetica"
                pixelSize: 20
                bold: true
              }
            }

            Rectangle {
              id: engineFalse
              color: "#2C2C2C"
              property bool selected: true
              Layout.fillHeight: true
              Layout.preferredWidth: 50
              radius: 8
              border {
                width: 2
                color: selected ? "#00e5b0" : "black"
              }

              MouseArea {
                anchors.fill: parent
                onClicked: {
                  if (!parent.selected) {
                    parent.selected = true
                    engineTrue.selected = false
                    enabledAiAssist.value = false
                  }
                }
              }

              Text {
                text: qsTr("No")
                color: "white"
                anchors.centerIn: parent
                font {
                  family: "helvetica"
                  pixelSize: 20
                  bold: true
                }
              }
            }

            Rectangle {
              id: engineTrue
              color: "#2C2C2C"
              property bool selected: false
              Layout.fillHeight: true
              Layout.preferredWidth: 50
              radius: 8
              border {
                width: 2
                color: selected ? "#00e5b0" : "black"
              }

              MouseArea {
                anchors.fill: parent
                onClicked: {
                  if (!parent.selected) {
                    parent.selected = true
                    engineFalse.selected = false
                    enabledAiAssist.value = true
                  }
                }
              }
              Text {
                text: qsTr("Yes")
                color: "white"
                anchors.centerIn: parent
                font {
                  family: "helvetica"
                  pixelSize: 20
                  bold: true
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
            Text {
              text: qsTr("Difficulty")
              color: "#00e5b0"
              font {
                family: "helvetica"
                pixelSize: 20
                bold: true
              }
            }
            Rectangle {
              id: aiEasy
              property bool selected: true
              Layout.fillHeight: true
              Layout.preferredWidth: 75
              radius: 8
              color: "#2C2C2C"
              border {
                width: 2
                color: selected ? "#00e5b0" : "black"
              }
              MouseArea {
                anchors.fill: parent
                onClicked: {
                  if (!parent.selected) {
                    parent.selected = true
                    aiMedium.selected = false
                    aiHard.selected = false
                    aiDifficulty.value = BoardModel.Easy
                  }
                }
              }

              Text {
                text: qsTr("Easy")
                color: "white"
                anchors.centerIn: parent
                font {
                  family: "helvetica"
                  pixelSize: 20
                  bold: true
                }
              }
            }
            Rectangle {
              id: aiMedium
              property bool selected: false
              Layout.fillHeight: true
              Layout.preferredWidth: 100
              color: "#2C2C2C"
              radius: 8
              border {
                width: 2
                color: selected ? "#00e5b0" : "black"
              }
              MouseArea {
                anchors.fill: parent
                onClicked: {
                  if (!parent.selected) {
                    parent.selected = true
                    aiEasy.selected = false
                    aiHard.selected = false
                    aiDifficulty.value = BoardModel.Medium
                  }
                }
              }

              Text {
                text: qsTr("Medium")
                color: "white"
                anchors.centerIn: parent
                font {
                  family: "helvetica"
                  pixelSize: 20
                  bold: true
                }
              }
            }

            Rectangle {
              id: aiHard
              property bool selected: false
              Layout.fillHeight: true
              Layout.preferredWidth: 75
              radius: 8
              color: "#2C2C2C"
              border {
                width: 2
                color: selected ? "#00e5b0" : "black"
              }
              MouseArea {
                anchors.fill: parent
                onClicked: {
                  if (!parent.selected) {
                    parent.selected = true
                    aiEasy.selected = false
                    aiMedium.selected = false
                    aiDifficulty.value = BoardModel.Hard
                  }
                }
              }

              Text {
                text: qsTr("Hard")
                color: "white"
                anchors.centerIn: parent
                font {
                  family: "helvetica"
                  pixelSize: 20
                  bold: true
                }
              }
            }
          }
        }
      }

      Rectangle {
        color: "#00e5b0"
        height: 40
        radius: 8
        anchors {
          left: parent.left
          right: parent.right
          bottom: parent.bottom
          leftMargin: 12
          rightMargin: 12
          bottomMargin: 8
        }

        MouseArea {
          anchors.fill: parent
          onClicked: { navigate(
                         "Home.qml", {
                           "userColor": leftOption.selected ? BoardModel.White : BoardModel.Black,
                           "aiAssistEnabled": enabledAiAssist.value,
                           "aiEnabled": aiEnabled.value,
                           "aiDifficulty": aiDifficulty.value
                         })
          }
        }

        Text {
          id: textInfo
          text: qsTr("Go")
          anchors.centerIn: parent

          font {
            family: "helvetica"
            pixelSize: 24
            bold: true
          }
          color: "black"
        }
      }
    }
  }
}
