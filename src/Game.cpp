#include "Game.h"

/******************************************************************************
 *
 * Method: Game()
 *
 *****************************************************************************/
Game::Game(QObject *parent)
  : QObject(parent)
{
  _board_manager = std::make_shared<chess::BoardManager>(&_generator);
  _board_model.setBoard(_board_manager->toArray());
  _move_model.clear();
}

/******************************************************************************
 *
 * Method: reset()
 *
 *****************************************************************************/
void Game::reset()
{
  _board_manager->reset();
  _board_model.setBoard(_board_manager->toArray());
  _move_model.clear();
}

/******************************************************************************
 *
 * Method: init()
 *
 *****************************************************************************/
void Game::init(chess::Color user, bool engine_help,
                bool /*ai_enable*/, chess::AIDifficulty aidiff)
{
  _board_model.setRotation(user);
  _settings._user_color = user;

  auto& ai_cfg = _settings._ai_settings;
  ai_cfg.difficulty = aidiff;
  ai_cfg.controlling = (user == chess::White) ? chess::Black : chess::White;
  ai_cfg.assisting_user = engine_help;
}

/******************************************************************************
 *
 * Method: handleMove()
 *
 *****************************************************************************/
void Game::handleMove(int from, int to, int promoted_piece)
{
  QUrl sound_to_play;

  auto source = static_cast<uint8_t>(_board_model.toInternalIndex(from));
  auto target = static_cast<uint8_t>(_board_model.toInternalIndex(to));

  auto color_moved = _board_manager->getSideToMove();

  auto will_promote = [&] () -> bool {
    if (auto p = _board_manager->pieceAt(source)) {
      if (*p == chess::WhitePawn) {
        if (target >= chess::A8 && target <= chess::H8) {
           return true;

        }
      } else if (*p == chess::BlackPawn) {
        if (target >= chess::A1 && target <= chess::H1) {
          return true;
        }
      }
    }
    return false;
  };

  if (promoted_piece == 0 &&
      will_promote())
  {
    emit promotionSelect(from, to, chess::util::toul(color_moved));
    return;
  }

  chess::Piece promoted;
  if (promoted_piece > 0) {
    promoted = static_cast<chess::Piece>(promoted_piece);
  }

  const auto [result, move_made] = _board_manager->tryMove({ source, target, promoted });

  sound_to_play = _result_to_sound.at(result);

  if (result != chess::MoveResult::Illegal) {

    _move_model.addEntry({ move_made,
                           color_moved,
                           result });

    emit moveConfirmed(from, to);
    _current_history_index++;

    // end of game conditions
    switch (result) {
      case chess::MoveResult::Checkmate:
      {
        emit gameOver(color_moved == chess::White ? QString(tr("White Wins!"))
                                                  : QString(tr("Black Wins!")));
        break;
      }

      case chess::MoveResult::Stalemate:
      {
        emit gameOver(QString(tr("Stalemate!")));
        break;
      }

      case chess::MoveResult::Draw:
      {
        emit gameOver(QString(tr("Draw by Repetition!")));
        break;
      }

      default:
        break;
    }
  }

  // always update the board and view number
  _current_move_index = _move_model.dataCount() - 1;
  _board_model.setBoard(std::move(_board_manager->toArray()));

  emit playSound(sound_to_play);
}
