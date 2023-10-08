#pragma once
#include "BoardManager.hxx"
#include <string>

namespace chess {

class Game {
  private:
    BoardManager _mgr{};

  public:

    Game(); 

    const MoveResult try_move(const Move& m);

    const bool is_move_pseudo_legal(const Move& m);

    int alg_to_index(const std::string &alg) const;
};
}