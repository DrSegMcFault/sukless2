import QtQuick
import QtQuick.Layouts
import Sukless

import ".."
import "style"

Rectangle {
  id: base
  color: Style.buttonColor
  radius: 8
  border {
    width: Style.borderThickness
    color: Style.borderColor
  }

  property int moveFrom
  property int moveTo
  property int colorMoving

  signal selected(int from, int to, int value);

  ListModel {
    id: whiteModel
    ListElement {
      source: "qrc:/images/knight_white.svg"
      value: 2 // chess::Piece::WhiteKnight
    }
    ListElement {
      source: "qrc:/images/bishop_white.svg"
      value: 3 // chess::Piece::WhiteBishop
    }
    ListElement {
      source: "qrc:/images/rook_white.svg"
      value: 4 // chess::Piece::WhiteRook
    }
    ListElement {
      source: "qrc:/images/queen_white.svg"
      value: 5 // chess::Piece::WhiteQueen
    }
  }
  ListModel {
    id: blackModel
    ListElement {
      source: "qrc:/images/knight_black.svg"
      value: 8 // chess::Piece::BlackKnight
    }
    ListElement {
      source: "qrc:/images/bishop_black.svg"
      value: 9 // chess::Piece::BlackBishop
    }
    ListElement {
      source: "qrc:/images/rook_black.svg"
      value: 10 // chess::Piece::BlackRook
    }
    ListElement {
      source: "qrc:/images/queen_black.svg"
      value: 11 // chess::Piece::BlackQueen
    }
  }

  RowLayout {
    id: row
    anchors.fill: parent
    spacing: Style.borderThickness

    Repeater {
      model: base.colorMoving === Game.WHITE ? whiteModel : blackModel
      delegate: Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Image {
          anchors.centerIn: parent
          source: model.source
          width: parent.width * 9/10
          height: width
        }

        MouseArea {
          anchors.fill: parent
          onClicked: base.selected(base.moveFrom, base.moveTo, model.value)
        }
      }
    }
  }
}
