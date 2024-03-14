#pragma once

#include <optional>
#include <cstdint>
#include <array>
#include <string>

#include "ChessTypes.hxx"

namespace chess {

  // prints the board to std::cout
  void print_board(const Bitboard& b);

  // returns the piece at the provided square from Board b
  std::optional<Piece> piece_at(const Board &b, uint8_t sqaure);

  // converts the array of Bitboards (Board) to an array of pieces
  std::array<std::optional<Piece>, 64> to_array(const Board& b);

  // convert a HashedMove into an algebraic move string
  // *Hashed moves are not aware of game states
  // like Checkmate, Stalemate, or Draws
  std::string to_string(const HashedMove& m);

  // convert the Bitboard to a string
  std::string to_string(const Bitboard& m);

  // convert the Color to a string
  std::string to_string(Color c);

  // convert the MoveResult to a string
  std::string to_string(MoveResult r);

  static constexpr std::array<Piece, 6> WhitePieces = {
    Piece::WhitePawn,
    Piece::WhiteKnight,
    Piece::WhiteBishop,
    Piece::WhiteRook,
    Piece::WhiteQueen,
    Piece::WhiteKing
  };

  static constexpr std::array<Piece, 6> BlackPieces = {
    Piece::BlackPawn,
    Piece::BlackKnight,
    Piece::BlackBishop,
    Piece::BlackRook,
    Piece::BlackQueen,
    Piece::BlackKing
  };

  static constexpr std::array<Piece, 12> AllPieces = {
    Piece::WhitePawn,
    Piece::WhiteKnight,
    Piece::WhiteBishop,
    Piece::WhiteRook,
    Piece::WhiteQueen,
    Piece::WhiteKing,
    Piece::BlackPawn,
    Piece::BlackKnight,
    Piece::BlackBishop,
    Piece::BlackRook,
    Piece::BlackQueen,
    Piece::BlackKing
  };
} // namespace chess

namespace chess::fen {

  // generates the FEN representation of the provided board and state
  std::string generate(const Board& b, const BoardState& s);

  // convert an algebraic notation of a square to an index
  std::optional<uint8_t> algebraic_to_index(const std::string& alg);

  // convert an index into the corresponding algebraic notation
  std::optional<std::string> index_to_algebraic(uint8_t index);

  // return the Piece associated with the provided char
  std::optional<Piece> char_to_piece(char c);

  // return the char associated with the provided piece
  char piece_to_char(Piece p);

} // namespace chess::fen

namespace chess::bits {

  #define set_bit(i, b) ((b) |= (1ULL << (i)))

  #define clear_bit(i, b) ((b) &= ~(1ULL << (i)))

  #define is_set(i, b) ((b) & (1ULL << (i)))

  #define move_bit(from, to, b) do { clear_bit(from, b); set_bit(to, b); } while(0)

  // count the number of set bits
  constexpr int count(Bitboard b) {
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

  // index of least significant bit, must be called under precondition
  // that b has at least 1 set bit
  constexpr uint8_t get_lsb_index(Bitboard b) {
    return count((b & -b) -1);
  }

} // namespace chess::bits
