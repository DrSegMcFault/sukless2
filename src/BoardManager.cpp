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
  using enum Piece;

  init_from_fen("7p/p7/2k5/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  init_attack_tables();
  
  // test as best as possible that the
  // lookup tables are working correctly
  auto test_attack_lookup = [this]() {
    std::random_device rd;
    std::mt19937 gen(rd());

    for (auto i = 0; i < 100000000; i ++) {
      std::uniform_int_distribution<unsigned long long> dis(
        std::numeric_limits<std::uint64_t>::min(),
        std::numeric_limits<std::uint64_t>::max());

      Bitboard occ = {dis(gen)}; 
      Pos r_square = (Pos)(rand() % 64);

      Bitboard bishop_actual = get_bishop_attacks(r_square, occ);
      Bitboard bishop_calculated = calc_bishop_attacks(r_square, occ);

      Bitboard rook_actual = get_rook_attacks(r_square, occ);
      Bitboard rook_calculated = calc_rook_attacks(r_square, occ);
      Bitboard queen_actual = get_queen_attacks(r_square, occ);
      Bitboard queen_calculated = (calc_rook_attacks(r_square, occ)) | calc_bishop_attacks(r_square, occ);

      // bishop
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
      // rook
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
  };
  test_attack_lookup();
}

/*******************************************************************************
 *
 * Method: is_attack_valid(const Move& m)
 *
 *******************************************************************************/
bool BoardManager::is_attack_valid(const Move& /*m*/) const
{
  return false;
}

/*******************************************************************************
 *
 * Method: init_pawn_attacks()
 *
 *******************************************************************************/
constexpr void BoardManager::init_pawn_attacks()
{
  constexpr auto get_mask = [](int side, int square) {
    Bitboard attacks {0ULL};
    Bitboard b {0ULL};

    // set the bit corresponding to square
    b |= (1ULL << square);

    // white
    if (side == 0) {
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
    pawn_attacks[Color::white][i] = get_mask(0,i);
    pawn_attacks[Color::black][i] = get_mask(1,i);
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
  init_sliderook_actualttacks();
}

/*******************************************************************************
 *
 * Method: is_square_attacked(int square, Color side)
 *
 *******************************************************************************/
bool BoardManager::is_square_attacked(int /*square*/, Color side) const
{
  // 'and' the relavant boards together and 'and' it with the sqaure
  if (side == Color::white) {
    return false;
  } else {
    return false; 
  }
}

/*******************************************************************************
 *
 * Method: get_bishop_attacks(int square, Bitboard occ)
 *
 *******************************************************************************/
Bitboard BoardManager::get_bishop_attacks(Pos square, Bitboard occupancy)
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
Bitboard BoardManager::get_rook_attacks(Pos square, Bitboard occupancy)
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
Bitboard BoardManager::get_queen_attacks(Pos square, Bitboard occ)
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
 * Method: calc_bishop_attacks(int square, Bitboard occ)
 *
 *******************************************************************************/
constexpr Bitboard BoardManager::calc_bishop_attacks(int square, Bitboard occ)
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
 * Method: init_sliderook_actualttacks()
 *
 *******************************************************************************/
constexpr void BoardManager::init_sliderook_actualttacks()
{
  auto set_occupancy = 
    [](uint64_t index, int bits_in_mask, Bitboard attack_mask)
  {
    Bitboard occupancy = 0ULL;

    for (int count = 0; count < bits_in_mask; count++)
    {
        int square = get_lsb_index(attack_mask);
        clear(square, attack_mask);

        if (index & (1ULL << count))
        {
          occupancy |= (1ULL << square);
        }
    }

    return occupancy;
  };

  const auto populate_attack_tables = [this,set_occupancy](bool is_bishop) {

    for (int square = 0; square < 64; square++) {

      Bitboard attack_mask = (is_bishop ? bishop_masks[square] : rook_masks[square]);
      int bit_count = (is_bishop ? bishop_bits[square] : rook_bits[square]);
      int occupancy_indicies = (1ULL << bit_count);

      for (int index = 0; index < occupancy_indicies; index++)
      {
        Bitboard occupancy = set_occupancy(index, bit_count, attack_mask);
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
 * Method: calc_rook_attacks(int square, Bitboard occ)
 *
 *******************************************************************************/
constexpr Bitboard BoardManager::calc_rook_attacks(int square, Bitboard occ)
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
 * Method: init_from_fen(Bitboard b)
 *
 *******************************************************************************/
void BoardManager::init_from_fen(const std::string &fen)
{
  // populate board occupancies and game state
  // using a FEN string
  for (auto& b : _board) {
    b = 0ULL;
  }

  auto piece_from_char = [](char c) {
    using enum Piece;
    switch (c) {
      case 'p': return w_pawn;
      case 'n': return w_knight;
      case 'b': return w_bishop;
      case 'r': return w_rook;
      case 'q': return w_queen;
      case 'k': return w_king;
      case 'P': return b_pawn;
      case 'N': return b_knight;
      case 'B': return b_bishop;
      case 'R': return b_rook;
      case 'Q': return b_queen;
      case 'K': return b_king;
      default: throw std::runtime_error("invalid FEN string");
    }
  };

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
    auto piece = piece_from_char(c);
    set(square, _board[piece]);
    square++;
  }
  // set the all pieces board
  _board[All] = _board[w_pawn] |
                _board[w_knight] |
                _board[w_bishop] |
                _board[w_rook] |
                _board[w_queen] |
                _board[w_king] |
                _board[b_pawn] |
                _board[b_knight] |
                _board[b_bishop] |
                _board[b_rook] |
                _board[b_queen] |
                _board[b_king];
  print();
}
