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
      source: "qrc:/images/knight_white.png"
      value: 2 // chess::Piece::WhiteKnight
    }
    ListElement {
      source: "qrc:/images/bishop_white.png"
      value: 3 // chess::Piece::WhiteBishop
    }
    ListElement {
      source: "qrc:/images/rook_white.png"
      value: 4 // chess::Piece::WhiteRook
    }
    ListElement {
      source: "qrc:/images/queen_white.png"
      value: 5 // chess::Piece::WhiteQueen
    }
  }
  ListModel {
    id: blackModel
    ListElement {
      source: "qrc:/images/knight_black.png"
      value: 8 // chess::Piece::BlackKnight
    }
    ListElement {
      source: "qrc:/images/bishop_black.png"
      value: 9 // chess::Piece::BlackBishop
    }
    ListElement {
      source: "qrc:/images/rook_black.png"
      value: 10 // chess::Piece::BlackRook
    }
    ListElement {
      source: "qrc:/images/queen_black.png"
      value: 11 // chess::Piece::BlackQueen
    }
  }
  RowLayout {
    id: row
    anchors.fill: parent
    spacing: 2

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
