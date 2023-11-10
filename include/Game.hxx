#pragma once
#include "BoardManager.hxx"
#include <string>
#include <vector>

namespace chess {

class Game {
  private:
    BoardManager _mgr {};

  public:

    Game();

    MoveResult try_move(const Move& m);

    bool is_move_pseudo_legal(const Move& m) const;

    std::array<std::optional<Piece>, 64> get_current_board() const;

    // for gui purposes. returns the squares the piece can go to
    std::vector<int> get_pseudo_legal_moves(int square) const;
};
}
