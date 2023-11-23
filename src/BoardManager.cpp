#include "BoardManager.hxx"
#include "MoveGen.hxx"
#include <cassert>

using namespace chess;

/*******************************************************************************
 *
 * Method: BoardManager()
 *
 *******************************************************************************/
BoardManager::BoardManager(std::shared_ptr<MoveGen> g)
  : _generator(g)
{
  _move_list.reserve(256);
  init_from_fen(starting_position);
}

/*******************************************************************************
 *
 * Method: BoardManager(const std::string& fen)
 *
 *******************************************************************************/
BoardManager::BoardManager(std::shared_ptr<MoveGen> g, const std::string& fen)
  : _generator(g)
{
  _move_list.reserve(256);
  init_from_fen(fen);
}

/*******************************************************************************
 *
 * Method: init_from_fen(Bitboard b)
 * TODO: this is incredibly naive
 *******************************************************************************/
void BoardManager::init_from_fen(const std::string &fen)
{
  // populate board occupancies and game state
  // using a FEN string
  for (auto& b : _board) {
    b = 0ULL;
  }

  // parse the FEN string
  uint32_t rank = 7; // start from the 8th rank
  uint32_t file = 0;
  
  for (auto c : fen) {
    if (isdigit(c)) {
      file += (c - '0');
    } else if (c == '/') {
      rank--;
      file = 0;
    } else if (c == ' ') {
      break;
    } else {
      auto piece = util::fen::piece_from_char(c);
      uint8_t square = rank * 8 + file;
      set_bit(square, _board[piece]);
      file++;
    }
  }

  _board[w_all] = calc_white_occupancy(_board);
  _board[b_all] = calc_black_occupancy(_board);
  _board[All] = calc_global_occupancy(_board);

  _state.side_to_move = (fen.find("w") != std::string::npos) ? Color::white : Color::black;

  // TODO: castling rights, en_passant
  _state.en_passant_target = chess::NoSquare;
  _state.castling_rights = util::toul(CastlingRights::WhiteCastlingRights) |
                           util::toul(CastlingRights::BlackCastlingRights);

  _generator->generate_moves(_board, _state, _move_list);
}

/*******************************************************************************
 *
 * Method: square_to_piece(uint8_t square)
 *
 *******************************************************************************/
std::optional<Piece> BoardManager::square_to_piece(uint8_t square) const
{
  std::optional<Piece> ret;
  for (uint8_t p = static_cast<uint8_t>(w_pawn); p <= static_cast<uint8_t>(b_king); p++) {
    if (is_set(square, _board[p])) {
      ret.emplace(static_cast<Piece>(p));
      return ret;
    }
  }

  return ret;
}

/*******************************************************************************
 *
 * Method: get_pseudo_legal_moves(uint8_t square)
 *
 *******************************************************************************/
std::vector<uint8_t> BoardManager::get_pseudo_legal_moves(uint8_t square) const
{
  std::vector<uint8_t> ret;
  ret.reserve(55);
  for (const auto& move : _move_list) {
    if (move.source == square) {
      ret.push_back(move.target);
    }
  }

  return ret;
}

/*******************************************************************************
 *
 * Method: get_current_board()
 *
 *******************************************************************************/
std::array<std::optional<Piece>, 64> BoardManager::get_current_board() const
{
  std::array<std::optional<Piece>, 64> board;
  for (auto i : util::range(NoSquare)) {
    board[i] = square_to_piece(i);
  }
  return board;
}

/*******************************************************************************
 *
 * Method: make_move(uint32_t move)
 *
 *******************************************************************************/
