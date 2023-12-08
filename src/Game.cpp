#include "Game.hxx"
#include <iostream>
#include <QtAssert>

using namespace chess;

/*******************************************************************************
 *
 * Method: Game::Game()
 *
 *******************************************************************************/
Game::Game() {
  _generator = std::make_shared<MoveGen>();
  _mgr = std::make_shared<BoardManager>(_generator);
  _ai = std::make_shared<AI>(this, _generator,
           AIConfig { AIDifficulty::Medium, 10, Color::black } );
}

/*******************************************************************************
 *
 * Method: Game::Game(AIConfig cfg)
 *
 *******************************************************************************/
Game::Game(AIConfig cfg) {
  _generator = std::make_shared<MoveGen>();
  _mgr = std::make_shared<BoardManager>(_generator);
  _ai = std::make_shared<AI>(this, _generator, cfg);
  // handle case where ai must go first
  if (_mgr->side_to_move() == _ai->get_controlling_color()) {
     _mgr->try_move(_ai->get_best_move().value());
  }
}

/*******************************************************************************
 *
 * Method: try_move(const Move& move)
 *
 *******************************************************************************/
MoveResult Game::try_move(const Move& m)
{
  MoveResult res;
  if (auto move = _mgr->find_move(m.from, m.to)) {
    if (auto r = _mgr->try_move(move.value());
       r != MoveResult::Illegal)
    {
      // do the AI move
      auto ai_m = _ai->get_best_move();
      if (ai_m) {
        auto r = _mgr->try_move(ai_m.value());
          Q_ASSERT(r != MoveResult::Illegal);
      }

      return r;
    }
  }

  return MoveResult::Illegal;
}

/*******************************************************************************
 *
 * Method: is_move_pseudo_legal(const Move& m)
 *
 *******************************************************************************/
bool Game::is_move_pseudo_legal(const Move& m) const
{
  return util::contains(get_pseudo_legal_moves(m.from), m.to);
}

/*******************************************************************************
 *
 * Method: get_pseudo_legal_moves(uint8_t square)
 * returns the squares the piece can go to 
 *******************************************************************************/
std::vector<uint8_t> Game::get_pseudo_legal_moves(uint8_t square) const
{
  return _mgr->get_pseudo_legal_moves(square);
}

/*******************************************************************************
 *
 * Method: get_current_board()
 * 
 *******************************************************************************/
std::array<std::optional<Piece>, 64> Game::get_current_board() const
{
  return _mgr->get_current_board();
}

/*******************************************************************************
 *
 * Method: get_side_to_move()
 *
 *******************************************************************************/
Color Game::get_side_to_move() const
{
  return _mgr->side_to_move();
}

AIConfig Game::get_ai_cfg() const
{
  return _ai ? _ai->get_cfg() : AIConfig{};
}
