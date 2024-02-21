#include "BoardManager.hxx"
#include <cassert>

using namespace chess;

/*******************************************************************************
 *
 * Method: BoardManager(const *MoveGenerator)
 *
 *******************************************************************************/
BoardManager::BoardManager(const MoveGenerator* g)
  : _generator(g)
{
  _move_list.reserve(256);
  _history.reserve(150);
  initFromFen(chess::starting_position);
  assert(generateFen() == chess::starting_position);
}

/*******************************************************************************
 *
 * Method: BoardManager(const MoveGenerator*, const string& fen)
 *
 *******************************************************************************/
BoardManager::BoardManager(const MoveGenerator* g, const std::string& fen)
  : _generator(g)
{
  _move_list.reserve(256);
  _history.reserve(150);
  initFromFen(fen);
}

/*******************************************************************************
 *
 * Method: initFromFen(const std::string& fen)
 * https://en.wikipedia.org/wiki/Forsyth–Edwards_Notation
 *******************************************************************************/
void BoardManager::initFromFen(const std::string &fen)
{
  std::fill(_board.begin(), _board.end(), 0ULL);
  _history.clear();
  _move_list.clear();

  if (auto val = makeBoardFromFen(fen))
  {
    auto [board, state] = *val;
    _board = board;
    _state = state;

    _history.push_back(generateFen());

    assert(generateFen() == fen);

    _generator->generateMoves(_board, _state, _move_list);

  } else {
    // critical error if the FEN string is invalid
    assert(false);
  }
}

/*******************************************************************************
 *
 * Method: makeBoardFromFen(const std::string& fen)
 *
 *******************************************************************************/
