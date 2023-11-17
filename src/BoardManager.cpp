#include "BoardManager.hxx"
#include <cassert>
#include <random>

using namespace chess;

/*******************************************************************************
 *
 * Method: BoardManager()
 *
 *******************************************************************************/
BoardManager::BoardManager() {
  _move_list.reserve(256);
  init_attack_tables();
  init_slider_attacks();
  init_from_fen(starting_position);
}

/*******************************************************************************
 *
 * Method: BoardManager(const std::string& fen)
 *
 *******************************************************************************/
BoardManager::BoardManager(const std::string& fen) {
  _move_list.reserve(256);
  init_attack_tables();
  init_slider_attacks();
  init_from_fen(fen);
}

/*******************************************************************************
 *
 * Method: init_attack_tables()
 *
 *******************************************************************************/
constexpr void BoardManager::init_attack_tables()
{
  init_pawn_attacks();
  init_knight_attacks();
  init_king_attacks();
  init_bishop_masks();
  init_rook_masks();
}

/*******************************************************************************
 *
 * Method: init_slider_attacks()
 *
 *******************************************************************************/
void BoardManager::init_slider_attacks()
{
  const auto populate_attack_tables = [this](bool is_bishop) {
    // generates the ith permutation of the attack mask
    const auto ith_permutation =
      [](uint64_t index, int bits_in_mask, Bitboard attack_mask)
    {
      Bitboard occupancy = 0ULL;

      for (uint8_t count = 0; count < bits_in_mask; count++)
      {
        uint8_t square = util::bits::get_lsb_index(attack_mask);
        clear_bit(square, attack_mask);

        if (is_set(count, index)) {
          set_bit(square, occupancy);
        }
      }

      return occupancy;
    };

    for (uint8_t square = 0; square < 64; square++) {

      Bitboard attack_mask = (is_bishop ? bishop_masks[square] : rook_masks[square]);
      uint8_t bit_count = (is_bishop ? bishop_bits[square] : rook_bits[square]);
      uint64_t permutations = (1ULL << bit_count);

      for (uint64_t index = 0; index < permutations; index++)
      {
        Bitboard occupancy = ith_permutation(index, bit_count, attack_mask);
        auto magic = (is_bishop ? bishop_magics[square] : rook_magics[square]);

        int magic_index = (occupancy * magic) >> (64ULL - bit_count);

        if (is_bishop) {
          bishop_attacks[square][magic_index] = calc_bishop_attacks(square, occupancy);
        }
        else {
          rook_attacks[square][magic_index] = calc_rook_attacks(square, occupancy);
        }
      }
    }
  };

  populate_attack_tables(true); // bishop
  populate_attack_tables(false); // rook
}

/*******************************************************************************
 *
 * Method: init_pawn_attacks()
 *
 *******************************************************************************/
constexpr void BoardManager::init_pawn_attacks()
{
  constexpr auto get_mask = [](Color side, int square) {
    Bitboard attacks {0ULL};
    Bitboard b {0ULL};

    set_bit(square, b);

    // white
    if (side == Color::white) {
      if ((b << 7) & not_h_file)
        attacks |= (b << 7);

      if ((b << 9) & not_a_file)
        attacks |= (b << 9);

    } else {
      if ((b >> 7) & not_a_file)
        attacks |= (b >> 7);

      if ((b >> 9) & not_h_file)
        attacks |= (b >> 9);
    }
    return attacks;
  };

  for (int i = 0; i < 64; i++) {
    pawn_attacks[white][i] = get_mask(white, i);
    pawn_attacks[black][i] = get_mask(black, i);
  }
}

/*******************************************************************************
 *
 * Method: init_knight_attacks()
 *
 *******************************************************************************/
constexpr void BoardManager::init_knight_attacks()
{
  for (uint64_t square = 0; square < 64ULL; square++) {
    Bitboard attacks {0ULL};
    Bitboard b {0ULL};

    set_bit(square, b);

    if ((b << 17) & not_a_file)
      attacks |= b << 17;
    if ((b << 15) & not_h_file)
      attacks |= b << 15;
    if ((b << 10) & not_ab_file)
      attacks |= b << 10;
    if ((b << 6) & not_hg_file)
      attacks |= b << 6;
    if ((b >> 17) & not_h_file)
      attacks |= b >> 17;
    if ((b >> 15) & not_a_file)
      attacks |= b >> 15;
    if ((b >> 10) & not_hg_file)
      attacks |= b >> 10;
    if ((b >> 6) & not_ab_file)
      attacks |= b >> 6;

    knight_attacks[square] = attacks;
  }
}

