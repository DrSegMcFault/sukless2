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
  init_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  init_attack_tables();
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
  init_slider_attacks();
}

/*******************************************************************************
 *
 * Method: init_slider_attacks()
 *
 *******************************************************************************/
constexpr void BoardManager::init_slider_attacks()
{
  const auto populate_attack_tables = [this](bool is_bishop) {
    // generates the ith permutation of the attack mask
    const auto ith_permutation =
      [](uint64_t index, int bits_in_mask, Bitboard attack_mask)
    {
      Bitboard occupancy = 0ULL;

      for (int count = 0; count < bits_in_mask; count++)
      {
        int square = util::bits::get_lsb_index(attack_mask);
        util::bits::clear(square, attack_mask);

        if (index & (1ULL << count)) {
          occupancy |= (1ULL << square);
        }
      }

      return occupancy;
    };

    for (int square = 0; square < 64; square++) {

      Bitboard attack_mask = (is_bishop ? bishop_masks[square] : rook_masks[square]);
      int bit_count = (is_bishop ? bishop_bits[square] : rook_bits[square]);
      int permutations = (1ULL << bit_count);

      for (int index = 0; index < permutations; index++)
      {
        Bitboard occupancy = ith_permutation(index, bit_count, attack_mask);
        auto magic = (is_bishop ? bishop_magics[square] : rook_magics[square]);

        int magic_index = (occupancy * magic) >> (64 - bit_count);

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

    // set the bit corresponding to square
    b |= (1ULL << square);

    // white
    if (side == white) {
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
  constexpr auto get_mask = [](int square) {
    Bitboard attacks {0ULL};
    Bitboard b {0ULL};

    // set the bit corresponding to square
    b |= (1ULL << square);
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

    return attacks;
  };

  for (int i = 0; i < 64; i++) {
    knight_attacks[i] = get_mask(i);
  }
}

/*******************************************************************************
 *
 * Method: init_king_attacks()
 * 
 *******************************************************************************/
constexpr void BoardManager::init_king_attacks()
{
  constexpr auto get_mask = [](int square) {
    Bitboard attacks {0ULL};
    Bitboard b {0ULL};

    // set the bit corresponding to square
    b |= (1ULL << square);

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

    return attacks;
  };

  for (int i = 0; i < 64; i++) {
    king_attacks[i] = get_mask(i);
  }
}

/*******************************************************************************
 *
 * Method: init_bishop_masks()
 *
 *******************************************************************************/
constexpr void BoardManager::init_bishop_masks()
{
  constexpr auto get_mask = [] (int square) {
    Bitboard attacks {0ULL};

    int r = 0;
    int f = 0;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) {
      attacks |= (1ULL << (r * 8 + f));
    }
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) {
      attacks |= (1ULL << (r * 8 + f));
    }
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) {
      attacks |= (1ULL << (r * 8 + f));
    }
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) {
      attacks |= (1ULL << (r * 8 + f));
    }

    return attacks;
  };

  for (int i = 0; i < 64; i++) {
    bishop_masks[i] = get_mask(i);
  }
}

/*******************************************************************************
 *
 * Method: init_rook_masks()
 *
 *******************************************************************************/
constexpr void BoardManager::init_rook_masks()
{
  constexpr auto get_mask = [] (int square) {
    Bitboard attacks {0ULL};

    int r = 0;
    int f = 0;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1; r <= 6; r++) {
      attacks |= (1ULL << (r * 8 + tf));
    }
    for (r = tr - 1; r >= 1; r--) {
      attacks |= (1ULL << (r * 8 + tf));
    }
    for (f = tf + 1; f <= 6; f++) {
      attacks |= (1ULL << (tr * 8 + f));
    }
    for (f = tf - 1; f >= 1; f--) {
      attacks |= (1ULL << (tr * 8 + f));
    }

    return attacks;
  };

  for (int i = 0; i < 64; i++) {
    rook_masks[i] = get_mask(i);
  }
}

/*******************************************************************************
 *
 * Method: calc_bishop_attacks(int square, Bitboard occ)
 *
 *******************************************************************************/
