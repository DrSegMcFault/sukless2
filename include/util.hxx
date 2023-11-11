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
      std::optional<int> get_lsb_index(Bitboard b);

      #define set_bit(i, b) ((b) |= (1ULL << (i)))
      #define clear_bit(i, b) ((b) &= ~(1ULL << (i)))
      #define is_set(i, b) ((b) & (1ULL << (i)))
      #define move_bit(from, to, b) do { clear_bit(from, b); set_bit(to, b); } while(0)

       /*
         Credit:
         https://github.com/maksimKorzh/chess_programming/blob/master/src/bbc/Zobrist_generate/bbc.c

                                                               hexidecimal constants

          0000 0000 0000 0000 0011 1111    source              0x3f
          0000 0000 0000 1111 1100 0000    target              0xfc0
          0000 0000 1111 0000 0000 0000    piece               0xf000
          0000 1111 0000 0000 0000 0000    promoted piece      0xf0000
          0001 0000 0000 0000 0000 0000    capture flag        0x100000
          0010 0000 0000 0000 0000 0000    double push flag    0x200000
          0100 0000 0000 0000 0000 0000    enpassant flag      0x400000
          1000 0000 0000 0000 0000 0000    castling flag       0x800000
     */

      #define encode_move(source, target, piece, promoted, capture, double, enpassant, castling) \
        (source) |          \
        (target << 6) |     \
        (piece << 12) |     \
        (promoted << 16) |  \
        (capture << 20) |   \
        (double << 21) |    \
        (enpassant << 22) | \
        (castling << 23)

      // extract source square
      #define get_move_source(move) (move & 0x3f)

      // extract target square
      #define get_move_target(move) ((move & 0xfc0) >> 6)

      // extract piece
      #define get_move_piece(move) ((move & 0xf000) >> 12)

      // extract promoted piece
      #define get_move_promoted(move) ((move & 0xf0000) >> 16)

      // extract capture flag
      #define get_move_capture(move) (move & 0x100000)

      // extract double pawn push flag
      #define get_move_double(move) (move & 0x200000)

      // extract enpassant flag
      #define get_move_enpassant(move) (move & 0x400000)

      // extract castling flag
      #define get_move_castling(move) (move & 0x800000)

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
      char char_from_piece(Piece p);

    } // namespace util::fen

  } // namespace util

} // namespace chess
