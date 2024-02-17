#pragma once

#include <array>
#include <optional>
#include <vector>

#include "util.hxx"
#include "MoveGenerator.hxx"

namespace chess {

class BoardManager
{
public:
  BoardManager(const MoveGenerator* g);
  BoardManager(const MoveGenerator* g, const std::string& fen);
  BoardManager(const BoardManager&) noexcept = default;
  BoardManager(BoardManager&&) = delete;
  ~BoardManager() = default;

  // attempts to perfrom the provided move on the board
  std::tuple<MoveResult, util::bits::HashedMove> tryMove(const chess::Move& move);

  // returns the piece at the provided square, if applicable
  std::optional<Piece> squareToPiece(uint8_t square) const;

  // return the squares that the piece can go to, provided a piece is there
  std::vector<uint8_t> getPseudoLegalMoves(uint8_t square) const;

  // return the move if found in the hashed form
  std::optional<util::bits::HashedMove> findMove(uint8_t source,
                                                 uint8_t target,
                                                 uint32_t promoted_to) const;

  // get an array represenation of the current board
  std::array<std::optional<Piece>, 64> getCurrentBoard() const;

  // get the color of the current side to move
  Color getSideToMove() const { return _state.side_to_move; }

  // reset the board back to the starting position
  void reset() {
    _history.clear();
    initFromFen(chess::starting_position);
  }

  // retrieve the bitboard for the specified piece
  const auto& operator[](Piece piece) const {
    return _board[piece];
  }

  // get the current number of the provided piece on the board
  auto pieceCount(Piece piece = Piece::All) const {
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
  const MoveGenerator* _generator;

  // FEN history of the current game being played
  std::vector<std::string> _history;

  // flags for the game state
  chess::State _state;

  // performs the move on the board
  MoveResult makeMove(const util::bits::HashedMove& move);

  // initialize board from FEN string
  void initFromFen(const std::string& fen);

  // generate FEN representation of current board
  std::string generateFen();

  // is the board in check
  bool isCheck(const Board&, const State&);

  // update the white occupancy bitboard
  inline Bitboard calcWhiteOccupancy(Board& board) {
    return (board[WhitePawn] | board[WhiteKnight] | board[WhiteBishop] |
            board[WhiteRook] | board[WhiteQueen] | board[WhiteKing]);
  }

  // update the black occupancy bitboard
  inline Bitboard calcBlackOccupancy(Board& board) {
    return (board[BlackPawn] | board[BlackKnight] | board[BlackBishop] |
            board[BlackRook] | board[BlackQueen] | board[BlackKing]);
  }

  // update the global occupancy bitboard
  inline Bitboard calcGlobalOccupancy(Board& board) {
    return (calcWhiteOccupancy(board) | calcBlackOccupancy(board));
  }

  friend class AI;
};
} // namespace chess