constexpr Bitboard BoardManager::calc_bishop_attacks(int square, Bitboard occ) const
{
  Bitboard attacks {0ULL};

  int r, f;
  int tr = square / 8;
  int tf = square % 8;

  for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++) {
    attacks |= (1ULL << (r * 8 + f));
    if ((1ULL << (r * 8 + f)) & occ) break;
  }
  for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++) {
    attacks |= (1ULL << (r * 8 + f));
    if ((1ULL << (r * 8 + f)) & occ) break;
  }
  for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--) {
    attacks |= (1ULL << (r * 8 + f));
    if ((1ULL << (r * 8 + f)) & occ) break;
  }
  for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) {
    attacks |= (1ULL << (r * 8 + f));
    if ((1ULL << (r * 8 + f)) & occ) break;
  }

  return attacks;
}

/*******************************************************************************
 *
 * Method: calc_rook_attacks(int square, Bitboard occ)
 *
 *******************************************************************************/
constexpr Bitboard BoardManager::calc_rook_attacks(int square, Bitboard occ) const
{
  Bitboard attacks {0ULL};

  int r = 0;
  int f = 0;
  int tr = square / 8;
  int tf = square % 8;

  for (r = tr + 1; r <= 7; r++) {
    attacks |= (1ULL << (r * 8 + tf));
    if ((1ULL << (r * 8 + tf)) & occ) break;
  }
  for (r = tr - 1; r >= 0; r--) {
    attacks |= (1ULL << (r * 8 + tf));
    if ((1ULL << (r * 8 + tf)) & occ) break;
  }
  for (f = tf + 1; f <= 7; f++) {
    attacks |= (1ULL << (tr * 8 + f));
    if ((1ULL << (tr * 8 + f)) & occ) break;
  }
  for (f = tf - 1; f >= 0; f--) {
    attacks |= (1ULL << (tr * 8 + f));
    if ((1ULL << (tr * 8 + f)) & occ) break;
  }

  return attacks;
}

/*******************************************************************************
 *
 * Method: calc_white_occupancy()
 *
 *******************************************************************************/
Bitboard BoardManager::calc_white_occupancy() {
  return (_board[w_pawn] |
          _board[w_knight] |
          _board[w_bishop] |
          _board[w_rook] |
          _board[w_queen] |
          _board[w_king]);
}

/*******************************************************************************
 *
 * Method: calc_black_occupancy()
 *
 *******************************************************************************/
Bitboard BoardManager::calc_black_occupancy() {
  return (_board[b_pawn] |
          _board[b_knight] |
          _board[b_bishop] |
          _board[b_rook] |
          _board[b_queen] |
          _board[b_king]);
}

/*******************************************************************************
 *
 * Method: get_bishop_attacks(int square, Bitboard occ)
 *
 *******************************************************************************/
Bitboard BoardManager::get_bishop_attacks(Pos square, Bitboard occupancy) const
{
  occupancy &= bishop_masks[square];
  int index =
    (occupancy * bishop_magics[square]) >> (64ULL - bishop_bits[square]);
  return bishop_attacks[square][index];
}

/*******************************************************************************
 *
 * Method: get_rook_attacks(int square, Bitboard occ)
 *
 *******************************************************************************/
Bitboard BoardManager::get_rook_attacks(Pos square, Bitboard occupancy) const
{
  occupancy &= rook_masks[square];
  int index =
    (occupancy * rook_magics[square]) >> (64ULL - rook_bits[square]);
  return rook_attacks[square][index];
}

/*******************************************************************************
 *
 * Method: get_rook_attacks(int square, Bitboard occ)
 *
 *******************************************************************************/
Bitboard BoardManager::get_queen_attacks(Pos square, Bitboard occ) const
{
  // i was getting a warning for bitwise oring the
  // bishop and rook get attacks methods, so, this was born
  Bitboard ret{0ULL};

  // bishop lookup
  auto b_occ = occ;
  b_occ &= bishop_masks[square];
  int b_index = 
    (b_occ * bishop_magics[square]) >> (64ULL - bishop_bits[square]);

  ret = bishop_attacks[square][b_index];

  // rook lookup
  auto r_occ = occ;
  r_occ &= rook_masks[square];
  int r_index = 
    (r_occ * rook_magics[square]) >> (64ULL - rook_bits[square]);

  ret |= rook_attacks[square][r_index];

  return ret;
}

/*******************************************************************************
 *
 * Method: is_square_attacked(int square, Color side)
 * is the given square attacked by the given side
 *******************************************************************************/
