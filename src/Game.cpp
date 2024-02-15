#include "Game.h"

/******************************************************************************
 *
 * Method: Game()
 *
 *****************************************************************************/
Game::Game(QObject *parent)
    : QObject(parent)
{
  reset();
}

/******************************************************************************
 *
 * Method: reset()
 *
 *****************************************************************************/
void Game::reset()
{
  _game = std::make_shared<BoardManager>(&_generator);
  _boardModel.setBoard(_game->get_current_board());
  _moveModel.clear();
}


/******************************************************************************
 *
 * Method: init()
 *
 *****************************************************************************/
void Game::init(chess::Color user, bool engine_help,
                bool ai_enable, int /*aidiff*/)
{
}

/******************************************************************************
 *
 * Method: handleMove()
 *
 *****************************************************************************/
void Game::handleMove(int from, int to, int promoted_piece)
{
  QUrl sound_to_play;

  auto source = static_cast<uint8_t>(_boardModel.toInternalIndex(from));
  auto target = static_cast<uint8_t>(_boardModel.toInternalIndex(to));

  std::optional<Piece> promoted;
  if (promoted_piece >= 0) {
    promoted = static_cast<Piece>(promoted_piece);
  }

  const auto [result, move_made] = _game->try_move({ source, target, promoted });

  sound_to_play = _result_to_sound.at(result);

  if (result != MoveResult::Illegal) {

    _moveModel.addEntry({ move_made,
                          _game->get_side_to_move() == Color::white ? Color::black : Color::white,
                          result });

    emit moveConfirmed(from, to);

    // end of game conditions
    switch (result) {
      case MoveResult::Checkmate:
      {
        emit gameOver(_game->get_side_to_move() == Color::black
                           ? QString(tr("White Wins!"))
                           : QString(tr("Black Wins!")));
        break;
      }

      case MoveResult::Stalemate:
      {
        emit gameOver(QString(tr("Stalemate!")));
        break;
      }

      case MoveResult::Draw:
      {
        emit gameOver(QString(tr("Draw by Repetition!")));
        break;
      }

      default:
        break;
    }
  }

  _boardModel.setBoard(std::move(_game->get_current_board()));

  emit playSound(sound_to_play);
}
