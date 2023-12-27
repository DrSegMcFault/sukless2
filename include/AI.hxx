#pragma once

#include <memory>

#include "MoveGen.hxx"
#include "BoardManager.hxx"
#include "util.hxx"

namespace chess {

  class AI
  {
    public:
      AI() = delete;

      AI(std::shared_ptr<MoveGen> g, AIConfig cfg);

      AI(const AI&) = delete;
      AI(AI&&) = delete;
      ~AI() = default;

      int evaluate(const BoardManager& b);

      Color get_controlling_color() { return _controlling_color; }

      AIConfig get_cfg() const { return _cfg; }

      int get_white_eval() const { return _white_eval; };

      int get_black_eval() const { return _black_eval; };

      std::optional<util::bits::HashedMove> get_best_move(const BoardManager&);

    private:
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