/*******************************************************************************
 *
 * Method: init_king_attacks()
 * 
 *******************************************************************************/
constexpr void BoardManager::init_king_attacks()
{
  for (uint64_t square = 0; square < 64ULL; square++) {
    Bitboard attacks {0ULL};
    Bitboard b {0ULL};

    set_bit(square, b);

    if ((b << 8))
      attacks |= b << 8;
    if ((b << 9) & not_a_file)
      attacks |= b << 9;
    if ((b << 7) & not_h_file)
      attacks |= b << 7;
    if ((b << 1) & not_a_file)
      attacks |= b << 1;

    if ((b >> 8))
      attacks |= b >> 8;
    if ((b >> 9) & not_h_file)
      attacks |= b >> 9;
    if ((b >> 7) & not_a_file)
      attacks |= b >> 7;
    if ((b >> 1) & not_h_file)
      attacks |= b >> 1;

    king_attacks[square] = attacks;
  }
}

/*******************************************************************************
 *
 * Method: init_bishop_masks()
 *
 *******************************************************************************/
constexpr void BoardManager::init_bishop_masks()
{
  for (uint8_t square = 0; square < 64; square++) {
    Bitboard attacks {0ULL};

    int r = 0;
    int f = 0;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) {
      set_bit(r * 8 + f, attacks);
    }
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) {
      set_bit(r * 8 + f, attacks);
    }
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) {
      set_bit(r * 8 + f, attacks);
    }
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) {
      set_bit(r * 8 + f, attacks);
    }

    bishop_masks[square] = attacks;
  }
}

/*******************************************************************************
 *
 * Method: init_rook_masks()
 *
 *******************************************************************************/
constexpr void BoardManager::init_rook_masks()
{
  for (uint8_t square = 0; square < 64ULL; square++) {
    Bitboard attacks {0ULL};

    int r = 0;
    int f = 0;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1; r <= 6; r++) {
      set_bit(r * 8 + tf, attacks);
    }
    for (r = tr - 1; r >= 1; r--) {
      set_bit(r * 8 + tf, attacks);
    }
    for (f = tf + 1; f <= 6; f++) {
      set_bit(tr * 8 + f, attacks);
    }
    for (f = tf - 1; f >= 1; f--) {
      set_bit(tr * 8 + f, attacks);
    }

    rook_masks[square] = attacks;
  }
}

/*******************************************************************************
 *
 * Method: calc_bishop_attacks(int square, Bitboard occ)
 *
 *******************************************************************************/
constexpr Bitboard BoardManager::calc_bishop_attacks(uint8_t square, Bitboard occ) const
{
  Bitboard attacks {0ULL};

  int r, f;
  int tr = square / 8;
  int tf = square % 8;

  for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++) {
    set_bit(r * 8 + f, attacks);
    if (is_set(r * 8 + f, occ)) break;
  }
  for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++) {
    set_bit(r * 8 + f, attacks);
    if (is_set(r * 8 + f, occ)) break;
  }
  for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--) {
    set_bit(r * 8 + f, attacks);
    if (is_set(r * 8 + f, occ)) break;
  }
  for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) {
    set_bit(r * 8 + f, attacks);
    if (is_set(r * 8 + f, occ)) break;
  }

  return attacks;
}

/*******************************************************************************
 *
 * Method: calc_rook_attacks(int square, Bitboard occ)
 *
 *******************************************************************************/
constexpr Bitboard BoardManager::calc_rook_attacks(uint8_t square, Bitboard occ) const
{
  Bitboard attacks {0ULL};

  int r = 0;
  int f = 0;
  int tr = square / 8;
  int tf = square % 8;

  for (r = tr + 1; r <= 7; r++) {
    set_bit(r * 8 + tf, attacks);
    if (is_set(r * 8 + tf, occ)) break;
  }
  for (r = tr - 1; r >= 0; r--) {
    set_bit(r * 8 + tf, attacks);
    if (is_set(r * 8 + tf, occ)) break;
  }
  for (f = tf + 1; f <= 7; f++) {
    set_bit(tr * 8 + f, attacks);
    if (is_set(tr * 8 + f, occ)) break;
  }
  for (f = tf - 1; f >= 0; f--) {
    set_bit(tr * 8 + f, attacks);
    if (is_set(tr * 8 + f, occ)) break;
  }

  return attacks;
}

