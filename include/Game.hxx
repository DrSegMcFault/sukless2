#pragma once

#include <string>
#include <vector>
#include <memory>

#include "BoardManager.hxx"
#include "AI.hxx"
#include "MoveGen.hxx"

namespace chess {
  class AI;
class Game {
  public:
    Game();
    Game(std::string position, AIConfig ai_cfg);

    MoveResult try_move(const Move& m);

    bool is_move_pseudo_legal(const Move& m) const;

    Color get_side_to_move() const;

    std::array<std::optional<Piece>, 64> get_current_board() const;
    std::tuple<Board, State> get_board_info() const { return _mgr->get_board_info(); }

    // for gui purposes. returns the squares the piece can go to
    std::vector<uint8_t> get_pseudo_legal_moves(uint8_t square) const;

  private:
    std::shared_ptr<MoveGen> _generator;
    std::shared_ptr<BoardManager> _mgr;
    std::shared_ptr<AI> _ai;
};
}
