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

  _board[w_pawn]   = 65280ULL;
  _board[w_knight] = 66ULL;
  _board[w_bishop] = 65280ULL;
  _board[w_rook]   = 129ULL;
  _board[w_queen]  = 8ULL;
  _board[w_king]   = 16ULL;

  _board[b_pawn]   = 71776119061217280ULL;
  _board[b_knight] = 4755801206503243776ULL;
  _board[b_bishop] = 71776119061217280ULL;
  _board[b_rook]   = 9295429630892703744ULL;
  _board[b_queen]  = 576460752303423488ULL;
  _board[b_king]   = 1152921504606846976ULL;

  _board[All]      = 18446462598732906495ULL;

  init_attack_tables();
  
  // test as best as possible that the
  // lookup tables are working correctly
  
  std::random_device rd;
  std::mt19937 gen(rd());

  for (auto i = 0; i < 100000000; i ++) {

    std::uniform_int_distribution<unsigned long long> dis(
      std::numeric_limits<std::uint64_t>::min(),
      std::numeric_limits<std::uint64_t>::max());

    BitBoard occ = {dis(gen)}; 
    Pos r_square = (Pos)(rand() % 64);

    BitBoard b_a = get_bishop_attacks(r_square, occ);
    BitBoard b_c = get_bishop_otf(r_square, occ.get());

    BitBoard r_a = get_rook_attacks(r_square, occ);
    BitBoard r_c = get_rook_otf(r_square, occ.get());

    // bishop
    if (b_a != b_c) {
      std::cout << "attack fetch falied for occ on square: " << r_square <<"\n";
      occ.print();
      std::cout << "expected:\n";
      b_c.print();
      std::cout << "actual:\n";
      b_a.print();
      std::cout << "mask: \n";
      bishop_masks[r_square].print();
      assert(false);
    }
    // rook
    if (r_a != r_c) {
      std::cout << "attack fetch falied for occ on square: " << r_square <<"\n";
      occ.print();
      std::cout << "expected:\n";
      r_c.print();
      std::cout << "actual:\n";
      r_a.print();
      std::cout << "mask: \n";
      rook_masks[r_square].print();
      assert(false);
    }
  }
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
  const auto get_mask = [](int side, int square) {
    uint64_t attacks {0ULL};
    uint64_t b {0ULL};

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
    return BitBoard(attacks);
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
  const auto get_mask = [](int square) {
    uint64_t attacks {0ULL};
    uint64_t b {0ULL};

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

    return BitBoard(attacks);
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
  const auto get_mask = [](int square) {
    uint64_t attacks {0ULL};
    uint64_t b {0ULL};

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

    return BitBoard(attacks);
  };

  for (int i = 0; i < 64; i++) {
    king_attacks[i] = get_mask(i);
  }
}

/*******************************************************************************
 *
 * Method: init_bishop_rel_occ()
 *
 *******************************************************************************/
constexpr void BoardManager::init_bishop_rel_occ()
{
  const auto get_mask = [] (int square) {
    uint64_t attacks {0ULL};

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

    return BitBoard(attacks);
  };

  for (int i = 0; i < 64; i++) {
    bishop_masks[i] = get_mask(i);
  }
}

/*******************************************************************************
 *
 * Method: init_rook_rel_occ()
 *
 *******************************************************************************/
constexpr void BoardManager::init_rook_rel_occ()
{
  const auto get_mask = [] (int square) {
    uint64_t attacks {0ULL};

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

    return BitBoard(attacks);
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
  init_bishop_rel_occ();
  init_rook_rel_occ();
  init_slider_attacks();
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
 * Method: get_bishop_attacks(int square, BitBoard occ)
 *
 *******************************************************************************/
BitBoard BoardManager::get_bishop_attacks(Pos square, const BitBoard occ)
{
  auto occupancy = occ.get();

  occupancy &= bishop_masks[square].get();
  int index = (occupancy * bishop_magics[square]) >> (64ULL - bishop_relevant_bits[square]);
    
  return bishop_attacks[square][index];
}

/*******************************************************************************
 *
 * Method: get_rook_attacks(int square, BitBoard occ)
 *
 *******************************************************************************/
BitBoard BoardManager::get_rook_attacks(Pos square, const BitBoard occ)
{
  auto occupancy = occ.get();

  occupancy &= rook_masks[square].get();
  int index = (occupancy * rook_magics[square]) >> (64ULL - rook_relevant_bits[square]);
  return rook_attacks[square][index];;
}

/*******************************************************************************
 *
 * Method: get_rook_attacks(int square, BitBoard occ)
 *
 *******************************************************************************/
BitBoard BoardManager::get_queen_attacks(Pos square, const BitBoard occ)
{
  return (get_bishop_attacks(square, occ) | get_rook_attacks(square, occ));
}

/*******************************************************************************
 *
 * Method: get_bishop_otf(int square, uint64_t occ)
 *
 *******************************************************************************/
constexpr BitBoard BoardManager::get_bishop_otf(int square, uint64_t occ)
{
  uint64_t attacks {0ULL};

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
  return BitBoard(attacks);
}

/*******************************************************************************
 *
 * Method: init_slider_attacks()
 *
 *******************************************************************************/
constexpr void BoardManager::init_slider_attacks()
{
  auto set_occupancy = [this](uint64_t index, int bits_in_mask, BitBoard attack_mask)
  {
    uint64_t occupancy = 0ULL;

    for (int count = 0; count < bits_in_mask; count++)
    {
        int square = *get_lsb_index(attack_mask);
        attack_mask.clear(square);

        if (index & (1ULL << count))
        {
          occupancy |= (1ULL << square);
        }
    }

    return occupancy;
  };

  const auto populate_attack_tables = [this,set_occupancy](bool is_bishop) {

    for (int square = 0; square < 64; square++)
    {
      BitBoard attack_mask = (is_bishop ? bishop_masks[square] : rook_masks[square]);
      int relevant_bits_count = (is_bishop ? bishop_relevant_bits[square] : rook_relevant_bits[square]);
      int occupancy_indicies = (1ULL << relevant_bits_count);

      for (int index = 0; index < occupancy_indicies; index++)
      {
        uint64_t occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
        auto magic = (is_bishop ? bishop_magics[square] : rook_magics[square]);

        int magic_index = (occupancy * magic) >> (64 - relevant_bits_count);

        if (is_bishop) {
          bishop_attacks[square][magic_index] = get_bishop_otf(square, occupancy);
        }
        else {
          rook_attacks[square][magic_index] = get_rook_otf(square, occupancy);
        }
      }
    }
  };

  populate_attack_tables(true); // bishop
  populate_attack_tables(false); // rook
}

/*******************************************************************************
 *
 * Method: get_rook_otf(int square, uint64_t occ)
 *
 *******************************************************************************/
constexpr BitBoard BoardManager::get_rook_otf(int square, uint64_t occ)
{
  uint64_t attacks {0ULL};

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
  return BitBoard(attacks);
}

/*******************************************************************************
 *
 * Method: get_lsb_index(BitBoard b)
 *
 *******************************************************************************/
std::optional<int> BoardManager::get_lsb_index(BitBoard b)
{
  std::optional<int> ret;
  uint64_t bb = b.get();
  const auto count = [](uint64_t b){
    uint32_t count = 0;
    while (b) {
      count++;
      b &= b - 1;
    }
    return count;
  };

  if (bb) {
    ret.emplace(count((bb & -bb) - 1));
  }
  return ret;
}