/*******************************************************************************
 *
 * Method: calc_white_occupancy()
 *
 *******************************************************************************/
Bitboard BoardManager::calc_white_occupancy(std::array<Bitboard, 15>& board) {
  return (board[w_pawn] |
          board[w_knight] |
          board[w_bishop] |
          board[w_rook] |
          board[w_queen] |
          board[w_king]);
}

/*******************************************************************************
 *
 * Method: calc_black_occupancy()
 *
 *******************************************************************************/
Bitboard BoardManager::calc_black_occupancy(std::array<Bitboard, 15>& board) {
  return (board[b_pawn] |
          board[b_knight] |
          board[b_bishop] |
          board[b_rook] |
          board[b_queen] |
          board[b_king]);
}

/*******************************************************************************
 *
 * Method: get_bishop_attacks(int square, Bitboard occ)
 *
 *******************************************************************************/
Bitboard BoardManager::get_bishop_attacks(uint8_t square, Bitboard occupancy) const
{
  occupancy &= bishop_masks[square];
  uint16_t index =
    (occupancy * bishop_magics[square]) >> (64ULL - bishop_bits[square]);
  return bishop_attacks[square][index];
}

/*******************************************************************************
 *
 * Method: get_rook_attacks(int square, Bitboard occ)
 *
 *******************************************************************************/
Bitboard BoardManager::get_rook_attacks(uint8_t square, Bitboard occupancy) const
{
  occupancy &= rook_masks[square];
  uint16_t index =
    (occupancy * rook_magics[square]) >> (64ULL - rook_bits[square]);
  return rook_attacks[square][index];
}

/*******************************************************************************
 *
 * Method: is_square_attacked(int square, Color side)
 * is the given square attacked by the given side
 *******************************************************************************/
bool BoardManager::is_square_attacked(uint8_t square,
                                      Color side,
                                      std::array<Bitboard, 15>& board) const
{
  // this seems counter intuitive at first glance, but heres why it works:
  // in this first case, we are checking if the square is attacked by a
  // white pawn. To do this, we check the black pawn attacks at the provided
  // square (we dont care if a black pawn is actually on the square).
  // If the location of those attacks overlaps with the presence of a
  // white pawn, then the white pawn can also attack there. This gives us the
  // desired result of seeing if 'square' is attacked by a white pawn.
  // the rest of the cases use the same principle

  if ((side == Color::white) && (pawn_attacks[black][square] & board[w_pawn]) ) {
    return true;
  }

  if ((side == Color::black) && (pawn_attacks[white][square] & board[b_pawn]) ) {
    return true;
  }

  if (knight_attacks[square] & board[(side == Color::white) ? w_knight : b_knight]) {
    return true;
  }

  if (get_bishop_attacks(square, board[All]) &
      ( (side == Color::white) ? board[w_bishop] : board[b_bishop] ) )
  {
    return true;
  }

  if (get_rook_attacks(square, board[All]) &
      ( (side == Color::white) ? board[w_rook] : board[b_rook] ) )
  {
    return true;
  }

  if (get_queen_attacks(square, board[All]) &
      ( (side == Color::white) ? board[w_queen] : board[b_queen] ) )
  {
    return true;
  }

  if (king_attacks[square] & board[(side == Color::white) ? w_king : b_king] )
  {
    return true;
  }

  return false;
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

  generate_moves();

  // TODO: castling rights, en_passant
  _state.en_passant_target = -1;
  _state.castling_rights = -1;
}

/*******************************************************************************
 *
 * Method: square_to_piece(int square)
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
 * Method: get_pseudo_legal_moves(int square)
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
  for (uint8_t i = 0; i < 64; i++) {
    board[i] = square_to_piece(i);
  }
  return board;
}

/*******************************************************************************
 *
 * Method: add_move()
 *
 *******************************************************************************/