bool BoardManager::is_square_attacked(int square, Color side) const
{
  // this seems counter intuitive at first glance, but heres why it works:
  // in this first case, we are checking if the square is attacked by a
  // white pawn. To do this, we check the black pawn attacks at the provided
  // square (we dont care if a black pawn is actually on the square).
  // If the location of those attacks overlaps with the presence of a
  // white pawn, then the white pawn can also attack there. This gives us the
  // desired result of seeing if 'square' is attacked by a white pawn.
  // the rest of the cases use the same principle

  if ((side == Color::white) && (pawn_attacks[black][square] & _board[w_pawn]) ) {
    return true;
  }

  if ((side == Color::black) && (pawn_attacks[white][square] & _board[b_pawn]) ) {
    return true;
  }

  if (knight_attacks[square] & _board[(side == Color::white) ? w_knight : b_knight]) {
    return true;
  }

  if (get_bishop_attacks(static_cast<Pos>(square), _board[All]) & 
      ( (side == Color::white) ? _board[w_bishop] : _board[b_bishop] ) )
  {
    return true;
  }

  if (get_rook_attacks(static_cast<Pos>(square), _board[All]) & 
      ( (side == Color::white) ? _board[w_rook] : _board[b_rook] ) )
  {
    return true;
  }

  if (get_queen_attacks(static_cast<Pos>(square), _board[All]) & 
      ( (side == Color::white) ? _board[w_queen] : _board[b_queen] ) )
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
  uint32_t square = 0;
  for (auto c : fen) {
    if (isdigit(c)) {
      square += atoi(&c);
      continue;
    } else if (c == '/') {
      continue;
    } else if (c == ' ') {
      break;
    }
    auto piece = util::fen::piece_from_char(c);
    util::bits::set(square, _board[piece]);
    square++;
  }

  _board[w_all] = calc_white_occupancy();
  _board[b_all] = calc_black_occupancy();
  _board[All] = calc_global_occupancy();

  print();
}

/*******************************************************************************
 *
 * Method: square_to_piece(int square)
 *
 *******************************************************************************/
std::optional<Piece> BoardManager::square_to_piece(int square) const
{
  std::optional<Piece> ret;
  for (int p = static_cast<int>(w_pawn); p <= static_cast<int>(b_king); p++) {
    if (util::bits::is_set(square, _board[p])) {
      ret.emplace(static_cast<Piece>(p));
      return ret;
    }
  }

  return ret;
}

/*******************************************************************************
 *
 * Method: get_pseudo_legal_attack_bitboard()
 *
 *******************************************************************************/
Bitboard BoardManager::get_pseudo_legal_attack_bitboard(Piece p, int square) const
{
  switch (p) {
    case w_pawn:
      return pawn_attacks[Color::white][square] & ~(_board[w_all]);

    case b_pawn:
      return pawn_attacks[Color::black][square] & ~(_board[b_all]);

    case w_knight:
      return knight_attacks[square] & ~(_board[w_all]);

    case b_knight:
      return knight_attacks[square] & ~(_board[b_all]);

    case w_bishop:
      return get_bishop_attacks(static_cast<Pos>(square), _board[All]) & ~(_board[w_all]);

    case b_bishop:
      return get_bishop_attacks(static_cast<Pos>(square), _board[All]) & ~(_board[b_all]);

    case w_rook:
      return get_rook_attacks(static_cast<Pos>(square), _board[All]) & ~(_board[w_all]);

    case b_rook:
      return get_rook_attacks(static_cast<Pos>(square), _board[All]) & ~(_board[b_all]);

    case w_queen:
      return get_queen_attacks(static_cast<Pos>(square), _board[All]) & ~(_board[w_all]);

    case b_queen:
      return get_queen_attacks(static_cast<Pos>(square), _board[All]) & ~(_board[b_all]);

    case w_king:
      return king_attacks[square] & ~(_board[w_all]);

    case b_king:
      return king_attacks[square] & ~(_board[b_all]);

    case b_all:
    case w_all:
    case All:
      return 0ULL;
   }
}

/*******************************************************************************
 *
 * Method: get_pseudo_legal_moves(int square)
 *
 *******************************************************************************/
std::vector<int> BoardManager::get_pseudo_legal_moves(int square) const
{
  std::vector<int> ret(55);

  if (auto piece = square_to_piece(square)) {
    if (auto board = get_pseudo_legal_attack_bitboard(*piece, square)) {
      while (board) {
        int index = util::bits::get_lsb_index(board);
        util::bits::clear(index, board);
        ret.push_back(index);
      }
    }
  }

  return ret;
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
    Pos r_square = (Pos)(rand() % 64);

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
