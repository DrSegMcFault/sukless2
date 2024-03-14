#pragma once

#include <algorithm>
#include <optional>
#include <cstdint>
#include <tuple>
#include <ranges>
#include <array>

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
  };

  enum class CastlingRights : uint8_t {
    WhiteKingSide = 1,
    WhiteQueenSide = 2,
    WhiteCastlingRights = 3,
    BlackKingSide = 4,
    BlackQueenSide = 8,
    BlackCastlingRights = 12
  };

  static constexpr uint8_t NoSquare = 64;

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

  const static std::string starting_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

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

  static constexpr uint8_t A1 = 0;  static constexpr uint8_t B1 = 1;
  static constexpr uint8_t C1 = 2;  static constexpr uint8_t D1 = 3;
  static constexpr uint8_t E1 = 4;  static constexpr uint8_t F1 = 5;
  static constexpr uint8_t G1 = 6;  static constexpr uint8_t H1 = 7;
  static constexpr uint8_t A2 = 8;  static constexpr uint8_t B2 = 9;
  static constexpr uint8_t C2 = 10; static constexpr uint8_t D2 = 11;
  static constexpr uint8_t E2 = 12; static constexpr uint8_t F2 = 13;
  static constexpr uint8_t G2 = 14; static constexpr uint8_t H2 = 15;
  static constexpr uint8_t A3 = 16; static constexpr uint8_t B3 = 17;
  static constexpr uint8_t C3 = 18; static constexpr uint8_t D3 = 19;
  static constexpr uint8_t E3 = 20; static constexpr uint8_t F3 = 21;
  static constexpr uint8_t G3 = 22; static constexpr uint8_t H3 = 23;
  static constexpr uint8_t A4 = 24; static constexpr uint8_t B4 = 25;
  static constexpr uint8_t C4 = 26; static constexpr uint8_t D4 = 27;
  static constexpr uint8_t E4 = 28; static constexpr uint8_t F4 = 29;
  static constexpr uint8_t G4 = 30; static constexpr uint8_t H4 = 31;
  static constexpr uint8_t A5 = 32; static constexpr uint8_t B5 = 33;
  static constexpr uint8_t C5 = 34; static constexpr uint8_t D5 = 35;
  static constexpr uint8_t E5 = 36; static constexpr uint8_t F5 = 37;
  static constexpr uint8_t G5 = 38; static constexpr uint8_t H5 = 39;
  static constexpr uint8_t A6 = 40; static constexpr uint8_t B6 = 41;
  static constexpr uint8_t C6 = 42; static constexpr uint8_t D6 = 43;
  static constexpr uint8_t E6 = 44; static constexpr uint8_t F6 = 45;
  static constexpr uint8_t G6 = 46; static constexpr uint8_t H6 = 47;
  static constexpr uint8_t A7 = 48; static constexpr uint8_t B7 = 49;
  static constexpr uint8_t C7 = 50; static constexpr uint8_t D7 = 51;
  static constexpr uint8_t E7 = 52; static constexpr uint8_t F7 = 53;
  static constexpr uint8_t G7 = 54; static constexpr uint8_t H7 = 55;
  static constexpr uint8_t A8 = 56; static constexpr uint8_t B8 = 57;
  static constexpr uint8_t C8 = 58; static constexpr uint8_t D8 = 59;
  static constexpr uint8_t E8 = 60; static constexpr uint8_t F8 = 61;
  static constexpr uint8_t G8 = 62; static constexpr uint8_t H8 = 63;

  // useful constants
  static constexpr Bitboard not_a_file = 18374403900871474942ULL;
  static constexpr Bitboard not_h_file = 9187201950435737471ULL;
  static constexpr Bitboard not_hg_file = 4557430888798830399ULL;
  static constexpr Bitboard not_ab_file = 18229723555195321596ULL;

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

namespace chess::util {

  template<typename T, typename V>
  bool contains(const T& a, const V& b) {
    return std::find(a.begin(), a.end(), b) != a.end();
  }

  template <typename T>
  constexpr auto range(T end) {
    return std::views::iota(T{0}, end);
  }

  template <typename T>
  constexpr auto range(T begin, T end) {
    return std::views::iota(begin, end);
  }

  template <typename T>
  concept EnumClass = std::is_enum_v<T>;

  template<EnumClass T>
  constexpr inline auto toul(T enum_value) {
    return static_cast<std::underlying_type_t<T>>(enum_value);
  }
} // namespace util

namespace chess::util::fen {

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

} // namespace chess::util::fen

namespace chess::util::bits {

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

} // namespace chess::util::bits
