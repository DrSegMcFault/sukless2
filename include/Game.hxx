#pragma once
#include <string>
#include <vector>

#include "BoardManager.hxx"

namespace chess {

class Game {
  private:
    BoardManager _mgr {};

  public:

    Game();

    MoveResult try_move(const Move& m);

    bool is_move_pseudo_legal(const Move& m) const;

    Color get_side_to_move() const;

    std::array<std::optional<Piece>, 64> get_current_board() const;

    // for gui purposes. returns the squares the piece can go to
    std::vector<uint8_t> get_pseudo_legal_moves(uint8_t square) const;
};
}
