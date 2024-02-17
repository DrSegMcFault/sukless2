
#include "MoveGenerator.hxx"
#include <random>
#include <cassert>

using namespace chess;

/*******************************************************************************
 *
 * Method: addMove()
 *
 *******************************************************************************/
inline void MoveGenerator::addMove(std::vector<util::bits::HashedMove>& moves,
                             uint32_t source, uint32_t target,
                             uint32_t piece, uint32_t promotion,
                             uint32_t capture, uint32_t double_push,
                             uint32_t enpassant, uint32_t castling) const
{
  moves.push_back({.source = source,
                   .target = target,
                   .piece = piece,
                   .promoted = promotion,
                   .capture = capture,
                   .double_push = double_push,
                   .enpassant = enpassant,
                   .castling = castling});
}

/*******************************************************************************
 *
 * Method: generateMoves(Board&, State&, util::bits::HashedMove& moves)
 *
 *******************************************************************************/
void MoveGenerator::generateMoves(const Board& b,
                            const State& s,
                            std::vector<util::bits::HashedMove>& moves) const
{
   switch (s.side_to_move) {
     case White:
       generateWhitePawnMoves(b, s, moves);
       break;
     case Black:
       generateBlackPawnMoves(b, s, moves);
       break;
   }
   generateCastlingMoves(b, s, moves);
   generateKnightMoves(b, s.side_to_move, moves);
   generateBishopMoves(b, s.side_to_move, moves);
   generateRookMoves(b, s.side_to_move, moves);
   generateQueenMoves(b, s.side_to_move, moves);
   generateKingMoves(b, s.side_to_move, moves);
}

/*******************************************************************************
 *
 * Method: getBishopAttacks(uint8_t square, Bitboard occ)
 *
 *******************************************************************************/
Bitboard MoveGenerator::getBishopAttacks(uint8_t square, Bitboard occupancy) const
{
  occupancy &= bishop_masks[square];
  uint16_t index =
    (occupancy * bishop_magics[square]) >> (64ULL - bishop_bits[square]);
  return bishop_attacks[square][index];
}

/*******************************************************************************
 *
 * Method: getRookAttacks(uint8_t square, Bitboard occ)
 *
 *******************************************************************************/
Bitboard MoveGenerator::getRookAttacks(uint8_t square, Bitboard occupancy) const
{
  occupancy &= rook_masks[square];
  uint16_t index =
    (occupancy * rook_magics[square]) >> (64ULL - rook_bits[square]);
  return rook_attacks[square][index];
}

/*******************************************************************************
 *
 * Method: isSquareAttacked(uint8_t square, Color side)
 * is the given square attacked by the given side
 *******************************************************************************/
bool MoveGenerator::isSquareAttacked(uint8_t square,
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

  if ((side == White) && (pawn_attacks[Black][square] & board[WhitePawn]) ) {
    return true;
  }

  if ((side == Black) && (pawn_attacks[White][square] & board[BlackPawn]) ) {
    return true;
  }

  if (knight_attacks[square] & board[(side == White) ? WhiteKnight : BlackKnight]) {
    return true;
  }

  if (getBishopAttacks(square, board[All]) &
      ( (side == White) ? board[WhiteBishop] : board[BlackBishop] ) )
  {
    return true;
  }

  if (getRookAttacks(square, board[All]) &
      ( (side == White) ? board[WhiteRook] : board[BlackRook] ) )
  {
    return true;
  }

  if (getQueenAttacks(square, board[All]) &
      ( (side == White) ? board[WhiteQueen] : board[BlackQueen] ) )
  {
    return true;
  }

  if (king_attacks[square] & board[(side == White) ? WhiteKing : BlackKing] )
  {
    return true;
  }

  return false;
}

/*******************************************************************************
 *
 * Method: generateWhitePawnMoves()
 *
 *******************************************************************************/
