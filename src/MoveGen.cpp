
#include "MoveGen.hxx"
#include <random>
#include <cassert>

using namespace chess;

/*******************************************************************************
 *
 * Method: add_move()
 *
 *******************************************************************************/
inline void MoveGen::add_move(std::vector<util::bits::HashedMove>& moves,
                              uint32_t source, uint32_t target,
                              uint32_t piece, uint32_t promotion,
                              uint32_t capture, uint32_t double_push,
                              uint32_t enpassant, uint32_t castling) const
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
                             std::vector<util::bits::HashedMove>& moves) const
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
                                   std::vector<util::bits::HashedMove>& moves) const
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
                                   std::vector<util::bits::HashedMove>& moves) const
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
                                        std::vector<util::bits::HashedMove>& moves) const
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
                                        std::vector<util::bits::HashedMove>& moves) const
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
                                            std::vector<util::bits::HashedMove>& moves) const
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
                                            std::vector<util::bits::HashedMove>& moves) const
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
                                    std::vector<util::bits::HashedMove>& moves) const
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
                                    std::vector<util::bits::HashedMove>& moves) const
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
                                  std::vector<util::bits::HashedMove>& moves) const
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
                                   std::vector<util::bits::HashedMove>& moves) const
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
                                  std::vector<util::bits::HashedMove>& moves) const
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
