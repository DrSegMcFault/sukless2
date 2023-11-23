#pragma once

#include <array>
#include <optional>
#include <vector>
#include <memory>
#include "util.hxx"
#include "MoveGen.hxx"

namespace chess {

class BoardManager
{
public:
    BoardManager(std::shared_ptr<MoveGen> g);
    BoardManager(std::shared_ptr<MoveGen> g, const std::string& fen);
    BoardManager(const BoardManager&) noexcept = default;
    BoardManager(BoardManager&&) = delete;
    ~BoardManager() = default;

    MoveResult make_move(util::bits::HashedMove move);
    std::optional<Piece> square_to_piece(uint8_t square) const;
    std::vector<uint8_t> get_pseudo_legal_moves(uint8_t square) const;
    std::optional<util::bits::HashedMove> find_move(uint8_t source, uint8_t target) const;
    std::array<std::optional<Piece>, 64> get_current_board() const;

    std::tuple<Board,State> get_board_info() const {
        return { _board, _state };
    }

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

    std::vector<util::bits::HashedMove> _move_list;
    std::shared_ptr<MoveGen> _generator;

    // flags for the game state
    chess::State _state;

    void init_from_fen(const std::string& fen);

    // after move update functions
    inline static Bitboard calc_white_occupancy(Board& board) {
        return (board[w_pawn] | board[w_knight] | board[w_bishop] |
                board[w_rook] | board[w_queen] | board[w_king]);
    }

    inline static Bitboard calc_black_occupancy(Board& board) {
        return (board[b_pawn] | board[b_knight] | board[b_bishop] |
                board[b_rook] | board[b_queen] | board[b_king]);
    }

    inline static Bitboard calc_global_occupancy(Board& board) {
        return (calc_white_occupancy(board) | calc_black_occupancy(board));
    }
};
} // namespace chess