void MoveGenerator::generateWhitePawnMoves(const Board& board_, const State& state,
                                     std::vector<util::bits::HashedMove>& moves) const
{
  Bitboard board = board_[WhitePawn];
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

        addMove(moves, source_square, target_square, WhitePawn, WhiteQueen, 0,0,0,0);
        addMove(moves, source_square, target_square, WhitePawn, WhiteRook,  0,0,0,0);
        addMove(moves, source_square, target_square, WhitePawn, WhiteBishop,0,0,0,0);
        addMove(moves, source_square, target_square, WhitePawn, WhiteKnight,0,0,0,0);

      } else { // one move forward, no promotion
          addMove(moves, source_square, target_square, WhitePawn, NoPiece, 0,0,0,0);

        if ((source_square >= chess::A2 && source_square <= chess::H2) &&
            !is_set(target_square + 8, board_[All]))
        {
          // two square push
          addMove(moves, source_square, target_square + 8, WhitePawn, NoPiece, 0,1,0,0);
        }
      }
    }

    auto attacks = pawn_attacks[White][source_square] & board_[BlackAll];

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      // capturing promotion
      if (source_square >= chess::A7 && source_square <= chess::H7) {
        addMove(moves, source_square, target_square, WhitePawn, WhiteQueen, 1,0,0,0);
        addMove(moves, source_square, target_square, WhitePawn, WhiteRook,  1,0,0,0);
        addMove(moves, source_square, target_square, WhitePawn, WhiteBishop,1,0,0,0);
        addMove(moves, source_square, target_square, WhitePawn, WhiteKnight,1,0,0,0);

      } else { // capture, no promotion
          addMove(moves, source_square, target_square, WhitePawn, NoPiece, 1,0,0,0);
      }

      clear_bit(target_square, attacks);
    }

    if (state.en_passant_target != chess::NoSquare) {
      auto en_passant_attacks = pawn_attacks[White][source_square] & (1ULL << state.en_passant_target);
      if (en_passant_attacks) {
        auto attack_square = util::bits::get_lsb_index(en_passant_attacks);
          addMove(moves, source_square, attack_square, WhitePawn, NoPiece, 1,0,1,0);
      }
    }

    clear_bit(source_square, board); 
  }
}

/*******************************************************************************
 *
 * Method: generateBlackPawnMoves()
 *
 *******************************************************************************/
void MoveGenerator::generateBlackPawnMoves(const Board& board_, const State& state,
                                        std::vector<util::bits::HashedMove>& moves) const
{
   using enum Piece;

   Bitboard board = board_[BlackPawn];
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
         addMove(moves, source_square, target_square, BlackPawn, BlackQueen, 0,0,0,0);
         addMove(moves, source_square, target_square, BlackPawn, BlackRook,  0,0,0,0);
         addMove(moves, source_square, target_square, BlackPawn, BlackBishop,0,0,0,0);
         addMove(moves, source_square, target_square, BlackPawn, BlackKnight,0,0,0,0);

       } else { // one move forward, no promotion
           addMove(moves, source_square, target_square, BlackPawn, NoPiece, 0,0,0,0);

         if (source_square >= chess::A7 && source_square <= chess::H7 &&
             !is_set(target_square - 8, board_[All]))
         {
           // two square push
           addMove(moves, source_square, target_square - 8, BlackPawn, NoPiece, 0,1,0,0);
         }
       }
     }

     auto attacks = pawn_attacks[Black][source_square] & board_[WhiteAll];

     while (attacks) {
       target_square = util::bits::get_lsb_index(attacks);

       // promotion
       if (source_square >= chess::A2 && source_square <= chess::H2) {
         addMove(moves, source_square, target_square, BlackPawn, BlackQueen, 1,0,0,0);
         addMove(moves, source_square, target_square, BlackPawn, BlackRook,  1,0,0,0);
         addMove(moves, source_square, target_square, BlackPawn, BlackBishop,1,0,0,0);
         addMove(moves, source_square, target_square, BlackPawn, BlackKnight,1,0,0,0);
       } else { // capture, no promotion
         addMove(moves, source_square, target_square, BlackPawn, NoPiece, 1,0,0,0);
       }
       clear_bit(target_square, attacks);
     }

     if (state.en_passant_target != chess::NoSquare) {
       auto en_passant_attacks = pawn_attacks[Black][source_square] & (1ULL << state.en_passant_target);
       if (en_passant_attacks) {
         auto attack_square = util::bits::get_lsb_index(en_passant_attacks);
         addMove(moves, source_square, attack_square, BlackPawn, NoPiece, 1,0,1,0);
       }
     }
     clear_bit(source_square, board);
  }
}

