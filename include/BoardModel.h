#pragma once

#include <QAbstractListModel>
#include <QtQml>
#include <QList>
#include <array>
#include <optional>
#include <unordered_map>
#include <QUrl>
#include <QColor>

#include "engine/ChessTypes.hxx"

class BoardModel : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY(QColor color1 MEMBER _color1 NOTIFY color1Changed)
  Q_PROPERTY(QColor color2 MEMBER _color2 NOTIFY color2Changed)
  Q_PROPERTY(float topEval MEMBER _top_diff NOTIFY topEvalChanged)
  Q_PROPERTY(float bottomEval MEMBER _bottom_diff NOTIFY bottomEvalChanged)
  Q_PROPERTY(Rotation rotation MEMBER _visual_rotation NOTIFY rotationChanged)
  QML_ELEMENT

public:
  [[nodiscard]] inline int toInternalIndex(int index) const noexcept {
    if (index < 0) {
      index = 0;
    }
    int row = index / 8;
    int column = index % 8;

    switch (_visual_rotation) {
      case Rotation::ViewFromWhite:
        return (7 - row) * 8 + column;
      case Rotation::ViewFromBlack:
        return row * 8 + (7 - column);
    }
    return 0;
  }

  enum Role {
    RolePiece = Qt::UserRole + 1,
    RoleIcon,
    RoleIndex,
    RoleRankLabel,
    RoleFileLabel
  };

  enum Rotation {
    ViewFromWhite,
    ViewFromBlack
  };
  Q_ENUM(Rotation)


signals:
  void color1Changed();
  void color2Changed();
  void rotationChanged();
  void topEvalChanged();
  void bottomEvalChanged();

protected:

  virtual QHash<int, QByteArray> roleNames() const override;

private:
  std::array<std::optional<chess::Piece>, 64> _data = {};

  QColor _color1 = QColor("white");
  QColor _color2 = QColor("#4284ed");

  Rotation _visual_rotation = Rotation::ViewFromWhite;

  float _top_diff = 0;
  float _bottom_diff = 0;
  float _white_diff = 0;
  float _black_diff = 0;

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
    { chess::WhitePawn,   QUrl("qrc:/images/pawn_white.svg")   },
    { chess::WhiteKnight, QUrl("qrc:/images/knight_white.svg") },
    { chess::WhiteBishop, QUrl("qrc:/images/bishop_white.svg") },
    { chess::WhiteRook,   QUrl("qrc:/images/rook_white.svg")   },
    { chess::WhiteQueen,  QUrl("qrc:/images/queen_white.svg")  },
    { chess::WhiteKing,   QUrl("qrc:/images/king_white.svg")   },
    { chess::BlackPawn,   QUrl("qrc:/images/pawn_black.svg")   },
    { chess::BlackKnight, QUrl("qrc:/images/knight_black.svg") },
    { chess::BlackBishop, QUrl("qrc:/images/bishop_black.svg") },
    { chess::BlackRook,   QUrl("qrc:/images/rook_black.svg")   },
    { chess::BlackQueen,  QUrl("qrc:/images/queen_black.svg")  },
    { chess::BlackKing,   QUrl("qrc:/images/king_black.svg")   }
  };

public:

  explicit BoardModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;

  Q_INVOKABLE void toggleRotation();

  void updateEvals(float white, float black) {
    _white_diff = white;
    _black_diff = black;
    onRotationChanged();
  }

  void onRotationChanged() {
    switch (_visual_rotation)  {
      case Rotation::ViewFromWhite:
      {
        _top_diff = _black_diff;
        _bottom_diff = _white_diff;
        break;
      }
      case Rotation::ViewFromBlack:
      {
        _top_diff = _white_diff;
        _bottom_diff = _black_diff;
        break;
      }
    }

    emit topEvalChanged();
    emit bottomEvalChanged();
  }

  void setBoard(std::array<std::optional<chess::Piece>, 64>&& b) {
    beginResetModel();
    _data = b;
    endResetModel();
  }

  Q_INVOKABLE void setRotation(chess::Color c) {
    beginResetModel();
    _visual_rotation = (c == chess::White) ? Rotation::ViewFromWhite
                                           : Rotation::ViewFromBlack;
    endResetModel();
    emit rotationChanged();
  }

  void reset();
};
