#pragma once

#include <QObject>
#include <QUrl>
#include <QtQml>
#include <memory>

#include "util.hxx"
#include "MoveGenerator.hxx"
#include "BoardManager.hxx"
#include "MoveModel.h"
#include "BoardModel.h"

class Game : public QObject
{
  Q_OBJECT
  Q_PROPERTY(MoveModel* moveModel READ moveModel CONSTANT)
  Q_PROPERTY(BoardModel* boardModel READ boardModel CONSTANT)

  QML_ELEMENT
  QML_UNCREATABLE("Game can't be created in qml")

public:

  enum D {
    EASY = chess::util::toul(chess::AIDifficulty::Easy),
    MEDIUM = chess::util::toul(chess::AIDifficulty::Medium),
    HARD = chess::util::toul(chess::AIDifficulty::Hard)
  };
  Q_ENUM(D)

  enum C {
    WHITE = chess::util::toul(chess::Color::White),
    BLACK = chess::util::toul(chess::Color::Black)
  };
  Q_ENUM(C)

public:
  auto moveModel() { return &_move_model; }
  auto boardModel() { return &_board_model; }

signals:
  void playSound(QUrl s);
  void gameOver(QString text);
  void promotionSelect(int from, int to, int color);
  void moveConfirmed(chess::HashedMove, chess::Color c, chess::MoveResult m);
  void gameStart(chess::AIConfig, std::string fen);

public slots:
  void onMoveReady(chess::HashedMove, chess::Color);
  void onSuggestionReady(chess::HashedMove);

private:
  chess::MoveGenerator _generator;

  MoveModel _move_model;
  BoardModel _board_model;
  std::shared_ptr<chess::BoardManager> _board_manager;

  // to keep track of the index in the move_model that is
  // currently being displayed
  size_t _current_move_index = 1;

  // to keep track of the current board being displayed from
  // the history in board_manager. after any legal move is
  // made this should be incremented.
  size_t _current_history_index = 0;

  const QUrl _move_sound = QUrl("qrc:/sounds/move.mp3");
  const QUrl _game_end_sound = QUrl("qrc:/sounds/game_end.mp3");
  const QUrl _illegal_sound = QUrl("qrc:/sounds/illegal_move.mp3");

  const std::unordered_map<chess::MoveResult, QUrl> _result_to_sound = {
    { chess::MoveResult::Illegal,   _illegal_sound },
    { chess::MoveResult::Valid,     _move_sound },
    { chess::MoveResult::Check,     _move_sound },
    { chess::MoveResult::Checkmate, _game_end_sound },
    { chess::MoveResult::Stalemate, _game_end_sound },
    { chess::MoveResult::Draw,      _game_end_sound }
  };

  const std::unordered_map<int, float> piece_values = {
    { chess::util::toul(chess::Piece::WhitePawn),   1.0 },
    { chess::util::toul(chess::Piece::WhiteKnight), 3.0 },
    { chess::util::toul(chess::Piece::WhiteBishop), 3.0 },
    { chess::util::toul(chess::Piece::WhiteRook),   5.0 },
    { chess::util::toul(chess::Piece::WhiteQueen),  9.0 },
    { chess::util::toul(chess::Piece::WhiteKing),   0.0 },

    { chess::util::toul(chess::Piece::BlackPawn),   1.0 },
    { chess::util::toul(chess::Piece::BlackKnight), 3.0 },
    { chess::util::toul(chess::Piece::BlackBishop), 3.0 },
    { chess::util::toul(chess::Piece::BlackRook),   5.0 },
    { chess::util::toul(chess::Piece::BlackQueen),  9.0 },
    { chess::util::toul(chess::Piece::BlackKing),   0.0 }
  };

  void afterMove(chess::MoveResult, chess::HashedMove);

public:
  explicit Game(QObject* parent = nullptr);

  Q_INVOKABLE void init(chess::Color user,
                        bool engineHelp,
                        bool aiEnable,
                        chess::AIDifficulty d);

  Q_INVOKABLE void handleMove(int from, int to, int promoted_piece);

  // reset members to default state
  Q_INVOKABLE void reset();

  // show the previous move
  Q_INVOKABLE void showPrevious();

  // show the next move
  Q_INVOKABLE void showNext();
};
