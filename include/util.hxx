#pragma once
#include <iostream>

namespace chess {
  using Bitboard = uint64_t;
  enum Color {
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

  enum Piece {
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

  // cool thing i didnt know about inline is that
  // it allows the function to be definined in multiple
  // translation units without causing duplicate symbol linker
  // errors
  // set the bit at index i
  inline void set(int i, Bitboard& b) { b |= (1ULL << i); }

  // clear the bit at index i
  inline void clear(int i, Bitboard& b) { b &= ~(1ULL << i); }

  // is the bit at index i set
  inline bool is_set(int i, const Bitboard& b) {
    return (b & (1ULL << i));
  }

  // move the bit at index 'from' to index 'to'
  inline void move(int from, int to, Bitboard& b) {
    clear(from, b);
    set(to, b);
  }

  // count the number of set bits
  inline uint32_t count(Bitboard b) {
    uint32_t count = 0;
    while (b) {
      count++;
      b &= b - 1;
    }
    return count;
  }

  void print_board(const Bitboard& b);
  int get_lsb_index(Bitboard b);
}
