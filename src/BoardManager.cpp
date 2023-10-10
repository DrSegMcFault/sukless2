#include "BoardManager.hxx"
/*
 * Set the board to the initial position
*/
using namespace chess;
BoardManager::BoardManager() {
  using enum Piece;

  _board[c(w_pawn)]   = 65280ULL;
  _board[c(w_knight)] = 66ULL;
  _board[c(w_bishop)] = 65280ULL;
  _board[c(w_rook)]   = 129ULL;
  _board[c(w_queen)]  = 8ULL;
  _board[c(w_king)]   = 16ULL;

  _board[c(b_pawn)]   = 71776119061217280ULL;
  _board[c(b_knight)] = 4755801206503243776ULL;
  _board[c(b_bishop)] = 71776119061217280ULL;
  _board[c(b_rook)]   = 9295429630892703744ULL;
  _board[c(b_queen)]  = 576460752303423488ULL;
  _board[c(b_king)]   = 1152921504606846976ULL;

  _board[c(All)]      = 18446462598732906495ULL;

  init_pawn_attacks();
  init_knight_attacks();
  init_king_attacks();
  init_bishop_rel_occ();
  init_rook_rel_occ();
}

/*******************************************************
* returns if the attack bitboard for the piece is set 
* or not
********************************************************/
bool BoardManager::is_attack_valid(const Move& m) const
{
  return _board[c(m.piece)].is_set(m.to);
}

/*******************************************************
* Copy constructor 
* 
********************************************************/
BoardManager::BoardManager(const BoardManager& b) {
  // may need to copy attack and constant bbs over
  _state = b._state;
  for (int i = 0; i < 13; i++) {
    _board[i] = b._board[i];
  };
}

/*******************************************************
* Populate the pawn attack bitboards 
* 
********************************************************/
constexpr void BoardManager::init_pawn_attacks()
{
  const auto get_mask = [this](int side, int square) {
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
    pawn_attacks[c(chess::Color::white)][i] = get_mask(0,i);
    pawn_attacks[c(chess::Color::black)][i] = get_mask(1,i);
  }
}

/*******************************************************
* Populate the knight attack bitboards 
* 
********************************************************/
constexpr void BoardManager::init_knight_attacks()
{
  const auto get_mask = [this](int square) {
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

/*******************************************************
* Populate the king attack bitboards 
* 
********************************************************/
constexpr void BoardManager::init_king_attacks()
{
  const auto get_mask = [this](int square) {
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

/*******************************************************
* Populate the bishops relavent occupancy for each square 
* 
********************************************************/
constexpr void BoardManager::init_bishop_rel_occ()
{
  const auto get_mask = [this] (int square) {
    uint64_t attacks {0ULL};

    int r = 0;
    int f = 0;
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1, f = tf + 1; r <=6 && f<=6; r++, f++) {
      attacks |= (1ULL << (r * 8 + f));
    }
    for (r = tr - 1, f = tf + 1; r >= 1 && f<=6; r--, f++) {
      attacks |= (1ULL << (r * 8 + f));
    }
    for (r = tr + 1, f = tf - 1; r <=6 && f>=1; r++, f--) {
      attacks |= (1ULL << (r * 8 + f));
    }
    for (r = tr - 1, f = tf - 1; r >= 1 && f>=1; r--, f--) {
      attacks |= (1ULL << (r * 8 + f));
    }

    return BitBoard(attacks);
  };

  for (int i = 0; i < 64; i++) {
    relavent_bishop_occ[i] = get_mask(i);
  }
}

/*******************************************************
* Populate the rooks relavant occupancy for each square 
* 
********************************************************/
constexpr void BoardManager::init_rook_rel_occ()
{  
  const auto get_mask = [this] (int square) {
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
    relavent_rook_occ[i] = get_mask(i);
  }
}

/*******************************************************
* Check if the given square is attacked by the given
* side 
********************************************************/
bool BoardManager::is_square_attacked(int square, Color side) const
{
  // 'and' the relavant boards together and 'and' it with the sqaure
  if (side == Color::white) {
    return false;
  } else {
    return false; 
  } 
}
