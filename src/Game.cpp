#include "Game.hxx"
#include <iostream>

using namespace chess;

/*******************************************************************************
 *
 * Method: Game()
 *
 *******************************************************************************/
Game::Game()
{
}

/*******************************************************************************
 *
 * Method: try_move(int square)
 *
 *******************************************************************************/
MoveResult Game::try_move(const Move& m)
{
  if (!is_move_pseudo_legal(m)) {
    return MoveResult::Illegal;
  }

  _mgr.make_move(encode_move(m.from, m.to, _mgr.square_to_piece(m.from).value(), 0, 0, 0, 0, 0));

  return MoveResult::Success;
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
 * Method: get_pseudo_legal_moves(int square)
 * returns the squares the piece can go to 
 *******************************************************************************/
std::vector<int> Game::get_pseudo_legal_moves(int square) const
{
  return _mgr.get_pseudo_legal_moves(square);
}

/*******************************************************************************
 *
 * Method: get_current_board()
 * 
 *******************************************************************************/
std::array<std::optional<Piece>, 64> Game::get_current_board() const
{
  return _mgr.get_current_board();
}
