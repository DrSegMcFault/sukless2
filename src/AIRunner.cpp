#include "AIRunner.h"

#include <cassert>
#include <QEventLoop>
#include <QDebug>

/******************************************************************************
 *
 * Method: AIRunner()
 *
 *****************************************************************************/
AIRunner::AIRunner(QObject* parent)
 : QObject(parent)
 , _generator()
 , _manager(&_generator)
 , _ai(&_generator, { chess::AIDifficulty::Easy, chess::Black, false, false })
{
  _thread = std::thread([&]() {
    qDebug() << "AIRunner: Starting...\n";

    QEventLoop ev_loop;
    {
      QObject::connect(this, &AIRunner::shutdown,
                       &ev_loop, &QEventLoop::quit);

      QObject::connect(this, &AIRunner::gameStartReceived, &ev_loop,
        [&](chess::AIConfig settings, std::string fen)
      {
        qDebug() << "AIRunner: gameStartReceived\n";

        _manager.reset(fen);
        _ai.cfg = settings;

        if (_ai.enabled() && _manager.getSideToMove() == _ai.color()) {
          if (auto m = _ai.getBestMove(_manager)) {
            emit moveReady(*m, _ai.color());
          }
        }
      });

      QObject::connect(this, &AIRunner::moveRecieved, &ev_loop,
        [&](chess::HashedMove m, chess::Color color, chess::MoveResult r)
      {

        if (!_ai.enabled() && !_ai.assisting()) {
          qDebug() << "AIRunner: Move Received but not configured\n";
          return;
        }

        qDebug() << "AIRunner: Move recieved\n";

        // make the move on our copy of the board
        auto&& [ result, move_made ] = _manager.tryMove(m.toMove());

        // this should never assert but if it does, something is very wrong.
        // not sure how i want to recover from this yet.
        assert(m == move_made && r == result);

        qDebug() << "AIRunner: move is legal\n";

        if (color != _ai.color()) {

          qDebug() << "AIRunner: finding best move...\n";

          if (auto move = _ai.getBestMove(_manager)) {
            emit moveReady(*move, _ai.color());
          }

        }
        else if (_ai.assisting()) {

          if (auto move = _ai.getBestMove(_manager)) {
            emit suggestionReady(*move);
          }
        }
      });

      ev_loop.exec();
      ev_loop.disconnect(&ev_loop);
    }
  });
}

/******************************************************************************
 *
 * Method: ~AIRunner()
 *
 *****************************************************************************/
AIRunner::~AIRunner() {
  emit shutdown();

  qDebug() << "AIRunner: shutting down...\n";

  if (_thread.joinable()) {
    _thread.join();
  }
}

/******************************************************************************
 *
 * Method: onGameResetReceived()
 *
 *****************************************************************************/
void AIRunner::onGameStart(chess::AIConfig c, std::string fen)
{
  emit gameStartReceived(c, fen);
}

/******************************************************************************
 *
 * Method: onMoveConfirmed(chess::HashedMove, chess::Color, chess::MoveResult)
 *
 *****************************************************************************/
void AIRunner::onMoveConfirmed(chess::HashedMove m,
                               chess::Color c,
                               chess::MoveResult r)
{
  emit moveRecieved(m,c,r);
}
