#include "BoardManager.hxx"
#include <cassert>

using namespace chess;

/*******************************************************************************
 *
 * Method: BoardManager(std::shared_ptr<MoveGen>)
 *
 *******************************************************************************/
BoardManager::BoardManager(const MoveGen* g)
  : _generator(g)
{
  _move_list.reserve(256);
  _history.reserve(150);
  init_from_fen(chess::starting_position);
}

/*******************************************************************************
 *
 * Method: BoardManager(std::shared_ptr<MoveGen>, const string& fen)
 *
 *******************************************************************************/
BoardManager::BoardManager(const MoveGen* g, const std::string& fen)
  : _generator(g)
{
  _move_list.reserve(256);
  _history.reserve(150);
  init_from_fen(fen);
}

/*******************************************************************************
 *
 * Method: init_from_fen(Bitboard b)
 * https://en.wikipedia.org/wiki/Forsyth–Edwards_Notation
 *******************************************************************************/
void BoardManager::init_from_fen(const std::string &fen)
{
  // populate board occupancies and game state
  // using a FEN string
  for (auto& b : _board) {
    b = 0ULL;
  }

  auto view = fen | std::views::split(' ') | std::views::transform([](auto&& range) {
                    return std::string(range.begin(), range.end());
                   });

  std::vector<std::string> tokens(view.begin(), view.end());

  assert(tokens.size() == 6);

  auto& fen_board_layout = tokens[0];
  auto& fen_turn = tokens[1];
  auto& fen_castling = tokens[2];
  auto& fen_en_passant = tokens[3];
  auto& fen_half_clock = tokens[4];
  auto& fen_move_cnt = tokens[5];

  // parse the board layout portion of the string
  uint32_t rank = 7;
  uint32_t file = 0;
  
  for (auto c : fen_board_layout) {
    if (isdigit(c)) {
      file += (c - '0');
    } else if (c == '/') {
      rank--;
      file = 0;
    } else if (c == ' ') {
      break;
    } else {
      auto piece = util::fen::char_to_piece(c);
      uint8_t square = rank * 8 + file;
      set_bit(square, _board[piece]);
      file++;
    }
  }

  _board[w_all] = calc_white_occupancy(_board);
  _board[b_all] = calc_black_occupancy(_board);
  _board[All] = calc_global_occupancy(_board);

  // get the turn info
  _state.side_to_move = fen_turn == "w" ? Color::white : Color::black;

  // castling rights
  if (fen_castling == "-") {
    _state.castling_rights = ~(util::toul(CastlingRights::WhiteCastlingRights) |
                               util::toul(CastlingRights::BlackCastlingRights));
  } else {
    if (util::contains(fen_castling, 'K')) {
      _state.castling_rights |= util::toul(CastlingRights::WhiteKingSide);
    }
    if (util::contains(fen_castling, 'Q')) {
      _state.castling_rights |= util::toul(CastlingRights::WhiteQueenSide);
    }
    if (util::contains(fen_castling, 'k')) {
      _state.castling_rights |= util::toul(CastlingRights::BlackKingSide);
    }
    if (util::contains(fen_castling, 'q')) {
      _state.castling_rights |= util::toul(CastlingRights::BlackQueenSide);
    }
  }

  // en passant information
  if (fen_en_passant == "-") {
    _state.en_passant_target = chess::NoSquare;
  }
  else if (auto index = util::fen::algebraic_to_index(fen_en_passant))
  {
    _state.en_passant_target = index.value();
  }

  // half move clock
  _state.half_move_clock = std::stoul(fen_half_clock);

  // full move count
  _state.full_move_count = std::stoul(fen_move_cnt);

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
  for (auto p : chess::AllPieces) {
    if (is_set(square, _board[p])) {
      ret.emplace(p);
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
 * Method: is_check(const Board&, const State&)
 *
 *******************************************************************************/
bool BoardManager::is_check(const Board& board_, const State& state_)
{
  return
    _generator->is_square_attacked(util::bits::get_lsb_index(state_.side_to_move == Color::white ? board_[w_king] : board_[b_king]),
                                   (state_.side_to_move == Color::white) ? Color::black : Color::white,
                                   board_);
}

/*******************************************************************************
 *
 * Method: make_move(uint32_t move)
 *
 *******************************************************************************/
MoveResult BoardManager::try_move(const chess::Move& proposed)
{
  MoveResult result = MoveResult::Illegal;

  if (auto move = find_move(proposed.from, proposed.to))
  {
    if (result = make_move(move.value());
        result != MoveResult::Illegal)
    {
      bool no_legal_moves = true;

      // now check if there are no legal moves
      for (auto m : _move_list) {
        BoardManager temp = *this;

        if (temp.make_move(m) != MoveResult::Illegal) {
          no_legal_moves = false;
          break;
        }
      }

      if (no_legal_moves) {
        result = is_check(_board, _state) ? MoveResult::Checkmate
                                          : MoveResult::Stalemate;
      }
    }
  }

  return result;
}

/*******************************************************************************
 *
 * Method: make_move(uint32_t move)
 *
 *******************************************************************************/
MoveResult BoardManager::make_move(
    const util::bits::HashedMove& move)
{
  using namespace util;

  MoveResult result = MoveResult::Illegal;

  // copy the board and state in case of illegal move
  Board board_copy = _board;
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

    const auto& pieces =
          (state_copy.side_to_move == Color::white) ? chess::BlackPieces
                                                    : chess::WhitePieces;

    // loop over possible capture pieces
    for (auto p : pieces)
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

  // if the move puts themselves in check -> Illegal
  if (is_check(board_copy, state_copy)) {
    return MoveResult::Illegal;
  }
  else 
  {
    result = MoveResult::Valid;
    state_copy.side_to_move = (state_copy.side_to_move == Color::white) ? Color::black
                                                                        : Color::white;

    // successful move from black means full move cnt++
    if (state_copy.side_to_move == Color::white) {
      state_copy.full_move_count++;
    }

    // the half move clock is the number of
    // half moves since the last pawn move or any capture
    if (!capture && ( piece != w_pawn || piece != b_pawn))
    {
      state_copy.half_move_clock++;
    }

    _board = board_copy;
    _state = state_copy;
    _history.push_back(generate_fen());

    _move_list.clear();
    _generator->generate_moves(_board, _state, _move_list);
  }

  return result;
}

/*******************************************************************************
 *
 * Method: generate_fen()
 * - return a fen string represenation of the current board
 * https://en.wikipedia.org/wiki/Forsyth–Edwards_Notation
 *******************************************************************************/
std::string BoardManager::generate_fen()
{
  std::string fen = "";

  uint8_t cur_missing_count = 0;
  static constexpr auto ranges = {56, 48, 40, 32, 24, 16, 8, 0};

  for (int start : ranges) {
    for (int square : std::views::iota(start, start + 8)) {

      std::optional<Piece> piece_at_sq;

      // see if there is a piece at this square
      for (auto p : chess::AllPieces) {
        if (is_set(square, _board[p])) {
          piece_at_sq = square_to_piece(square);
          break;
        }
      }

      if (piece_at_sq) {
        std::string temp = "";
        if (cur_missing_count) {
          temp += std::to_string(cur_missing_count);
        }
        temp += util::fen::piece_to_char(*piece_at_sq);
        fen += temp;
        cur_missing_count = 0;

      } else {
        cur_missing_count++;
      }
    }

    if (cur_missing_count) {
      fen += std::to_string(cur_missing_count);
      cur_missing_count = 0;
    }

    if (start != *(ranges.end() - 1)) {
      fen += "/";
    }
  }

  fen += " ";

  fen += _state.side_to_move == Color::white ? 'w' : 'b';

  fen += " ";

  if (!_state.castling_rights) {
    fen += '-';
  } else {
    if (_state.castling_rights & util::toul(CastlingRights::WhiteKingSide)) {
      fen += "K";
    }
    if (_state.castling_rights & util::toul(CastlingRights::WhiteQueenSide)) {
      fen += "Q";
    }
    if (_state.castling_rights & util::toul(CastlingRights::BlackKingSide)) {
      fen += "k";
    }
    if (_state.castling_rights & util::toul(CastlingRights::BlackQueenSide)) {
      fen += "q";
    }
  }

  fen += " ";

  if (_state.en_passant_target == chess::NoSquare) {
    fen += "-";
  } else if (auto str = util::fen::index_to_algebraic(_state.en_passant_target)) {
    fen += *str;
  }

  fen += " ";

  fen += (std::to_string(_state.half_move_clock)) + " " +
          std::to_string(_state.full_move_count);

  return fen;
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
