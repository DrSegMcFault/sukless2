#pragma once

#include <array>
#include <optional>
#include <vector>

#include "util.hxx"
#include "MoveGen.hxx"

namespace chess {

class BoardManager
{
  public:
    BoardManager(const MoveGen* g);
    BoardManager(const MoveGen* g, const std::string& fen);
    BoardManager(const BoardManager&) noexcept = default;
    BoardManager(BoardManager&&) = delete;
    ~BoardManager() = default;

    // attempts to perfrom the provided move on the board
    std::tuple<MoveResult, util::bits::HashedMove> try_move(const chess::Move& move);

    // returns the piece at the provided square, if applicable
    std::optional<Piece> square_to_piece(uint8_t square) const;

    // return the squares that the piece can go to, provided a piece is there
    std::vector<uint8_t> get_pseudo_legal_moves(uint8_t square) const;

    // return the move if found in the hashed form
    std::optional<util::bits::HashedMove> find_move(uint8_t source,
                                                    uint8_t target,
                                                    uint32_t promoted_to) const;

    // get an array represenation of the current board
    std::array<std::optional<Piece>, 64> get_current_board() const;

    // get the color of the current side to move
    Color get_side_to_move() const { return _state.side_to_move; }

    // retrieve the bitboard for the specified piece
    const auto& operator[](Piece piece) const {
      return _board[piece];
    }

    // get the current number of the provided piece on the board
    auto piece_count(Piece piece = Piece::All) const {
      return util::bits::count(_board[piece]);
    }

    // print a string representation of the specified piece's bitboard
    void print(Piece p = Piece::All) const {
      print_board(_board[p]);
    }

  private:
    // collection of all Bitboards, layout is the same as the chess::Piece enum
    Board _board;

    // current list of pseudo legal moves
    std::vector<util::bits::HashedMove> _move_list;

    // move generator
    const MoveGen* _generator;

    // FEN history of the current game being played
    std::vector<std::string> _history;

    // flags for the game state
    chess::State _state;

    // performs the move on the board
    MoveResult make_move(const util::bits::HashedMove& move);

    // initialize board from FEN string
    void init_from_fen(const std::string& fen);

    // generate FEN representation of current board
    std::string generate_fen();

    // is the board in check
    bool is_check(const Board&, const State&);

    // update the white occupancy bitboard
    inline Bitboard calc_white_occupancy(Board& board) {
      return (board[w_pawn] | board[w_knight] | board[w_bishop] |
              board[w_rook] | board[w_queen] | board[w_king]);
    }

    // update the black occupancy bitboard
    inline Bitboard calc_black_occupancy(Board& board) {
      return (board[b_pawn] | board[b_knight] | board[b_bishop] |
              board[b_rook] | board[b_queen] | board[b_king]);
    }

    // update the global occupancy bitboard
    inline Bitboard calc_global_occupancy(Board& board) {
      return (calc_white_occupancy(board) | calc_black_occupancy(board));
    }

    friend class AI;
};
} // namespace chess
