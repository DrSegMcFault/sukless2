
#include "MoveGen.hxx"
#include <random>
#include <cassert>

using namespace chess;

/*******************************************************************************
 *
 * Method: MoveGen()
 *
 *******************************************************************************/
MoveGen::MoveGen()
{
  init_attack_tables();
  init_slider_attacks();
}

/*******************************************************************************
 *
 * Method: init_attack_tables()
 *
 *******************************************************************************/
constexpr void MoveGen::init_attack_tables()
{
  init_pawn_attacks();
  init_knight_attacks();
  init_king_attacks();
  init_bishop_masks();
  init_rook_masks();
}

/*******************************************************************************
 *
 * Method: add_move()
 *
 *******************************************************************************/
inline void MoveGen::add_move(std::vector<util::bits::HashedMove>& moves,
                              uint32_t source, uint32_t target,
                              uint32_t piece, uint32_t promotion,
                              uint32_t capture, uint32_t double_push,
                              uint32_t enpassant, uint32_t castling)
{
  moves.push_back({.source = source,
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
 * Method: generate_moves(Board&, State&, util::bits::HashedMove& moves)
 *
 *******************************************************************************/
void MoveGen::generate_moves(const Board& b,
                    const State& s,
                    std::vector<util::bits::HashedMove>& moves)
{
   switch (s.side_to_move) {
     case Color::white:
       generate_white_moves(b, s, moves);
       break;
     case Color::black:
       generate_black_moves(b, s, moves);
       break;
   }
}


/*******************************************************************************
 *
 * Method: init_slider_attacks()
 *
 *******************************************************************************/
void MoveGen::init_slider_attacks()
{
  const auto populate_attack_tables = [this](bool is_bishop) {
    // generates the ith permutation of the attack mask
    const auto ith_permutation =
      [](uint64_t index, uint8_t bits_in_mask, Bitboard attack_mask)
    {
      Bitboard occupancy = 0ULL;

      for (auto count : util::range(bits_in_mask))
      {
        uint8_t square = util::bits::get_lsb_index(attack_mask);
        clear_bit(square, attack_mask);

        if (is_set(count, index)) {
          set_bit(square, occupancy);
        }
      }

      return occupancy;
    };

    for (auto square : util::range(NoSquare)) {

      Bitboard attack_mask = (is_bishop ? bishop_masks[square] : rook_masks[square]);
      uint8_t bit_count = (is_bishop ? bishop_bits[square] : rook_bits[square]);
      uint64_t permutations = (1ULL << bit_count);

      for (auto index : util::range(permutations))
      {
        Bitboard occupancy = ith_permutation(index, bit_count, attack_mask);
        auto magic = (is_bishop ? bishop_magics[square] : rook_magics[square]);

        uint16_t magic_index = (occupancy * magic) >> (64ULL - bit_count);

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
constexpr void MoveGen::init_pawn_attacks()
{
  constexpr auto get_mask = [](Color side, uint8_t square) {
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

  for (auto i : util::range(NoSquare)) {
    pawn_attacks[white][i] = get_mask(white, i);
    pawn_attacks[black][i] = get_mask(black, i);
  }
}

/*******************************************************************************
 *
 * Method: init_knight_attacks()
 *
 *******************************************************************************/
constexpr void MoveGen::init_knight_attacks()
{
  for (auto square : util::range(NoSquare)) {
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
constexpr void MoveGen::init_king_attacks()
{
  for (auto square : util::range(NoSquare)) {
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
constexpr void MoveGen::init_bishop_masks()
{
  for (auto square : util::range(NoSquare)) {
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
constexpr void MoveGen::init_rook_masks()
{
  for (auto square : util::range(NoSquare)) {
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
 * Method: calc_bishop_attacks(uint8_t square, Bitboard occ)
 *
 *******************************************************************************/
constexpr Bitboard MoveGen::calc_bishop_attacks(uint8_t square, Bitboard occ) const
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
 * Method: calc_rook_attacks(uint8_t square, Bitboard occ)
 *
 *******************************************************************************/
constexpr Bitboard MoveGen::calc_rook_attacks(uint8_t square, Bitboard occ) const
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
 * Method: get_bishop_attacks(uint8_t square, Bitboard occ)
 *
 *******************************************************************************/
Bitboard MoveGen::get_bishop_attacks(uint8_t square, Bitboard occupancy) const
{
  occupancy &= bishop_masks[square];
  uint16_t index =
    (occupancy * bishop_magics[square]) >> (64ULL - bishop_bits[square]);
  return bishop_attacks[square][index];
}

/*******************************************************************************
 *
 * Method: get_rook_attacks(uint8_t square, Bitboard occ)
 *
 *******************************************************************************/
Bitboard MoveGen::get_rook_attacks(uint8_t square, Bitboard occupancy) const
{
  occupancy &= rook_masks[square];
  uint16_t index =
    (occupancy * rook_magics[square]) >> (64ULL - rook_bits[square]);
  return rook_attacks[square][index];
}

/*******************************************************************************
 *
 * Method: is_square_attacked(uint8_t square, Color side)
 * is the given square attacked by the given side
 *******************************************************************************/
bool MoveGen::is_square_attacked(uint8_t square,
                                 Color side,
                                 const Board& board) const
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
 * Method: generate_white_moves()
 *
 *******************************************************************************/
void MoveGen::generate_white_moves(const Board& b,
                                   const State& s,
                                   std::vector<util::bits::HashedMove>& moves)
{
  generate_white_pawn_moves(b, s, moves);
  generate_white_castling_moves(b, s, moves);
  generate_knight_moves(b, Color::white, moves);
  generate_bishop_moves(b, Color::white, moves);
  generate_rook_moves(b, Color::white, moves);
  generate_queen_moves(b, Color::white, moves);
  generate_king_moves(b, Color::white, moves);
}

/*******************************************************************************
 *
 * Method: generate_black_moves()
 *
 *******************************************************************************/
void MoveGen::generate_black_moves(const Board& b,
                                   const State& s,
                                   std::vector<util::bits::HashedMove>& moves)
{
  generate_black_pawn_moves(b, s, moves);
  generate_black_castling_moves(b, s, moves);
  generate_knight_moves(b, Color::black, moves);
  generate_bishop_moves(b, Color::black, moves);
  generate_rook_moves(b, Color::black, moves);
  generate_queen_moves(b, Color::black, moves);
  generate_king_moves(b, Color::black, moves);
}

/*******************************************************************************
 *
 * Method: generate_white_pawn_moves()
 *
 *******************************************************************************/
void MoveGen::generate_white_pawn_moves(const Board& board_, const State& state,
                                        std::vector<util::bits::HashedMove>& moves)
{
  Bitboard board = board_[w_pawn];
  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);

    // this is a white pawn push of 1 square
    target_square = source_square + 8;

    // if the target square is on board and not occupied
    if (target_square <= chess::H8 && !is_set(target_square, board_[All])) {
      // promotion
      if (source_square >= chess::A7 && source_square <= chess::H7) {

        add_move(moves, source_square, target_square, Piece::w_pawn, Piece::w_queen, 0,0,0,0);
        add_move(moves, source_square, target_square, Piece::w_pawn, Piece::w_rook,  0,0,0,0);
        add_move(moves, source_square, target_square, Piece::w_pawn, Piece::w_bishop,0,0,0,0);
        add_move(moves, source_square, target_square, Piece::w_pawn, Piece::w_knight,0,0,0,0);

      } else { // one move forward, no promotion
        add_move(moves, source_square, target_square, Piece::w_pawn, 0, 0,0,0,0);

        if ((source_square >= chess::A2 && source_square <= chess::H2) &&
            !is_set(target_square + 8, board_[All]))
        {
           // two square push
           add_move(moves, source_square, target_square + 8, Piece::w_pawn, 0, 0,1,0,0);
        }
      }
    }

    auto attacks = pawn_attacks[Color::white][source_square] & board_[b_all];

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      // promotion
      if (source_square >= chess::A7 && source_square <= chess::H7) {
        add_move(moves, source_square, target_square, Piece::w_pawn, Piece::w_queen, 1,0,0,0);
        add_move(moves, source_square, target_square, Piece::w_pawn, Piece::w_rook,  1,0,0,0);
        add_move(moves, source_square, target_square, Piece::w_pawn, Piece::w_bishop,1,0,0,0);
        add_move(moves, source_square, target_square, Piece::w_pawn, Piece::w_knight,1,0,0,0);

      } else { // capture, no promotion
        add_move(moves, source_square, target_square, Piece::w_pawn, 0, 1,0,0,0);
      }

      clear_bit(target_square, attacks);
    }

    if (state.en_passant_target != chess::NoSquare) {
      auto en_passant_attacks = pawn_attacks[Color::white][source_square] & (1ULL << state.en_passant_target);
      if (en_passant_attacks) {
        auto attack_square = util::bits::get_lsb_index(en_passant_attacks);
        add_move(moves, source_square, attack_square, Piece::w_pawn, 0,1,0,1,0);
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
void MoveGen::generate_black_pawn_moves(const Board& board_, const State& state,
                                        std::vector<util::bits::HashedMove>& moves)
{
   Bitboard board = board_[b_pawn];
   uint8_t source_square = 0;
   uint8_t target_square = 0;

   while (board) {
     source_square = util::bits::get_lsb_index(board);
     // this is a black pawn push of 1 square
     target_square = source_square - 8;

     // if the target square is on board and not occupied
     if (target_square >= chess::A1 && !is_set(target_square, board_[All])) {
       // promotion
       if (source_square >= chess::A2 && source_square <= chess::H2)
       {
         add_move(moves, source_square, target_square, Piece::b_pawn, Piece::b_queen, 0,0,0,0);
         add_move(moves, source_square, target_square, Piece::b_pawn, Piece::b_rook,  0,0,0,0);
         add_move(moves, source_square, target_square, Piece::b_pawn, Piece::b_bishop,0,0,0,0);
         add_move(moves, source_square, target_square, Piece::b_pawn, Piece::b_knight,0,0,0,0);

       } else { // one move forward, no promotion
         add_move(moves, source_square, target_square, Piece::b_pawn, 0, 0,0,0,0);

         if (source_square >= chess::A7 && source_square <= chess::H7 &&
             !is_set(target_square - 8, board_[All]))
         {
           // two square push
           add_move(moves, source_square, target_square - 8, Piece::b_pawn, 0, 0,1,0,0);
         }
       }
     }

     auto attacks = pawn_attacks[Color::black][source_square] & board_[w_all];

     while (attacks) {
       target_square = util::bits::get_lsb_index(attacks);

       // promotion
       if (source_square >= chess::A2 && source_square <= chess::H2) {
         add_move(moves, source_square, target_square, Piece::b_pawn, Piece::b_queen, 1,0,0,0);
         add_move(moves, source_square, target_square, Piece::b_pawn, Piece::b_rook,  1,0,0,0);
         add_move(moves, source_square, target_square, Piece::b_pawn, Piece::b_bishop,1,0,0,0);
         add_move(moves, source_square, target_square, Piece::b_pawn, Piece::b_knight,1,0,0,0);
       } else { // capture, no promotion
         add_move(moves, source_square, target_square, Piece::b_pawn, 0, 1,0,0,0);
       }
       clear_bit(target_square, attacks);
     }

     if (state.en_passant_target != chess::NoSquare) {
       auto en_passant_attacks = pawn_attacks[Color::black][source_square] & (1ULL << state.en_passant_target);
       if (en_passant_attacks) {
         auto attack_square = util::bits::get_lsb_index(en_passant_attacks);
         add_move(moves, source_square, attack_square, Piece::b_pawn, 0,1,0,1,0);
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
void MoveGen::generate_white_castling_moves(const Board& board_, const State& state,
                                            std::vector<util::bits::HashedMove>& moves)
{
  using namespace util;
  if (state.castling_rights & toul(CastlingRights::WhiteKingSide))
  {
    if (!is_set(chess::F1, board_[All]) &&
        !is_set(chess::G1, board_[All]))
    {
      if (!is_square_attacked(chess::E1, Color::black, board_) &&
          !is_square_attacked(chess::F1, Color::black, board_))
      {
        add_move(moves, chess::E1, chess::G1, Piece::w_king, 0,0,0,0,1);
       }
     }
   }

   if (state.castling_rights & toul(CastlingRights::WhiteQueenSide))
   {
     if (!(is_set(chess::D1, board_[All])) &&
         !(is_set(chess::C1, board_[All])) &&
         !(is_set(chess::B1, board_[All])))
     {
       if (!is_square_attacked(chess::E1, Color::black, board_) &&
           !is_square_attacked(chess::D1, Color::black, board_))
       {
         add_move(moves, chess::E1, chess::C1, Piece::w_king, 0,0,0,0,1);
       }
     }
   }
}

/*******************************************************************************
 *
 * Method: generate_black_castling_moves()
 *
 *******************************************************************************/
void MoveGen::generate_black_castling_moves(const Board& board_, const State& state,
                                            std::vector<util::bits::HashedMove>& moves)
{
   using namespace util;
   if (state.castling_rights & toul(CastlingRights::BlackKingSide))
   {
     if (!is_set(chess::F8, board_[All]) &&
         !is_set(chess::G8, board_[All]))
     {
       if (!is_square_attacked(chess::E8, Color::white, board_) &&
           !is_square_attacked(chess::F8, Color::white, board_))
       {
         add_move(moves, chess::E8, chess::G8, Piece::b_king, 0,0,0,0,1);
       }
     }
   }

   if (state.castling_rights & toul(CastlingRights::BlackQueenSide))
   {
     if (!is_set(chess::D8, board_[All]) &&
         !is_set(chess::C8, board_[All]) &&
         !is_set(chess::B8, board_[All]))
     {
       if (!is_square_attacked(chess::E8, Color::white, board_) &&
           !is_square_attacked(chess::D8, Color::white, board_))
       {
         add_move(moves, chess::E8, chess::C8, Piece::b_king, 0,0,0,0,1);
       }
     }
   }
}

/*******************************************************************************
 *
 * Method: generate_knight_moves(Color side_to_move)
 *
 *******************************************************************************/
void MoveGen::generate_knight_moves(const Board& board_, Color side_to_move,
                                    std::vector<util::bits::HashedMove>& moves)
{
  Bitboard board = board_[(side_to_move == Color::white) ? w_knight : b_knight];
  Bitboard attacks = 0ULL;
  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = knight_attacks[source_square] & ~(board_[(side_to_move == Color::white) ? w_all : b_all]);

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

       // if the move is not a capture
       if (!is_set(target_square, board_[(side_to_move == Color::white) ? b_all : w_all])) {
         add_move(moves, source_square, target_square, (side_to_move == Color::white) ? w_knight : b_knight, 0, 0,0,0,0);
       } else {
         add_move(moves, source_square, target_square, (side_to_move == Color::white) ? w_knight : b_knight, 0, 1,0,0,0);
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
void MoveGen::generate_bishop_moves(const Board& board_, Color side_to_move,
                                    std::vector<util::bits::HashedMove>& moves) 
{
  Bitboard board = board_[(side_to_move == Color::white) ? w_bishop : b_bishop];
  Bitboard attacks = 0ULL;
  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = get_bishop_attacks(source_square, board_[All]) & ~(board_[(side_to_move == Color::white) ? w_all : b_all]);

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      // if the move is not a capture 
      if (!is_set(target_square, board_[side_to_move == Color::white ? b_all : w_all])) {
        add_move(moves, source_square, target_square, (side_to_move == Color::white) ? w_bishop : b_bishop, 0, 0,0,0,0);
      } else { 
        add_move(moves, source_square, target_square, (side_to_move == Color::white) ? w_bishop : b_bishop, 0, 1,0,0,0);
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
void MoveGen::generate_rook_moves(const Board& board_, Color side_to_move,
                                  std::vector<util::bits::HashedMove>& moves)
{
  Bitboard board = board_[(side_to_move == Color::white) ? w_rook : b_rook];
  Bitboard attacks = 0ULL;
  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = get_rook_attacks(source_square, board_[All]) & ~(board_[(side_to_move == Color::white) ? w_all : b_all]);

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      // if the move is not a capture
      if (!is_set(target_square, board_[side_to_move == Color::white ? b_all : w_all])) {
        add_move(moves, source_square, target_square, (side_to_move == Color::white) ? w_rook : b_rook, 0, 0,0,0,0);
      } else { 
        add_move(moves, source_square, target_square, (side_to_move == Color::white) ? w_rook : b_rook, 0, 1,0,0,0);
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
void MoveGen::generate_queen_moves(const Board& board_, Color side_to_move,
                                   std::vector<util::bits::HashedMove>& moves) 
{
  Bitboard board = board_[(side_to_move == Color::white) ? w_queen : b_queen];
  Bitboard attacks = 0ULL;
  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = get_queen_attacks(source_square, board_[All]) & ~(board_[(side_to_move == Color::white) ? w_all : b_all]);

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      // if the move is not a capture 
      if (!is_set(target_square, board_[side_to_move == Color::white ? b_all : w_all])) {
        add_move(moves, source_square, target_square, (side_to_move == Color::white) ? w_queen : b_queen, 0, 0,0,0,0);
      } else { 
        add_move(moves, source_square, target_square, (side_to_move == Color::white) ? w_queen : b_queen, 0, 1,0,0,0);
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
void MoveGen::generate_king_moves(const Board& board_, Color side_to_move,
                                  std::vector<util::bits::HashedMove>& moves)
{
  Bitboard board = board_[(side_to_move == Color::white) ? w_king : b_king];
  Bitboard attacks = 0ULL;
  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = king_attacks[source_square] & (side_to_move == Color::white ? ~(board_[w_all]) : ~(board_[b_all]));

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      // if the move is not a capture 
      if (!is_set(target_square, board_[side_to_move == Color::white ? b_all : w_all])) {
        add_move(moves, source_square, target_square, (side_to_move == Color::white) ? w_king : b_king, 0, 0,0,0,0);
      } else { 
        add_move(moves, source_square, target_square, (side_to_move == Color::white) ? w_king : b_king, 0, 1,0,0,0);
      }

      clear_bit(target_square, attacks);
    }
    clear_bit(source_square, board);
  }
}
/*******************************************************************************
 *
 * Method: test_attack_lookup()
 *
 *******************************************************************************/
void MoveGen::test_attack_lookup()
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
