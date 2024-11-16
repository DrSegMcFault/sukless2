#pragma once

#include <tuple>
#include <array>

#include "Constants.hxx"

namespace chess {
  using Bitboard = uint64_t;
  using Board = std::array<Bitboard, 16>;

  enum Color {
    White,
    Black
  };

  enum class MoveResult {
    Illegal,
    Valid,
    Check,
    Checkmate,
    Stalemate,
    Draw
  };

  enum Piece {
    NoPiece,
    WhitePawn, WhiteKnight, WhiteBishop,
    WhiteRook, WhiteQueen,  WhiteKing,
    BlackPawn, BlackKnight, BlackBishop,
    BlackRook, BlackQueen,  BlackKing,
    WhiteAll,  BlackAll,    All
  };

  struct Move {
    uint8_t from;
    uint8_t to;
    Piece promoted_to;
  };

  union HashedMove {
    struct {
      uint32_t source : 6;
      uint32_t target : 6;
      uint32_t piece : 5;
      uint32_t promoted : 5;
      uint32_t capture : 1;
      uint32_t double_push : 1;
      uint32_t enpassant : 1;
      uint32_t castling : 1;
      uint32_t : 6;
    } m;

    uint32_t hashed;

    HashedMove() {}

    HashedMove(uint32_t hashed_) : hashed(hashed_) {}

    // this while not visually appealing, is technically faster 
    // than setting bitfield members individually
    HashedMove(
      uint32_t s, uint32_t t,
      uint32_t p, uint32_t pr,
      uint32_t cap, uint32_t dbl,
      uint32_t ep, uint32_t c)
       : hashed(s | (t << 6) | (p << 12) | (pr << 17) | (cap << 22) | (dbl << 23) | (ep << 24) | (c << 25))
     {}


    bool operator==(const HashedMove& other) const {
      return hashed == other.hashed;
    }

    auto explode() const {
      return
          std::make_tuple(static_cast<uint8_t>(m.source), static_cast<uint8_t>(m.target),
                          static_cast<Piece>(m.piece), static_cast<Piece>(m.promoted),
                          static_cast<bool>(m.capture), static_cast<bool>(m.double_push),
                          static_cast<bool>(m.enpassant), static_cast<bool>(m.castling));
    }

    chess::Move toMove() const {
      return { static_cast<uint8_t>(m.source),
               static_cast<uint8_t>(m.target),
               static_cast<chess::Piece>(m.promoted) };
    }

    ~HashedMove() {}
  };

  enum class CastlingRights : uint8_t {
    WhiteKingSide = 1,
    WhiteQueenSide = 2,
    WhiteCastlingRights = 3,
    BlackKingSide = 4,
    BlackQueenSide = 8,
    BlackCastlingRights = 12
  };

  struct BoardState {
    uint8_t castling_rights = 15; //0b1111;
    uint8_t half_move_clock = 0;
    uint8_t full_move_count = 1;
    uint8_t en_passant_target = chess::NoSquare;
    Color side_to_move = White;
  };

  enum class AIDifficulty {
    Easy,
    Medium,
    Hard
  };

  struct AIConfig {
    AIDifficulty difficulty;
    Color controlling;
    bool assisting_user;
    bool enabled;
  };
}