/*******************************************************************************
 *
 * Method: generateCastlingMoves()
 *
 *******************************************************************************/
void MoveGenerator::generateCastlingMoves(const Board& board_, const State& state,
                                    std::vector<util::bits::HashedMove>& moves) const
{
   using namespace util;
   using enum Piece;

   switch (state.side_to_move) {
     case White:
     {
       if (state.castling_rights & toul(CastlingRights::WhiteKingSide))
       {
         if (!is_set(chess::F1, board_[All]) &&
             !is_set(chess::G1, board_[All]))
         {
           if (!isSquareAttacked(chess::E1, Black, board_) &&
               !isSquareAttacked(chess::F1, Black, board_))
           {
             addMove(moves, chess::E1, chess::G1, WhiteKing, NoPiece, 0,0,0,1);
           }
         }
       }

       if (state.castling_rights & toul(CastlingRights::WhiteQueenSide))
       {
         if (!(is_set(chess::D1, board_[All])) &&
             !(is_set(chess::C1, board_[All])) &&
             !(is_set(chess::B1, board_[All])))
          {
            if (!isSquareAttacked(chess::E1, Black, board_) &&
                !isSquareAttacked(chess::D1, Black, board_))
            {
              addMove(moves, chess::E1, chess::C1, WhiteKing, NoPiece, 0,0,0,1);
            }
          }
       }
       break;
     }

     case Black:
     {
       if (state.castling_rights & toul(CastlingRights::BlackKingSide))
       {
         if (!is_set(chess::F8, board_[All]) &&
             !is_set(chess::G8, board_[All]))
         {
           if (!isSquareAttacked(chess::E8, White, board_) &&
               !isSquareAttacked(chess::F8, White, board_))
           {
             addMove(moves, chess::E8, chess::G8, BlackKing, NoPiece, 0,0,0,1);
           }
         }
       }

       if (state.castling_rights & toul(CastlingRights::BlackQueenSide))
       {
         if (!is_set(chess::D8, board_[All]) &&
             !is_set(chess::C8, board_[All]) &&
             !is_set(chess::B8, board_[All]))
         {
           if (!isSquareAttacked(chess::E8, White, board_) &&
               !isSquareAttacked(chess::D8, White, board_))
           {
             addMove(moves, chess::E8, chess::C8, BlackKing, NoPiece, 0,0,0,1);
           }
         }
       }

       break;
     }
   }
}

/*******************************************************************************
 *
 * Method: generateKnightMoves(Color side_to_move)
 *
 *******************************************************************************/
void MoveGenerator::generateKnightMoves(const Board& board_, Color side_to_move,
                                    std::vector<util::bits::HashedMove>& moves) const
{
  using enum Piece;
  auto piece_t = (side_to_move == White) ? WhiteKnight : BlackKnight;

  Bitboard board = board_[piece_t];
  Bitboard attacks = 0ULL;

  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = knight_attacks[source_square] & ~(board_[(side_to_move == White) ? WhiteAll : BlackAll]);

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      auto is_capture =
          static_cast<bool>(is_set(target_square, board_[side_to_move == White ? BlackAll : WhiteAll]));

      addMove(moves,
              source_square,
              target_square,
              piece_t, NoPiece, is_capture, 0, 0, 0);

      clear_bit(target_square, attacks);
    }

    clear_bit(source_square, board);
  }
}

/*******************************************************************************
 *
 * Method: generateBishopMoves(Color side_to_move)
 *
 *******************************************************************************/
void MoveGenerator::generateBishopMoves(const Board& board_, Color side_to_move,
                                  std::vector<util::bits::HashedMove>& moves) const
{
  auto piece_t = (side_to_move == White) ? WhiteBishop : BlackBishop;

  Bitboard board = board_[piece_t];
  Bitboard attacks = 0ULL;

  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = getBishopAttacks(source_square, board_[All]) & ~(board_[(side_to_move == White) ? WhiteAll : BlackAll]);

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      auto is_capture =
          static_cast<bool>(is_set(target_square, board_[side_to_move == White ? BlackAll : WhiteAll]));

      addMove(moves,
              source_square,
              target_square,
              piece_t, NoPiece, is_capture, 0, 0, 0);

      clear_bit(target_square, attacks);
    }

    clear_bit(source_square, board);
  }
}

