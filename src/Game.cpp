#include "Game.h"

/******************************************************************************
 *
 * Method: Game()
 *
 *****************************************************************************/
Game::Game(QObject *parent)
  : QObject(parent)
  , _move_model(this)
  , _board_model(this)
  , _board_manager(std::make_shared<chess::BoardManager>(&_generator))
{
  connect(this, &Game::moveConfirmed, &_move_model, &MoveModel::onMoveConfirmed);
  reset();
}

/******************************************************************************
 *
 * Method: init()
 * Q_INVOKABLE
 *****************************************************************************/
void Game::init(chess::Color user,
                bool engine_help,
                bool ai_enable,
                chess::AIDifficulty aidiff)
{
  reset();

  chess::Color controlling =
      user == chess::White ? chess::Black : chess::White;

  emit gameStart({ aidiff, controlling, engine_help, ai_enable },
                   chess::starting_position);
}

/******************************************************************************
 *
 * Method: reset()
 * Q_INVOKABLE
 *****************************************************************************/
void Game::reset()
{
  _board_manager->reset();
  _board_model.setBoard(_board_manager->toArray());
  _board_model.updateEvals(0, 0);
  _move_model.clear();
  _current_move_index = 1;
  _current_history_index = 0;

  emit gameStart({ chess::AIDifficulty::Easy, chess::Black, false, false },
                   chess::starting_position);
}

/******************************************************************************
 *
 * Method: handleMove()
 * Q_INVOKABLE
 *****************************************************************************/
void Game::handleMove(int from, int to, int promoted_piece)
{
  auto source = static_cast<uint8_t>(_board_model.toInternalIndex(from));
  auto target = static_cast<uint8_t>(_board_model.toInternalIndex(to));

  QUrl sound_to_play;

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

  chess::Piece promoted = chess::Piece::NoPiece;
  if (promoted_piece > 0) {
    promoted = static_cast<chess::Piece>(promoted_piece);
  }

  const auto [result, move_made] = _board_manager->tryMove({ source, target, promoted });

  sound_to_play = _result_to_sound.at(result);

  if (result != chess::MoveResult::Illegal) {
    emit moveConfirmed( move_made, color_moved, result);
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
  else if (source == target) {
    sound_to_play = "";
  }


  emit playSound(sound_to_play);
  afterMove(result, move_made);
}

/******************************************************************************
 *
 * Method: onMoveReady(chess::HashedMove, chess::Color)
 *
 *****************************************************************************/
void Game::onMoveReady(chess::HashedMove m, chess::Color color_moved)
{
  qDebug() << "AI move received\n";

  const auto [result, move_made] = _board_manager->tryMove(m.toMove());

  if (result != chess::MoveResult::Illegal) {
    emit moveConfirmed( move_made, color_moved, result);
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

  emit playSound(_result_to_sound.at(result));
  afterMove(result, move_made);
}

/******************************************************************************
 *
 * Method: afterMove()
 *
 *****************************************************************************/
void Game::afterMove(chess::MoveResult result, chess::HashedMove move_made)
{
  // always update the board and view number
  if (_current_move_index != _move_model.dataCount() - 1) {
     _current_move_index = _move_model.dataCount() - 1;
    _move_model.setSelected(_current_move_index);
  }
  _board_model.setBoard(_board_manager->toArray());

  if (move_made.capture) {

    float white_mat = 0.0f;
    float black_mat = 0.0f;

    for (auto p : chess::WhitePieces) {
      white_mat +=
            piece_values.at(p) * _board_manager->pieceCount(p);
    }

    for (auto p : chess::BlackPieces) {
      black_mat +=
            piece_values.at(p) * _board_manager->pieceCount(p);
    }

    _board_model.updateEvals(white_mat - black_mat, black_mat - white_mat);
  }
}

/******************************************************************************
 *
 * Method: onSuggestionReady(chess::HashedMove)
 *
 *****************************************************************************/
void Game::onSuggestionReady(chess::HashedMove m)
{
  qDebug() << "Game: onSuggestionReady " << chess::to_string(m) << "\n";
}

/******************************************************************************
 *
 * Method: showPrevious()
 * Q_INVOKABLE
 *****************************************************************************/
void Game::showPrevious() {

  if (_current_history_index >= 1) {
    --_current_history_index;

    if (auto fen = _board_manager->historyAt(_current_history_index)) {

      if (_current_move_index >= 1 && _current_history_index % 2 == 0) {
        _move_model.setSelected(--_current_move_index);
      }

      auto opt = _board_manager->makeBoardFromFen(*fen);
      auto&& [board, state] = *opt;

      _board_model.setBoard(chess::to_array(board));
    }
  }
}

/******************************************************************************
 *
 * Method: showNext()
 * Q_INVOKABLE
 *****************************************************************************/
void Game::showNext() {

  if (auto fen = _board_manager->historyAt(_current_history_index + 1)) {

    _current_history_index++;

    if (_current_move_index + 1 <= _move_model.dataCount() -1) {
      if (_current_history_index > 1 && _current_history_index % 2 != 0) {
        _move_model.setSelected(++_current_move_index);
      }
    }

    auto opt = _board_manager->makeBoardFromFen(*fen);
    auto&& [board, state] = *opt;

    _board_model.setBoard(chess::to_array(board));
  }
}