inline void BoardManager::add_move(uint32_t source, uint32_t target,
                                   uint32_t piece, uint32_t promotion,
                                   uint32_t capture, uint32_t double_push,
                                   uint32_t enpassant, uint32_t castling)
{
  _move_list.push_back({.source = source,
                        .target= target,
                        .piece = piece,
                        .promoted = promotion,
                        .capture = capture,
                        .double_push = double_push,
                        .enpassant = enpassant,
                        .castling = castling});
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
    state_copy.en_passant_target = -1;
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
  if (is_square_attacked(((state_copy.side_to_move == Color::white)  
                          ? util::bits::get_lsb_index(board_copy[w_king]) 
                          : util::bits::get_lsb_index(board_copy[b_king])), 
                          ((state_copy.side_to_move == Color::white) ? Color::black : Color::white),
                          board_copy))
  {
    return MoveResult::Illegal;
  }
  else {
    result = MoveResult::Success;
    state_copy.side_to_move = (state_copy.side_to_move == Color::white) ? Color::black : Color::white;

    _board = board_copy;
    _state = state_copy;
    _move_list.clear();
    generate_moves();
  }

  return result;
}

/*******************************************************************************
 *
 * Method: generate_moves()
 *
 *******************************************************************************/
void BoardManager::generate_moves()
{
  switch (_state.side_to_move) {
    case Color::white:
    {
      generate_white_moves();
      break;
    }
    case Color::black:
    {
      generate_black_moves();
      break;
    }
  }
}

/*******************************************************************************
 *
 * Method: generate_white_moves()
 *
 *******************************************************************************/
void BoardManager::generate_white_moves()
{
  generate_white_pawn_moves();
  generate_white_castling_moves();
  generate_knight_moves(Color::white);
  generate_bishop_moves(Color::white);
  generate_rook_moves(Color::white);
  generate_queen_moves(Color::white);
  generate_king_moves(Color::white);
}

/*******************************************************************************
 *
 * Method: generate_black_moves()
 *
 *******************************************************************************/
void BoardManager::generate_black_moves()
{
  generate_black_pawn_moves();
  generate_black_castling_moves();
  generate_knight_moves(Color::black);
  generate_bishop_moves(Color::black);
  generate_rook_moves(Color::black);
  generate_queen_moves(Color::black);
  generate_king_moves(Color::black);
}

/*******************************************************************************
 *
 * Method: generate_white_pawn_moves()
 *
 *******************************************************************************/
void BoardManager::generate_white_pawn_moves() 
{
  Bitboard board = _board[w_pawn];
  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);

    // this is a white pawn push of 1 square
    target_square = source_square + 8;

    // if the target square is on board and not occupied
    if (target_square <= chess::H8 && !is_set(target_square, _board[All])) {
      // promotion
      if (source_square >= chess::A7 && source_square <= chess::H7) {

        add_move(source_square, target_square, Piece::w_pawn, Piece::w_queen, 0,0,0,0);
        add_move(source_square, target_square, Piece::w_pawn, Piece::w_rook,  0,0,0,0);
        add_move(source_square, target_square, Piece::w_pawn, Piece::w_bishop,0,0,0,0);
        add_move(source_square, target_square, Piece::w_pawn, Piece::w_knight,0,0,0,0);

      } else { // one move forward, no promotion
        add_move(source_square, target_square, Piece::w_pawn, 0, 0,0,0,0);

        if ((source_square >= chess::A2 && source_square <= chess::H2) &&
            !is_set(target_square + 8, _board[All]))
        {
           // two square push
           add_move(source_square, target_square + 8, Piece::w_pawn, 0, 0,1,0,0);
        }
      }
    }

    auto attacks = pawn_attacks[Color::white][source_square] & _board[b_all];

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      // promotion
      if (source_square >= chess::A7 && source_square <= chess::H7) {
        add_move(source_square, target_square, Piece::w_pawn, Piece::w_queen, 1,0,0,0);
        add_move(source_square, target_square, Piece::w_pawn, Piece::w_rook,  1,0,0,0);
        add_move(source_square, target_square, Piece::w_pawn, Piece::w_bishop,1,0,0,0);
        add_move(source_square, target_square, Piece::w_pawn, Piece::w_knight,1,0,0,0);

      } else { // capture, no promotion
        add_move(source_square, target_square, Piece::w_pawn, 0, 1,0,0,0);
      }

      clear_bit(target_square, attacks);
    }

    if (_state.en_passant_target != -1) {
      auto en_passant_attacks = pawn_attacks[Color::white][source_square] & (1ULL << _state.en_passant_target);
      if (en_passant_attacks) {
        auto attack_square = util::bits::get_lsb_index(en_passant_attacks);
        add_move(source_square, attack_square, Piece::w_pawn, 0,1,0,1,0);
      }
    }

    clear_bit(source_square, board); 
  }
}

