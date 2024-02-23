#pragma once

#include <QObject>
#include <thread>
#include "util.hxx"
#include "BoardManager.hxx"
#include "MoveGenerator.hxx"
#include "AI.hxx"

class AIRunner : public QObject
{
  Q_OBJECT

private:
  chess::BoardManager _manager;
  chess::MoveGenerator _generator;
  chess::AI _ai;
  std::thread _thread;

public slots:
  void onMoveConfirmed(chess::HashedMove, chess::Color, chess::MoveResult);
  void onGameStart(chess::AIConfig c, std::string fen);

signals:
  // signals to route to this event loop
  void gameStartReceived(chess::AIConfig c, std::string fen);
  void moveRecieved(chess::HashedMove, chess::Color, chess::MoveResult);

  //  signals to be connected to
  void moveReady(chess::HashedMove, chess::Color);
  void suggestionReady(chess::HashedMove);
  void shutdown();

public:
  explicit AIRunner(QObject* parent = nullptr);
  ~AIRunner();

};
