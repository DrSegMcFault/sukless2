#pragma once

#include <algorithm>
#include <iostream>
#include <optional>
#include <cstdint>
#include <tuple>
#include <ranges>
#include <array>

namespace chess {
using Bitboard = uint64_t;
using Board = std::array<Bitboard, 15>;

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

struct Move {
    uint8_t from;
    uint8_t to;
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

// flags for the game state
struct State {
    uint8_t castling_rights = 0b00001111;
    Color side_to_move = Color::white;
    // target enpassant square
    uint8_t en_passant_target = chess::NoSquare;
};

enum class AIDifficulty {
    Easy,
    Medium,
    Hard
};

struct AIConfig {
    AIDifficulty difficulty;
    uint8_t max_depth;
    Color controlling;
};

static const std::string starting_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void print_board(const Bitboard& b);

namespace util {

template<typename T, typename V>
bool contains(const T& a, const V& b) {
    return std::find(a.begin(), a.end(), b) != a.end();
}

template <typename T>
constexpr auto range(T end) {
    return std::ranges::views::iota(static_cast<T>(0), end);
}

template <typename T>
concept EnumClass = std::is_enum_v<T> && !std::is_same_v<T, std::underlying_type_t<T>>;

template<EnumClass T>
inline auto toul(T enum_value) {
    return static_cast<std::underlying_type_t<T>>(enum_value);
}

namespace bits {

// index of least significant bit
uint8_t get_lsb_index(Bitboard b);

#define set_bit(i, b) ((b) |= (1ULL << (i)))
#define clear_bit(i, b) ((b) &= ~(1ULL << (i)))
#define is_set(i, b) ((b) & (1ULL << (i)))
#define move_bit(from, to, b) do { clear_bit(from, b); set_bit(to, b); } while(0)

union HashedMove {
    struct {
        uint32_t source : 6;
        uint32_t target : 6;
        uint32_t piece : 4;
        uint32_t promoted : 4;
        uint32_t capture : 1;
        uint32_t double_push : 1;
        uint32_t enpassant : 1;
        uint32_t castling : 1;
        uint32_t : 8;
    };

    uint32_t move;

    bool operator==(const HashedMove& other) const {
        return move == other.move;
    }

    /*******************************************************************************
        *
        * Method: explode()
        * returns a tuple of the anonymous struct in order of declaration
        *******************************************************************************/
    auto explode() const {
        return
            std::make_tuple(static_cast<uint8_t>(source), static_cast<uint8_t>(target),
                            static_cast<Piece>(piece), static_cast<Piece>(promoted),
                            static_cast<bool>(capture), static_cast<bool>(double_push),
                            static_cast<bool>(enpassant), static_cast<bool>(castling));
    }
};

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

std::optional<uint8_t> algebraic_to_index(const std::string& alg);

Piece piece_from_char(char c);
char char_from_piece(Piece p);

} // namespace util::fen

} // namespace util
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
} // namespace chess