/*******************************************************************************
 *
 * Method: generate_black_pawn_moves()
 *
 *******************************************************************************/
void BoardManager::generate_black_pawn_moves() 
{
   Bitboard board = _board[b_pawn];
   uint8_t source_square = 0;
   uint8_t target_square = 0;

   while (board) {
     source_square = util::bits::get_lsb_index(board);
     // this is a black pawn push of 1 square
     target_square = source_square - 8;

     // if the target square is on board and not occupied
     if (target_square >= chess::A1 && !is_set(target_square, _board[All])) {
       // promotion
       if (source_square >= chess::A2 && source_square <= chess::H2)
       {
         add_move(source_square, target_square, Piece::b_pawn, Piece::b_queen, 0,0,0,0);
         add_move(source_square, target_square, Piece::b_pawn, Piece::b_rook,  0,0,0,0);
         add_move(source_square, target_square, Piece::b_pawn, Piece::b_bishop,0,0,0,0);
         add_move(source_square, target_square, Piece::b_pawn, Piece::b_knight,0,0,0,0);

       } else { // one move forward, no promotion
         add_move(source_square, target_square, Piece::b_pawn, 0, 0,0,0,0);

         if (source_square >= chess::A7 && source_square <= chess::H7 &&
             !is_set(target_square - 8, _board[All]))
         {
           // two square push
           add_move(source_square, target_square - 8, Piece::b_pawn, 0, 0,1,0,0);
         }
       }
     }

     auto attacks = pawn_attacks[Color::black][source_square] & _board[w_all];

     while (attacks) {
       target_square = util::bits::get_lsb_index(attacks);

       // promotion
       if (source_square >= chess::A2 && source_square <= chess::H2) {
         add_move(source_square, target_square, Piece::b_pawn, Piece::b_queen, 1,0,0,0);
         add_move(source_square, target_square, Piece::b_pawn, Piece::b_rook,  1,0,0,0);
         add_move(source_square, target_square, Piece::b_pawn, Piece::b_bishop,1,0,0,0);
         add_move(source_square, target_square, Piece::b_pawn, Piece::b_knight,1,0,0,0);
       } else { // capture, no promotion
         add_move(source_square, target_square, Piece::b_pawn, 0, 1,0,0,0);
       }
       clear_bit(target_square, attacks);
     }

     if (_state.en_passant_target != -1) {
       auto en_passant_attacks = pawn_attacks[Color::black][source_square] & (1ULL << _state.en_passant_target);
       if (en_passant_attacks) {
         auto attack_square = util::bits::get_lsb_index(en_passant_attacks);
         add_move(source_square, attack_square, Piece::b_pawn, 0,1,0,1,0);
       }
     }
     clear_bit(source_square, board);
  }
}

/*******************************************************************************
 *
 * Method: generate_white_castling_moves()
 *
 *******************************************************************************/
void BoardManager::generate_white_castling_moves() 
{
  using namespace util;
  if (_state.castling_rights & toul(CastlingRights::WhiteKingSide))
  {
    if (!is_set(chess::F1, _board[All]) &&
        !is_set(chess::G1, _board[All]))
    {
      if (!is_square_attacked(chess::E1, Color::black, _board) &&
          !is_square_attacked(chess::F1, Color::black, _board))
      {
        add_move(chess::E1, chess::G1, Piece::w_king, 0,0,0,0,1);
       }
     }
   }

   if (_state.castling_rights & toul(CastlingRights::WhiteQueenSide))
   {
     if (!(is_set(chess::D1, _board[All])) &&
         !(is_set(chess::C1, _board[All])) &&
         !(is_set(chess::B1, _board[All])))
     {
       if (!is_square_attacked(chess::E1, Color::black, _board) &&
           !is_square_attacked(chess::D1, Color::black, _board))
       {
         add_move(chess::E1, chess::C1, Piece::w_king, 0,0,0,0,1);
       }
     }
   }
}

