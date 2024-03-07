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

  Color color() { return cfg.controlling; }

  Color opponent() { return cfg.controlling == White ? Black : White; }

  bool assisting() { return cfg.assisting_user; }

  bool enabled() { return cfg.enabled; }

  int getWhiteEval() const { return _white_eval; };

  int getBlackEval() const { return _black_eval; };

  std::optional<HashedMove> getBestMove(const BoardManager&);

  AIConfig cfg;

private:
  const MoveGenerator* _generator;

  int _depth;
  int _white_eval;
  int _black_eval;
  int _white_material_score;
  int _black_material_score;

  // calc material diff score
  std::pair<int,int> calcMaterialScore(const BoardManager& b) const;

  int calcPositionalScore(const BoardManager& b, Color c) const;

  int evaluate(const BoardManager& b) const;

  int alphaBeta(BoardManager& mgr, int alpha, int beta, int cur_depth, bool is_max);

  // get the legal moves from a board
  std::vector<HashedMove> getLegalMoves(const BoardManager&);

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

  static constexpr std::array<int, 64> pawn_values = {
     90, 100, 100, 100, 100, 100, 100,  90,
     40,  40,  50,  60,  60,  50,  40,  40,
     20,  20,  50,  55,  55,  50,  20,  20,
     10,  10,  40,  50,  50,  40,  10,  10,
      0,   0,  20,  40,  40,  20,   0,   0,
    -10, -10,  10,  30,  30,  10, -10, -10,
      1,   1,   1,   1,   1,   1,   1,   1,
      0,   0,   0,   0,   0,   0,   0,   0
  };

  static constexpr std::array<int, 64> knight_values = {
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   5,   5,   5,   5,   5,   5,  0,
   0,   5,  20,   25, 25,  20,   5,  0,
   0,   5,  10,   30, 30,  10,   5,  0,
   0,   5,  10,   30, 30,  10,   5,  0,
   0,   5,  20,   25, 25,  20,   5,  0,
   0,   5,   5,   5,   5,   5,   5,  0,
   0,   0,   0,   0,   0,   0,   0,  0
  };

  static constexpr std::array<int, 64> king_values = {
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0
  };

  static constexpr std::array<int, 64> bishop_values = {
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0
  };

  static constexpr std::array<int, 64> rook_values = {
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0
  };

  static constexpr std::array<int, 64> queen_values = {
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   0,   0,  0
  };

  std::unordered_map<Piece, std::array<int, 64>> positional_map {
    {Piece::WhitePawn, pawn_values},
    {Piece::WhiteKnight, knight_values},
    {Piece::WhiteBishop, bishop_values},
    {Piece::WhiteRook, rook_values},
    {Piece::WhiteQueen, queen_values},
    {Piece::WhiteKing, king_values},
    {Piece::BlackPawn, pawn_values},
    {Piece::BlackKnight, knight_values},
    {Piece::BlackBishop, bishop_values},
    {Piece::BlackRook, rook_values},
    {Piece::BlackQueen, queen_values},
    {Piece::BlackKing, king_values}
  };
};
}
