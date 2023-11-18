#pragma once

#include <array>
#include <optional>
#include <unordered_map>
#include <vector>

#include "util.hxx"
#include "BoardConstants.hxx"

namespace chess {

class BoardManager
{
  public:
    BoardManager();
    BoardManager(const std::string& fen);
    BoardManager(const BoardManager&) noexcept = default;
    BoardManager(BoardManager&&) = delete;
    ~BoardManager() = default;

    MoveResult make_move(util::bits::HashedMove move);

    std::optional<Piece> square_to_piece(uint8_t square) const;

    std::vector<uint8_t> get_pseudo_legal_moves(uint8_t square) const;

    std::optional<util::bits::HashedMove> find_move(uint8_t source, uint8_t target) const;

    std::array<std::optional<Piece>, 64> get_current_board() const;

    Color side_to_move() const { return _state.side_to_move; }

    // convienence functions
    const auto& operator[](Piece piece) const {
      return _board[piece];
    }

    auto piece_count(Piece piece = Piece::All) const {
       return util::bits::count(_board[piece]);
    }

    void print(Piece p = Piece::All) const {
      chess::print_board(_board[p]);
    }

    const Bitboard& all() const {
      return _board[Piece::All];
    }

  private:
    // collection of all Bitboards
    Board _board;

    // pre-calculated attack Bitboards
    std::array<std::array<Bitboard, 64>, 2> pawn_attacks;
    std::array<Bitboard, 64> knight_attacks;
    std::array<Bitboard, 64> king_attacks;
    std::array<std::array<Bitboard, 512>, 64> bishop_attacks;
    std::array<std::array<Bitboard, 4096>, 64> rook_attacks;

    std::vector<util::bits::HashedMove> _move_list;

    enum class CastlingRights : uint8_t {
      WhiteKingSide = 1,
      WhiteQueenSide = 2,
      WhiteCastlingRights = 3,
      BlackKingSide = 4,
      BlackQueenSide = 8,
      BlackCastlingRights = 12
    };

    // flags for the game state
    struct State {
      uint8_t castling_rights = 0b00001111;
      Color side_to_move = Color::white;
      // target enpassant square
      uint8_t en_passant_target = chess::NoSquare;
    } _state;

    // attack masks per square
    std::array<Bitboard, 64> bishop_masks;
    std::array<Bitboard, 64> rook_masks;

    const std::string starting_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    bool is_square_attacked(uint8_t square,
                            Color side,
                            std::array<Bitboard, 15>& board) const;

    // after move update functions
    Bitboard calc_white_occupancy(Board& board);
    Bitboard calc_black_occupancy(Board& board);
    Bitboard calc_global_occupancy(Board& board) {
      return (calc_white_occupancy(board) | calc_black_occupancy(board));
    }

    // attack retrieval functions
    Bitboard get_bishop_attacks(uint8_t square, Bitboard occ) const;
    Bitboard get_rook_attacks(uint8_t square, Bitboard occ) const;
    Bitboard get_queen_attacks(uint8_t square, Bitboard occ) const
    {
      return (get_bishop_attacks(square, occ) | get_rook_attacks(square, occ));
    }

    // initialization functions
    constexpr void init_attack_tables();
    constexpr void init_pawn_attacks();
    constexpr void init_knight_attacks();
    constexpr void init_king_attacks();
    constexpr void init_bishop_masks();
    constexpr void init_rook_masks();
    void init_slider_attacks();
    constexpr Bitboard calc_bishop_attacks(uint8_t square, Bitboard occ) const;
    constexpr Bitboard calc_rook_attacks(uint8_t square, Bitboard occ) const;
    void init_from_fen(const std::string& fen);

    // move generation

    inline void add_move(uint32_t source, uint32_t target,
                         uint32_t piece, uint32_t promotion,
                         uint32_t capture, uint32_t double_push,
                         uint32_t enpassant, uint32_t castling);

    void generate_moves();
    void generate_white_moves();
    void generate_black_moves();
    void generate_white_pawn_moves();
    void generate_black_pawn_moves();
    void generate_white_castling_moves();
    void generate_black_castling_moves();
    void generate_king_moves(Color side_to_move);
    void generate_knight_moves(Color side_to_move);
    void generate_bishop_moves(Color side_to_move);
    void generate_rook_moves(Color side_to_move);
    void generate_queen_moves(Color side_to_move);

    // tests
    void test_attack_lookup();

    friend class AI;
};
} // namespace chess
