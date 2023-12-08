#pragma once

#include <memory>
#include <unordered_map>
#include <tuple>

#include "util.hxx"
#include "Game.hxx"

namespace chess {
  class Game;

  class AI
  {
    public:
      AI() = delete;

      AI(Game* game, std::shared_ptr<MoveGen> g, AIConfig cfg);

      AI(const AI&) = delete;
      AI(AI&&) = delete;
      ~AI() = default;

      int evaluate(const Board& b, const State& s);

      Color get_controlling_color() { return _controlling_color; }

      AIConfig get_cfg() const { return _cfg; }

      int get_white_eval() const { return _white_eval; };

      int get_black_eval() const { return _black_eval; };

      std::optional<util::bits::HashedMove> get_best_move();

    private:
      Game* _game;
      std::shared_ptr<MoveGen> _generator;
      AIConfig _cfg;
      int _depth;
      int _white_eval;
      int _black_eval;
      int _white_material_score;
      int _black_material_score;
      Color _controlling_color;
  };
}
