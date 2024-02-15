#pragma once

#include <QObject>
#include <QUrl>
#include <QThread>
#include <memory>

#include <QtQml>
#include "util.hxx"
#include "MoveGen.hxx"
#include "BoardManager.hxx"
#include "MoveModel.h"
#include "BoardModel.h"

using namespace chess;

class Game : public QObject
{
  Q_OBJECT
  Q_PROPERTY(MoveModel* moveModel READ moveModel CONSTANT)
  Q_PROPERTY(BoardModel* boardModel READ boardModel CONSTANT)
  QML_ELEMENT

public:
  auto moveModel() { return &_moveModel; }
  auto boardModel() { return &_boardModel; }

  struct GameSettings {
    Color _user_color;
    AIConfig _ai_settings;
  };

signals:
  void playSound(QUrl s);
  void moveConfirmed(int from, int to);
  void gameOver(QString text);
  void promotionSelect();

private:
  MoveModel _moveModel;
  BoardModel _boardModel;

  std::shared_ptr<BoardManager> _game;
  QThread ai_runner;
  static constexpr MoveGen _generator;

  GameSettings _settings;

  const QUrl _move_sound = QUrl("qrc:/sounds/move.mp3");
  const QUrl _game_end_sound = QUrl("qrc:/sounds/game_end.mp3");
  const QUrl _illegal_sound = QUrl("qrc:/sounds/illegal_move.mp3");

  const std::unordered_map<MoveResult, QUrl> _result_to_sound = {
    { MoveResult::Valid,     _move_sound },
    { MoveResult::Illegal,   _illegal_sound },
    { MoveResult::Check,     _move_sound },
    { MoveResult::Checkmate, _game_end_sound },
    { MoveResult::Stalemate, _game_end_sound },
    { MoveResult::Draw,      _game_end_sound }
  };

public:
  explicit Game(QObject* parent = nullptr);

  Q_INVOKABLE void handleMove(int from, int to, int promoted_piece);
  Q_INVOKABLE void reset();
  Q_INVOKABLE void init(Color user, bool engineHelp, bool aiEnable, int aiDifficulty);
};
