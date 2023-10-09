#include "Game.hxx"
#include <iostream>

using namespace chess;

Game::Game()
{
}

/*******************************************************
* Attempts to make the given move. Returns a MoveResult
*
********************************************************/
const MoveResult Game::try_move(const Move& m)
{
  // check if it is within the pieces psuedo-legal moves

  return MoveResult::Success;
}

/*******************************************************
* returns whether or not the given move is pseudo-legal.
*
********************************************************/
const bool Game::is_move_pseudo_legal(const Move& m)
{
  return _mgr.is_attack_valid(m);
}

/*******************************************************
* Converts algebraic notation ex. ("a1") to the
* associated index. In this impl, a1 = 0, h8=63
********************************************************/
int Game::alg_to_index(const std::string& alg) const 
{
  if (alg.length() != 2) {
    std::cerr << "Invalid algebraic notation: " << 
      alg << std::endl;
    return -1;
  }

  const char& file = alg[0];
  const char& rank = alg[1];

  if (file < 'a' || file > 'h' || rank < '1' || rank > '8') {
      std::cerr << "Invalid algebraic notation: "
        << alg << std::endl;
      return -1;
  }

  int index = (rank - '1') * 8 + (file - 'a');
  return index;
}
