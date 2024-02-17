#pragma once

#include <unordered_map>

#include "MoveGenerator.hxx"
#include "BoardManager.hxx"
#include "util.hxx"

namespace chess {

class AI
{
public:
  AI() = delete;

  AI(const MoveGenerator* g, AIConfig cfg);

  AI(const AI&) = delete;
  AI(AI&&) = delete;
  ~AI() = default;

  Color getControllingColor() { return _controlling_color; }

  AIConfig getCfg() const { return _cfg; }

  int getWhiteEval() const { return _white_eval; };

  int getBlackEval() const { return _black_eval; };

  std::optional<util::bits::HashedMove> getBestMove(const BoardManager&);

private:
  const MoveGenerator* _generator;
  AIConfig _cfg;

  int _depth;
  int _white_eval;
  int _black_eval;
  int _white_material_score;
  int _black_material_score;

  Color _controlling_color;

  // calc material diff score
  int calcMaterialScore(const BoardManager& b, Color c) const;

  const std::unordered_map<int, int> piece_values = {
    { util::toul(Piece::WhitePawn),    100    },
    { util::toul(Piece::WhiteKnight),  300    },
    { util::toul(Piece::WhiteBishop),  325    },
    { util::toul(Piece::WhiteRook),    500    },
    { util::toul(Piece::WhiteQueen),   900    },
    { util::toul(Piece::WhiteKing),    10'000 },

    { util::toul(Piece::BlackPawn),    100    },
    { util::toul(Piece::BlackKnight),  300    },
    { util::toul(Piece::BlackBishop),  325    },
    { util::toul(Piece::BlackRook),    500    },
    { util::toul(Piece::BlackQueen),   900    },
    { util::toul(Piece::BlackKing),    10'000 }
  };
};
}
