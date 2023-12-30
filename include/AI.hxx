#pragma once

#include <unordered_map>

#include "MoveGen.hxx"
#include "BoardManager.hxx"
#include "util.hxx"

namespace chess {

  class AI
  {
    public:
      AI() = delete;

      AI(const MoveGen* g, AIConfig cfg);

      AI(const AI&) = delete;
      AI(AI&&) = delete;
      ~AI() = default;

      Color get_controlling_color() { return _controlling_color; }

      AIConfig get_cfg() const { return _cfg; }

      int get_white_eval() const { return _white_eval; };

      int get_black_eval() const { return _black_eval; };

      std::optional<util::bits::HashedMove> get_best_move(const BoardManager&);

    private:
      const MoveGen* _generator;
      AIConfig _cfg;

      int _depth;
      int _white_eval;
      int _black_eval;
      int _white_material_score;
      int _black_material_score;

      Color _controlling_color;

      // calc material diff score
      int calc_material_score(const BoardManager& b, Color c) const;

      const std::unordered_map<int, int> piece_values = {
        { util::toul(Piece::w_pawn),    100    },
        { util::toul(Piece::w_knight),  300    },
        { util::toul(Piece::w_bishop),  325    },
        { util::toul(Piece::w_rook),    500    },
        { util::toul(Piece::w_queen),   900    },
        { util::toul(Piece::w_king),    10'000 },

        { util::toul(Piece::b_pawn),    100    },
        { util::toul(Piece::b_knight),  300    },
        { util::toul(Piece::b_bishop),  325    },
        { util::toul(Piece::b_rook),    500    },
        { util::toul(Piece::b_queen),   900    },
        { util::toul(Piece::b_king),    10'000 }
      };
  };
}