MoveResult BoardManager::make_move(util::bits::HashedMove move) {
  using namespace util;

  MoveResult result = MoveResult::Illegal;

  // copy the board and state in case of illegal move
  std::array<Bitboard, 15> board_copy = _board;
  State state_copy = _state;

  // parse the move
  const auto&& [ source_square, target_square,
                 piece, promoted_to,
                 capture, double_push,
                 was_en_passant, castling ] = move.explode();

  // do the move, update the bitboard of the piece that moved
  move_bit(source_square, target_square, board_copy[piece]);

  // if the move was a capture move, remove the captured piece
  if (capture) {
    Piece start_piece;
    Piece end_piece;

    switch(state_copy.side_to_move) {
      case Color::white:
      {
        start_piece = Piece::b_pawn;
        end_piece = Piece::b_king;
        break;
      }
      case Color::black:
      {
        start_piece = Piece::w_pawn;
        end_piece = Piece::w_king;
        break;
      }
    }

    // loop over possible capture pieces
    for (uint8_t p = static_cast<uint8_t>(start_piece);
        p <= static_cast<uint8_t>(end_piece); p++)
    {
      if (is_set(target_square, board_copy[p])) {
        clear_bit(target_square, board_copy[p]);
        break;
      }
    }
  }

  if (was_en_passant) {
    switch (state_copy.side_to_move) {
      // if white made an en passant capture
      case Color::white:
      {
        clear_bit(target_square - 8, board_copy[b_pawn]);
        break;
      }
      // if black made an en passant capture
      case Color::black:
      {
        clear_bit(target_square + 8, board_copy[w_pawn]);
        break;
      }
    }
  } else {
    state_copy.en_passant_target = chess::NoSquare;
  }

  if (double_push) {
    switch (state_copy.side_to_move) {
      case Color::white:
      {
        state_copy.en_passant_target = target_square - 8;
        break;
      }
      case Color::black:
      {
        state_copy.en_passant_target = target_square + 8;
        break;
      }
    }
  }
  else if (static_cast<uint8_t>(promoted_to))
  {
    switch (state_copy.side_to_move) {
      case Color::white:
      {
        clear_bit(target_square, board_copy[w_pawn]);
        set_bit(target_square, board_copy[promoted_to]);
        break;
      }
      case Color::black:
      {
        clear_bit(target_square, board_copy[b_pawn]);
        set_bit(target_square, board_copy[promoted_to]);
        break;
      }
    }
  }
  else if (castling) {
    switch (target_square) {
      // white castling king side
      case chess::G1:
      {
        move_bit(chess::H1, chess::F1, board_copy[w_rook]);
        state_copy.castling_rights &= ~toul(CastlingRights::WhiteCastlingRights);
        break;
      }
      case chess::C1:
      {
        move_bit(chess::A1, chess::D1, board_copy[w_rook]);
        state_copy.castling_rights &= ~toul(CastlingRights::WhiteCastlingRights);
        break;
      }
      case chess::G8:
      {
        move_bit(chess::H8, chess::F8, board_copy[b_rook]);
        state_copy.castling_rights &= ~toul(CastlingRights::BlackCastlingRights);
        break;
      }
      case chess::C8:
      {
        move_bit(chess::A8, chess::D8, board_copy[b_rook]);
        state_copy.castling_rights &= ~toul(CastlingRights::BlackCastlingRights);
        break;
      }
      default:
        break;
    }
  }

  if (piece == w_king && !castling) {
    state_copy.castling_rights &= ~toul(CastlingRights::WhiteCastlingRights);
  }
  else if (piece == b_king && !castling) {
    state_copy.castling_rights &= ~toul(CastlingRights::BlackCastlingRights);
  }

  // update occupancies
  board_copy[w_all] = calc_white_occupancy(board_copy);
  board_copy[b_all] = calc_black_occupancy(board_copy);
  board_copy[All] = calc_global_occupancy(board_copy);

  // if the king is under attack after the move, the move is illegal
  if (_generator->is_square_attacked(((state_copy.side_to_move == Color::white)
                          ? util::bits::get_lsb_index(board_copy[w_king])
                          : util::bits::get_lsb_index(board_copy[b_king])),
                          ((state_copy.side_to_move == Color::white) ? Color::black : Color::white),
                          board_copy))
  {
    return MoveResult::Illegal;
  }
  else 
  {
    result = MoveResult::Success;
    state_copy.side_to_move = (state_copy.side_to_move == Color::white) ? Color::black : Color::white;

    _board = board_copy;
    _state = state_copy;
    _move_list.clear();
    _generator->generate_moves(_board, _state, _move_list);
  }

  return result;
}

/*******************************************************************************
 *
 * Method: find_move(uint8_t source, uint8_t target)
 *
 *******************************************************************************/
std::optional<util::bits::HashedMove> BoardManager::find_move(uint8_t source, uint8_t target) const
{
  std::optional<util::bits::HashedMove> ret;
  for (const auto& move : _move_list) {
    if (move.source == source && move.target == target) {
      ret.emplace(move);
      return ret;
    }
  }

  return std::nullopt;
}
