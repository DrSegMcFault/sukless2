#pragma once

#include <memory>
#include "Game.hxx"

namespace chess {
  class AI
  {
    public:
      AI() = delete;

      AI(std::shared_ptr<Game> game, Color color, int depth);

      AI(const AI&) = delete;
      AI(AI&&) = delete;
      ~AI() = default;
      int get_white_eval() const { return _white_eval; };
      int get_black_eval() const { return _black_eval; };
      Move get_best_move() const;

    private:
      std::shared_ptr<Game> _game;
      int _depth;
      int _white_eval;
      int _black_eval;
      int _white_material_score;
      int _black_material_score;
      Color _controlling_color;
  };
}