/*******************************************************************************
 *
 * Method: generate_black_castling_moves()
 *
 *******************************************************************************/
void BoardManager::generate_black_castling_moves() 
{
   using namespace util;
   if (_state.castling_rights & toul(CastlingRights::BlackKingSide))
   {
     if (!is_set(chess::F8, _board[All]) &&
         !is_set(chess::G8, _board[All]))
     {
       if (!is_square_attacked(chess::E8, Color::white, _board) &&
           !is_square_attacked(chess::F8, Color::white, _board))
       {
         add_move(chess::E8, chess::G8, Piece::b_king, 0,0,0,0,1);
       }
     }
   }

   if (_state.castling_rights & toul(CastlingRights::BlackQueenSide))
   {
     if (!is_set(chess::D8, _board[All]) &&
         !is_set(chess::C8, _board[All]) &&
         !is_set(chess::B8, _board[All]))
     {
       if (!is_square_attacked(chess::E8, Color::white, _board) &&
           !is_square_attacked(chess::D8, Color::white, _board))
       {
         add_move(chess::E8, chess::C8, Piece::b_king, 0,0,0,0,1);
       }
     }
   }
}

/*******************************************************************************
 *
 * Method: generate_knight_moves(Color side_to_move)
 *
 *******************************************************************************/
void BoardManager::generate_knight_moves(Color side_to_move) 
{
  Bitboard board = _board[(side_to_move == Color::white) ? w_knight : b_knight];
  Bitboard attacks = 0ULL;
  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = knight_attacks[source_square] & ~(_board[(side_to_move == Color::white) ? w_all : b_all]);

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

       // if the move is not a capture
       if (!is_set(target_square, _board[(side_to_move == Color::white) ? b_all : w_all])) {
         add_move(source_square, target_square, (side_to_move == Color::white) ? w_knight : b_knight, 0, 0,0,0,0);
       } else {
         add_move(source_square, target_square, (side_to_move == Color::white) ? w_knight : b_knight, 0, 1,0,0,0);
       }

      clear_bit(target_square, attacks);
    }
    clear_bit(source_square, board);
  }
}

/*******************************************************************************
 *
 * Method: generate_bishop_moves(Color side_to_move)
 *
 *******************************************************************************/
void BoardManager::generate_bishop_moves(Color side_to_move) 
{
  Bitboard board = _board[(side_to_move == Color::white) ? w_bishop : b_bishop];
  Bitboard attacks = 0ULL;
  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = get_bishop_attacks(source_square, _board[All]) & ~(_board[(side_to_move == Color::white) ? w_all : b_all]);

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      // if the move is not a capture 
      if (!is_set(target_square, _board[side_to_move == Color::white ? b_all : w_all])) {
        add_move(source_square, target_square, (side_to_move == Color::white) ? w_bishop : b_bishop, 0, 0,0,0,0);
      } else { 
        add_move(source_square, target_square, (side_to_move == Color::white) ? w_bishop : b_bishop, 0, 1,0,0,0);
      }

      clear_bit(target_square, attacks);
    }

    clear_bit(source_square, board);
  }
}

/*******************************************************************************
 *
 * Method: generate_rook_moves(Color side_to_move)
 *
 *******************************************************************************/
void BoardManager::generate_rook_moves(Color side_to_move) 
{
  Bitboard board = _board[(side_to_move == Color::white) ? w_rook : b_rook];
  Bitboard attacks = 0ULL;
  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = get_rook_attacks(source_square, _board[All]) & ~(_board[(side_to_move == Color::white) ? w_all : b_all]);

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      // if the move is not a capture
      if (!is_set(target_square, _board[side_to_move == Color::white ? b_all : w_all])) {
        add_move(source_square, target_square, (side_to_move == Color::white) ? w_rook : b_rook, 0, 0,0,0,0);
      } else { 
        add_move(source_square, target_square, (side_to_move == Color::white) ? w_rook : b_rook, 0, 1,0,0,0);
      }

      clear_bit(target_square, attacks);
    }

    clear_bit(source_square, board);
  }
}

