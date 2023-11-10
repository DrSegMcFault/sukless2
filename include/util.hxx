#pragma once
#include <iostream>
#include <optional>

namespace chess {
  using Bitboard = uint64_t;

  enum Color {
    white,
    black
  };

  enum class MoveResult {
    Success,
    Illegal,
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
    w_all,  b_all,    All
  };

  enum Pos {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
  };

  struct Move {
    int from;
    int to;
  };

  void print_board(const Bitboard& b);

  namespace util {

    template<typename T, typename V>
    bool contains(const T& a, const V& b) {
      return std::find(a.begin(), a.end(), b) != a.end();
    }

    namespace bits {

      // index of least significant bit
      int get_lsb_index(Bitboard b);

      #define set_bit(i, b) (b) |= (1ULL << (i))
      #define clear_bit(i, b) (b) &= ~(1ULL << (i))
      #define is_set(i, b) (b) & (1ULL << (i))
      #define move_bit(from, to, b) do { clear_bit(from, b); set_bit(to, b); } while(0)

      // count the number of set bits
      inline int count(Bitboard b) {
        #if defined (__GNUC__) || defined (__clang__)
          return __builtin_popcountll(b);
        #else
          int count = 0;
          while (b) {
            count++;
            b &= b - 1;
          }
          return count;
        #endif
      }

    } // namespace util::bits

    namespace fen {

      std::optional<int> algebraic_to_index(const std::string& alg);

      Piece piece_from_char(char c);

    } // namespace util::fen

  } // namespace util

} // namespace chess
