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
}

bool BoardManager::is_attack_valid(const Move& m) const
{
  return false;
}

/*
 * Ensure the board is copied over
*/
BoardManager::BoardManager(const BoardManager& b) {
  // may need to copy attack and constant bbs over
  _state = b._state;
  for (int i = 0; i < 13; i++) {
    _board[i] = b._board[i];
  };
}

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
    pawn_attacks[0][i] = get_mask(0,i);
    pawn_attacks[1][i] = get_mask(1,i);
  }
}

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