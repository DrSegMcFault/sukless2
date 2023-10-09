#pragma once

namespace chess {
  enum class Color {
    white,
    black
  };

  enum class MoveResult {
    Success,
    Invalid,
    Check,
    Checkmate,
    Stalemate,
    Draw
  };

  enum class Piece {
    w_pawn, w_knight, w_bishop,
    w_rook, w_queen,  w_king,
    b_pawn, b_knight, b_bishop,
    b_rook, b_queen,  b_king,
    All
  };

  enum Pos {
    a1,b1,c1,d1,e1,f1,g1,h1,
    a2,b2,c2,d2,e2,f2,g2,h2,
    a3,b3,c3,d3,e3,f3,g3,h3,
    a4,b4,c4,d4,e4,f4,g4,h4,
    a5,b5,c5,d5,e5,f5,g5,h5,
    a6,b6,c6,d6,e6,f6,g6,h6,
    a7,b7,c7,d7,e7,f7,g7,h7,
    a8,b8,c8,d8,e8,f8,g8,h8
  };

  struct Move {
    int from;
    int to;
    Piece piece;
    Color color;
  };
}
