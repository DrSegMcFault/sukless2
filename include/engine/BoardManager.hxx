#pragma once

#include <array>
#include <optional>
#include <vector>
#include <string>

#include "ChessUtil.hxx"
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

private:
  BoardManager(const MoveGenerator* g,
               const Board&,
               const BoardState&,
               const std::vector<HashedMove>&);

public:
  // attempts to perfrom the provided move on the board
  [[nodiscard]] std::tuple<MoveResult, HashedMove> tryMove(const chess::Move& move);

  // return the squares that the piece can go to, provided a piece is there
  std::vector<uint8_t> getPseudoLegalMoves(uint8_t square) const;

  // return the move if found in the hashed form
  std::optional<HashedMove> findMove(uint8_t source,
                                     uint8_t target,
                                     uint32_t promoted_to) const;

  // generates Board and BoardState from FEN string
  std::optional<std::pair<Board, BoardState>> makeBoardFromFen(const std::string& fen) const;

  // get an array represenation of the current board
  std::array<std::optional<Piece>, 64> toArray() const {
    return chess::to_array(_board);
  }

  // returns the piece at the provided square, if applicable
  std::optional<Piece> pieceAt(uint8_t square) const {
    return chess::piece_at(_board, square);
  }

  // get the color of the current side to move
  Color getSideToMove() const { return _state.side_to_move; }

  // get the current half move clock
  uint8_t getHalfMoveClock() const { return _state.half_move_clock; }

  // get the current full move count
  uint8_t getFullMoveCount() const { return _state.full_move_count; }

  // return the fen string at the provided index
  std::optional<std::string> historyAt(size_t index) {
    if (index < _history.size()) {
      return _history[index];
    }
    return std::nullopt;
  }

  // reset the board back to the starting position
  void reset() {
    initFromFen(chess::starting_position);
  }

  // set the state of the game to the fen string
  void reset(const std::string& fen) {
    initFromFen(fen);
  }

  // generate the fen representation of the current board
  std::string generateFen() const {
    return fen::generate(_board, _state);
  }

  // get the current number of the provided piece on the board
  auto pieceCount(Piece piece = Piece::All) const {
    return bits::count(_board[piece]);
  }

  // print a string representation of the specified piece's bitboard
  void print(Piece p = Piece::All) const {
    print_board(_board[p]);
  }

private:
  // collection of all Bitboards, layout is the same as the chess::Piece enum
  Board _board;

  // flags for the game state
  BoardState _state;

  // move generator
  const MoveGenerator* _generator;

  // current list of pseudo legal moves
  std::vector<HashedMove> _move_list;

  // FEN history of the current game being played
  std::vector<std::string> _history;

  // flag to not generate fen history
  // useful for AI move making when in move search
  bool NO_HISTORY = false;

  // performs the move on the board
  MoveResult makeMove(const HashedMove& move);

  // initialize board from FEN string
  void initFromFen(const std::string& fen);

  // is the board in check
  bool isCheck(const Board&, const BoardState&) const;

  // update the occupancy bitboards
  inline void updateOccupancies(Board& b) const {
    b[WhiteAll] = (b[WhitePawn] | b[WhiteKnight] | b[WhiteBishop] |
                   b[WhiteRook] | b[WhiteQueen] | b[WhiteKing]);

    b[BlackAll] = (b[BlackPawn] | b[BlackKnight] | b[BlackBishop] |
                   b[BlackRook] | b[BlackQueen] | b[BlackKing]);

    b[All] = b[WhiteAll] | b[BlackAll];
  }

  friend class AI;
};
} // namespace chess