/*******************************************************************************
 *
 * Method: generate_queen_moves(Color side_to_move)
 *
 *******************************************************************************/
void BoardManager::generate_queen_moves(Color side_to_move) 
{
  Bitboard board = _board[(side_to_move == Color::white) ? w_queen : b_queen];
  Bitboard attacks = 0ULL;
  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = get_queen_attacks(source_square, _board[All]) & ~(_board[(side_to_move == Color::white) ? w_all : b_all]);

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      // if the move is not a capture 
      if (!is_set(target_square, _board[side_to_move == Color::white ? b_all : w_all])) {
        add_move(source_square, target_square, (side_to_move == Color::white) ? w_queen : b_queen, 0, 0,0,0,0);
      } else { 
        add_move(source_square, target_square, (side_to_move == Color::white) ? w_queen : b_queen, 0, 1,0,0,0);
      }

      clear_bit(target_square, attacks);
    }

    clear_bit(source_square, board);
  }
}

/*******************************************************************************
 *
 * Method: generate_king_moves(Color side_to_move)
 *
 *******************************************************************************/
void BoardManager::generate_king_moves(Color side_to_move) {
  Bitboard board = _board[(side_to_move == Color::white) ? w_king : b_king];
  Bitboard attacks = 0ULL;
  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = king_attacks[source_square] & (side_to_move == Color::white ? ~(_board[w_all]) : ~(_board[b_all]));

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      // if the move is not a capture 
      if (!is_set(target_square, _board[side_to_move == Color::white ? b_all : w_all])) {
        add_move(source_square, target_square, (side_to_move == Color::white) ? w_king : b_king, 0, 0,0,0,0);
      } else { 
        add_move(source_square, target_square, (side_to_move == Color::white) ? w_king : b_king, 0, 1,0,0,0);
      }

      clear_bit(target_square, attacks);
    }
    clear_bit(source_square, board);
  }
}

/*******************************************************************************
 *
 * Method: find_move(int source, int target)
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

/*******************************************************************************
 *
 * Method: test_attack_lookup()
 *
 *******************************************************************************/
void BoardManager::test_attack_lookup()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for (auto i = 0; i < 100000000; i ++) {
    std::uniform_int_distribution<uint64_t> dis(
      std::numeric_limits<std::uint64_t>::min(),
      std::numeric_limits<std::uint64_t>::max());

    Bitboard occ = {dis(gen)}; 
    uint8_t r_square = (rand() % 64);

    Bitboard bishop_actual = get_bishop_attacks(r_square, occ);
    Bitboard bishop_calculated = calc_bishop_attacks(r_square, occ);

    Bitboard rook_actual = get_rook_attacks(r_square, occ);
    Bitboard rook_calculated = calc_rook_attacks(r_square, occ);
    Bitboard queen_actual = get_queen_attacks(r_square, occ);
    Bitboard queen_calculated =
      (calc_rook_attacks(r_square, occ)) | calc_bishop_attacks(r_square, occ);

    if (bishop_actual != bishop_calculated) {
      std::cout << "attack fetch falied for occ on square: " << r_square <<"\n";
      print_board(occ);
      std::cout << "expected:\n";
      print_board(bishop_calculated);

      std::cout << "actual:\n";
      print_board(bishop_actual);
      std::cout << "mask: \n";
      print_board(bishop_masks[r_square]);

      assert(false);
    }

    if (rook_actual != rook_calculated) {
      std::cout << "attack fetch falied for occ on square: " << r_square <<"\n";
      print_board(occ);
      std::cout << "expected:\n";
      print_board(rook_calculated);
      std::cout << "actual:\n";
      print_board(rook_actual);
      std::cout << "mask: \n";
      print_board(rook_masks[r_square]);
      assert(false);
    }

    if (queen_actual != queen_calculated) {
      std::cout << "attack fetch falied for occ on square: " << r_square <<"\n";
      print_board(occ);
      std::cout << "expected:\n";
      print_board(queen_calculated);
      std::cout << "actual:\n";
      print_board(queen_actual);
      std::cout << "mask: \n";
      print_board(rook_masks[r_square] | bishop_masks[r_square]);
      assert(false);
    }
  }
}