std::optional<std::pair<Board,State>>
    BoardManager::makeBoardFromFen(const std::string& fen) const
{
  Board board;
  State state;

  // populate board occupancies and game state
  // using a FEN string
  for (auto& b : board) {
    b = 0ULL;
  }

  auto view = fen | std::views::split(' ')
                  | std::views::transform([](auto&& c) {
                      return std::string(c.begin(), c.end());
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
  
  for (auto c : fen_board_layout)
  {
    if (isdigit(c)) {
      file += (c - '0');
    }
    else if (c == '/') {
      rank--;
      file = 0;
    }
    else if (auto piece = util::fen::char_to_piece(c)) {
      uint8_t square = rank * 8 + file;
      set_bit(square, board[*piece]);
      file++;
    }
    else {
      return std::nullopt;
    }
  }

  board[WhiteAll] = calcWhiteOccupancy(board);
  board[BlackAll] = calcBlackOccupancy(board);
  board[All] = calcGlobalOccupancy(board);

  // get the turn info
  state.side_to_move = fen_turn == "w" ? White : Black;

  // castling rights
  if (fen_castling == "-") {
    state.castling_rights = ~(util::toul(CastlingRights::WhiteCastlingRights) |
                              util::toul(CastlingRights::BlackCastlingRights));
  } else {
    if (util::contains(fen_castling, 'K')) {
      state.castling_rights |= util::toul(CastlingRights::WhiteKingSide);
    }
    if (util::contains(fen_castling, 'Q')) {
      state.castling_rights |= util::toul(CastlingRights::WhiteQueenSide);
    }
    if (util::contains(fen_castling, 'k')) {
      state.castling_rights |= util::toul(CastlingRights::BlackKingSide);
    }
    if (util::contains(fen_castling, 'q')) {
      state.castling_rights |= util::toul(CastlingRights::BlackQueenSide);
    }
  }

  // en passant information
  if (fen_en_passant == "-") {
    state.en_passant_target = chess::NoSquare;
  }
  else if (auto index = util::fen::algebraic_to_index(fen_en_passant))
  {
    state.en_passant_target = index.value();
  }

  // half move clock
  state.half_move_clock = std::stoul(fen_half_clock);

  // full move count
  state.full_move_count = std::stoul(fen_move_cnt);

  return std::make_tuple(board, state);
}

/*******************************************************************************
 *
 * Method: get_pseudo_legal_moves(uint8_t square)
 *
 *******************************************************************************/
std::vector<uint8_t> BoardManager::getPseudoLegalMoves(uint8_t square) const
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
 * Method: is_check(const Board&, const State&)
 *
 *******************************************************************************/
bool BoardManager::isCheck(const Board& board_, const State& state_)
{
  return
    _generator->isSquareAttacked(util::bits::get_lsb_index(state_.side_to_move == White ? board_[WhiteKing] : board_[BlackKing]),
                                 (state_.side_to_move == White) ? Black : White,
                                 board_);
}

/*******************************************************************************
 *
 * Method: make_move(uint32_t move)
 *
 *******************************************************************************/
std::tuple<MoveResult, util::bits::HashedMove> BoardManager::tryMove(const chess::Move& proposed)
{
  MoveResult result = MoveResult::Illegal;
  util::bits::HashedMove move_made;

  if (auto move = findMove(proposed.from,
                           proposed.to,
                           util::toul(proposed.promoted_to)))
  {
    if (result = makeMove(move.value());
        result != MoveResult::Illegal)
    {
      move_made = move.value();

      bool no_legal_moves = true;
      bool was_check = isCheck(_board, _state);

      if (was_check) {
        result = MoveResult::Check;
      }

      // now check if there are no legal moves
      for (auto m : _move_list) {
        BoardManager temp = *this;

        if (temp.makeMove(m) != MoveResult::Illegal) {
          no_legal_moves = false;
          break;
        }
      }

      if (no_legal_moves) {
        result = was_check ? MoveResult::Checkmate
                           : MoveResult::Stalemate;
      }
    }
  }

  return std::make_tuple(result, move_made);
}

/*******************************************************************************
 *
 * Method: make_move(uint32_t move)
 *
 *******************************************************************************/
MoveResult BoardManager::makeMove(
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

  // do the move on the pieces bitboard
  move_bit(source_square, target_square, board_copy[piece]);

  // if the move was a capture move, remove the captured piece
  if (capture) {

    const auto& pieces =
          (state_copy.side_to_move == White) ? chess::BlackPieces
                                             : chess::WhitePieces;
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
      case White:
      {
        clear_bit(target_square - 8, board_copy[BlackPawn]);
        break;
      }
      // if black made an en passant capture
      case Black:
      {
        clear_bit(target_square + 8, board_copy[WhitePawn]);
        break;
      }
    }
  } else {
    state_copy.en_passant_target = chess::NoSquare;
  }

  if (double_push) {
    switch (state_copy.side_to_move) {
      case White:
      {
        state_copy.en_passant_target = target_square - 8;
        break;
      }
      case Black:
      {
        state_copy.en_passant_target = target_square + 8;
        break;
      }
    }
  }
  else if (static_cast<uint8_t>(promoted_to))
  {
    switch (state_copy.side_to_move) {
      case White:
      {
        clear_bit(target_square, board_copy[WhitePawn]);
        set_bit(target_square, board_copy[promoted_to]);
        break;
      }
      case Black:
      {
        clear_bit(target_square, board_copy[BlackPawn]);
        set_bit(target_square, board_copy[promoted_to]);
        break;
      }
    }
  }
  else if (castling) {
    switch (target_square) {
      case chess::G1:
      {
        move_bit(chess::H1, chess::F1, board_copy[WhiteRook]);
        state_copy.castling_rights &= ~toul(CastlingRights::WhiteCastlingRights);
        break;
      }
      case chess::C1:
      {
        move_bit(chess::A1, chess::D1, board_copy[WhiteRook]);
        state_copy.castling_rights &= ~toul(CastlingRights::WhiteCastlingRights);
        break;
      }
      case chess::G8:
      {
        move_bit(chess::H8, chess::F8, board_copy[BlackRook]);
        state_copy.castling_rights &= ~toul(CastlingRights::BlackCastlingRights);
        break;
      }
      case chess::C8:
      {
        move_bit(chess::A8, chess::D8, board_copy[BlackRook]);
        state_copy.castling_rights &= ~toul(CastlingRights::BlackCastlingRights);
        break;
      }
      default:
        break;
    }
  }

  if (piece == WhiteKing && !castling) {
    state_copy.castling_rights &= ~toul(CastlingRights::WhiteCastlingRights);
  }
  else if (piece == BlackKing && !castling) {
    state_copy.castling_rights &= ~toul(CastlingRights::BlackCastlingRights);
  }

  // update occupancies
  board_copy[WhiteAll] = calcWhiteOccupancy(board_copy);
  board_copy[BlackAll] = calcBlackOccupancy(board_copy);
  board_copy[All] = calcGlobalOccupancy(board_copy);

  // if the move puts themselves in check -> Illegal
  if (isCheck(board_copy, state_copy)) {
    return MoveResult::Illegal;
  }
  else 
  {
    result = MoveResult::Valid;
    state_copy.side_to_move = (state_copy.side_to_move == White) ? Black
                                                                 : White;

    // successful move from black means full move cnt++
    if (state_copy.side_to_move == White) {
      state_copy.full_move_count++;
    }

    // the half move clock is the number of
    // half moves since the last pawn move or any capture
    if (!capture && ( piece != WhitePawn || piece != BlackPawn))
    {
      state_copy.half_move_clock++;
    }

    _board = board_copy;
    _state = state_copy;
    _history.push_back(generateFen());

    _move_list.clear();
    _generator->generateMoves(_board, _state, _move_list);
  }

  return result;
}

/*******************************************************************************
 *
 * Method: find_move(uint8_t source, uint8_t target)
 *
 *******************************************************************************/
std::optional<util::bits::HashedMove> BoardManager::findMove(uint8_t source,
                                                             uint8_t target,
                                                             uint32_t promoted_to) const
{
  std::optional<util::bits::HashedMove> ret;
  for (const auto& move : _move_list) {
    if (move.source == source &&
        move.target == target &&
        move.promoted == promoted_to)
    {
      ret.emplace(move);
      return ret;
    }
  }

  return std::nullopt;
}
