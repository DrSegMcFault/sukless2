#pragma once

#include <QAbstractListModel>
#include <QList>
#include <array>
#include <optional>
#include <memory>
#include <unordered_map>
#include <QUrl>
#include <vector>
#include <QString>
#include <QColor>

#include "BoardManager.hxx"
#include "util.hxx"
#include "MoveModel.h"

using namespace chess;

class BoardModel : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY(MoveModel* moveModel READ moveModel CONSTANT)

public:
  auto moveModel() { return &_moveModel; }

public:

  Q_ENUM(Color)
  Q_ENUM(AIDifficulty);

  enum Role {
    RolePiece = Qt::UserRole + 1,
    RoleIcon,
    RoleIndex,
    RolePossibleMove,
    RoleRankLabel,
    RoleFileLabel
  };

  enum class Rotation {
    ViewFromWhite,
    ViewFromBlack
  };

signals:

  void playSound(QUrl sound);
  void gameOver(QString text);
  void moveModelChanged();

protected:

  virtual QHash<int, QByteArray> roleNames() const override;

private:
  MoveModel _moveModel;

  std::array<std::optional<Piece>, 64> _data = {};
  std::vector<uint8_t> _possible_moves = {};

  std::shared_ptr<BoardManager> _game;

  static constexpr MoveGen _generator;

  Rotation _visual_rotation = Rotation::ViewFromWhite;
  Color _user_color = Color::white;
  AIDifficulty _ai_strength = AIDifficulty::Easy;

  bool _ai_enabled = false;
  bool _ai_assist_enabled = false;

  [[nodiscard]] inline int toInternalIndex(int index) const {
    int row = index / 8;
    int column = index % 8;

    switch (_visual_rotation) {
      case Rotation::ViewFromWhite:
        return  (7 - row) * 8 + column;
      case Rotation::ViewFromBlack:
        return  row * 8 + (7 - column);
    }
  }

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
    { Piece::w_pawn,   QUrl("qrc:/resources/pawn_white.png")   },
    { Piece::w_knight, QUrl("qrc:/resources/knight_white.png") },
    { Piece::w_bishop, QUrl("qrc:/resources/bishop_white.png") },
    { Piece::w_rook,   QUrl("qrc:/resources/rook_white.png")   },
    { Piece::w_queen,  QUrl("qrc:/resources/queen_white.png")  },
    { Piece::w_king,   QUrl("qrc:/resources/king_white.png")   },
    { Piece::b_pawn,   QUrl("qrc:/resources/pawn_black.png")   },
    { Piece::b_knight, QUrl("qrc:/resources/knight_black.png") },
    { Piece::b_bishop, QUrl("qrc:/resources/bishop_black.png") },
    { Piece::b_rook,   QUrl("qrc:/resources/rook_black.png")   },
    { Piece::b_queen,  QUrl("qrc:/resources/queen_black.png")  },
    { Piece::b_king,   QUrl("qrc:/resources/king_black.png")   }
  };

  const QUrl _move_sound = QUrl("qrc:/resources/move_sound.mp3");
  const QUrl _game_end_sound = QUrl("qrc:/resources/game_end.mp3");
  const QUrl _illegal_sound = QUrl("qrc:/resources/illegal_move.mp3");

  const std::unordered_map<MoveResult, QUrl> _move_to_sound = {
    { MoveResult::Valid,     _move_sound },
    { MoveResult::Illegal,   _illegal_sound },
    { MoveResult::Check,     _move_sound },
    { MoveResult::Checkmate, _game_end_sound },
    { MoveResult::Stalemate, _game_end_sound },
    { MoveResult::Draw,      _game_end_sound }
  };

public:

  explicit BoardModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;

  Q_INVOKABLE void move(int from, int to);

  Q_INVOKABLE void toggleRotation();

  Q_INVOKABLE void boardClicked(int index);

  Q_INVOKABLE void reset();

  Q_INVOKABLE void init(Color user, bool engine_assist, bool ai_enable, AIDifficulty diff);

};
