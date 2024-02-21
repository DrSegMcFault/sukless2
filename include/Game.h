#pragma once

#include <QObject>
#include <QUrl>
#include <QThread>
#include <memory>
#include <QtQml>

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

  struct GameSettings {
    chess::Color _user_color;
    chess::AIConfig _ai_settings;
  };

signals:
  void playSound(QUrl s);
  void moveConfirmed(int from, int to);
  void gameOver(QString text);
  void promotionSelect(int from, int to, int color);

private:
  static constexpr chess::MoveGenerator _generator;

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

  GameSettings _settings;

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

public:
  explicit Game(QObject* parent = nullptr);

  Q_INVOKABLE void handleMove(int from, int to, int promoted_piece);
  Q_INVOKABLE void reset();
  Q_INVOKABLE void init(chess::Color user,
                        bool engineHelp,
                        bool aiEnable,
                        chess::AIDifficulty d);

  Q_INVOKABLE void showPrevious() {

    if (_current_history_index >= 1) {
      --_current_history_index;

      if (auto fen = _board_manager->historyAt(_current_history_index)) {

        if (_current_move_index >= 1 && _current_history_index % 2 == 0) {
          _move_model.setSelected(--_current_move_index);
        }

        auto opt = _board_manager->makeBoardFromFen(*fen);
        auto [board, state] = *opt;

        _board_model.setBoard(std::move(chess::to_array(board)));
      }
    }
  }

  Q_INVOKABLE void showNext() {

    if (auto fen = _board_manager->historyAt(_current_history_index + 1)) {

      _current_history_index++;

      if (_current_move_index + 1 <= _move_model.dataCount() -1) {
        if (_current_history_index > 1 && _current_history_index % 2 != 0) {
          _move_model.setSelected(++_current_move_index);
        }
      }

      auto opt = _board_manager->makeBoardFromFen(*fen);
      auto [board, state] = *opt;

      _board_model.setBoard(std::move(chess::to_array(board)));
    }
  }
};