/*******************************************************************************
 *
 * Method: generateRookMoves(Color side_to_move)
 *
 *******************************************************************************/
void MoveGenerator::generateRookMoves(const Board& board_, Color side_to_move,
                                  std::vector<util::bits::HashedMove>& moves) const
{
  auto piece_t = (side_to_move == White) ? WhiteRook : BlackRook;

  Bitboard board = board_[piece_t];
  Bitboard attacks = 0ULL;

  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = getRookAttacks(source_square, board_[All]) & ~(board_[(side_to_move == White) ? WhiteAll : BlackAll]);

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      auto is_capture =
          static_cast<bool>(is_set(target_square, board_[side_to_move == White ? BlackAll : WhiteAll]));

      addMove(moves,
              source_square,
              target_square,
              piece_t, NoPiece, is_capture, 0, 0, 0);

      clear_bit(target_square, attacks);
    }

    clear_bit(source_square, board);
  }
}

/*******************************************************************************
 *
 * Method: generateQueenMoves(Color side_to_move)
 *
 *******************************************************************************/
void MoveGenerator::generateQueenMoves(const Board& board_, Color side_to_move,
                                   std::vector<util::bits::HashedMove>& moves) const
{
  auto piece_t = (side_to_move == White) ? WhiteQueen : BlackQueen;

  Bitboard board = board_[piece_t];
  Bitboard attacks = 0ULL;

  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = getQueenAttacks(source_square, board_[All]) & ~(board_[(side_to_move == White) ? WhiteAll : BlackAll]);

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      auto is_capture =
          static_cast<bool>(is_set(target_square, board_[side_to_move == White ? BlackAll : WhiteAll]));

      addMove(moves,
              source_square,
              target_square,
              piece_t, NoPiece, is_capture, 0, 0, 0);

      clear_bit(target_square, attacks);
    }

    clear_bit(source_square, board);
  }
}

/*******************************************************************************
 *
 * Method: generateKingMoves(Color side_to_move)
 *
 *******************************************************************************/
void MoveGenerator::generateKingMoves(const Board& board_, Color side_to_move,
                                  std::vector<util::bits::HashedMove>& moves) const
{
  auto piece_t = (side_to_move == White) ? WhiteKing : BlackKing;

  Bitboard board = board_[piece_t];
  Bitboard attacks = 0ULL;

  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = king_attacks[source_square] & (side_to_move == White ? ~(board_[WhiteAll]) : ~(board_[BlackAll]));

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      auto is_capture =
          static_cast<bool>(is_set(target_square, board_[side_to_move == White ? BlackAll : WhiteAll]));

      addMove(moves,
              source_square,
              target_square,
              piece_t, NoPiece, is_capture, 0, 0, 0);

      clear_bit(target_square, attacks);
    }

    clear_bit(source_square, board);
  }
}

/*******************************************************************************
 *
 * Method: testAttackLookup()
 *
 *******************************************************************************/
void MoveGenerator::testAttackLookup()
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for (auto i = 0; i < 100'000'000; i ++) {
    std::uniform_int_distribution<uint64_t> dis(
      std::numeric_limits<std::uint64_t>::min(),
      std::numeric_limits<std::uint64_t>::max());

    Bitboard occ = {dis(gen)};
    uint8_t r_square = (rand() % 64);

    Bitboard bishop_actual = getBishopAttacks(r_square, occ);
    Bitboard bishop_calculated = calcBishopAttacks(r_square, occ);

    Bitboard rook_actual = getRookAttacks(r_square, occ);
    Bitboard rook_calculated = calcRookAttacks(r_square, occ);

    Bitboard queen_actual = getQueenAttacks(r_square, occ);
    Bitboard queen_calculated =
      (calcRookAttacks(r_square, occ)) | calcBishopAttacks(r_square, occ);

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
