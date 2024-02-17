#pragma once

#include <QAbstractListModel>
#include <QtQml>
#include <QList>
#include <array>
#include <optional>
#include <unordered_map>
#include <QUrl>

#include <QColor>

#include "util.hxx"

class BoardModel : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY(QColor color1 READ color1 WRITE setColor1 NOTIFY color1Changed)
  Q_PROPERTY(QColor color2 READ color2 WRITE setColor2 NOTIFY color2Changed)
  QML_ELEMENT

public:
  auto color1() { return _color1; }
  auto color2() { return _color2; }

  Q_INVOKABLE void setColor1(QColor c) {
    if (c != _color1) {
      _color1 = c;
      emit color1Changed();
    }
  }

  Q_INVOKABLE void setColor2(QColor c) {
    if (c != _color2) {
      _color2 = c;
      emit color2Changed();
    }
  }

  [[nodiscard]] inline int toInternalIndex(int index) const noexcept {
    int row = index / 8;
    int column = index % 8;

    switch (_visual_rotation) {
      case Rotation::ViewFromWhite:
        return  (7 - row) * 8 + column;
      case Rotation::ViewFromBlack:
        return  row * 8 + (7 - column);
    }
    return 0;
  }

public:

  enum Role {
    RolePiece = Qt::UserRole + 1,
    RoleIcon,
    RoleIndex,
    RoleRankLabel,
    RoleFileLabel
  };

  enum class Rotation {
    ViewFromWhite,
    ViewFromBlack
  };

signals:
  void color1Changed();
  void color2Changed();

protected:

  virtual QHash<int, QByteArray> roleNames() const override;

private:
  std::array<std::optional<chess::Piece>, 64> _data = {};

  QColor _color1 = QColor("white");
  QColor _color2 = QColor("#4284ed");

  Rotation _visual_rotation = Rotation::ViewFromWhite;

  const std::unordered_map<int, QString> _white_rank_map = {
    { 0,   QString("1") },
    { 8,   QString("2") },
    { 16,  QString("3") },
    { 24,  QString("4") },
    { 32,  QString("5") },
    { 40,  QString("6") },
    { 48,  QString("7") },
    { 56,  QString("8") }
  };

  const std::unordered_map<int, QString> _white_file_map = {
    { 0,  QString("a") },
    { 1,  QString("b") },
    { 2,  QString("c") },
    { 3,  QString("d") },
    { 4,  QString("e") },
    { 5,  QString("f") },
    { 6,  QString("g") },
    { 7,  QString("h") }
  };

  const std::unordered_map<int, QString> _black_file_map = {
    { 63,  QString("h") },
    { 62,  QString("g") },
    { 61,  QString("f") },
    { 60,  QString("e") },
    { 59,  QString("d") },
    { 58,  QString("c") },
    { 57,  QString("b") },
    { 56,  QString("a") },
  };

  const std::unordered_map<int, QString> _black_rank_map = {
    { 63,  QString("8") },
    { 55,  QString("7") },
    { 47,  QString("6") },
    { 39,  QString("5") },
    { 31,  QString("4") },
    { 23,  QString("3") },
    { 15,  QString("2") },
    { 7,   QString("1") }
  };

  const std::unordered_map<chess::Piece, QUrl> _icons = {
    { chess::WhitePawn,   QUrl("qrc:/images/pawn_white.png")   },
    { chess::WhiteKnight, QUrl("qrc:/images/knight_white.png") },
    { chess::WhiteBishop, QUrl("qrc:/images/bishop_white.png") },
    { chess::WhiteRook,   QUrl("qrc:/images/rook_white.png")   },
    { chess::WhiteQueen,  QUrl("qrc:/images/queen_white.png")  },
    { chess::WhiteKing,   QUrl("qrc:/images/king_white.png")   },
    { chess::BlackPawn,   QUrl("qrc:/images/pawn_black.png")   },
    { chess::BlackKnight, QUrl("qrc:/images/knight_black.png") },
    { chess::BlackBishop, QUrl("qrc:/images/bishop_black.png") },
    { chess::BlackRook,   QUrl("qrc:/images/rook_black.png")   },
    { chess::BlackQueen,  QUrl("qrc:/images/queen_black.png")  },
    { chess::BlackKing,   QUrl("qrc:/images/king_black.png")   }
  };

public:

  explicit BoardModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;

  Q_INVOKABLE void toggleRotation();

  void setBoard(std::array<std::optional<chess::Piece>, 64>&& b) {
    beginResetModel();
    _data = b;
    endResetModel();
  }

  void setRotation(chess::Color c) {
    beginResetModel();
    _visual_rotation = (c == chess::White) ? Rotation::ViewFromWhite
                                           : Rotation::ViewFromBlack;
    endResetModel();
  }

  void reset();
};
