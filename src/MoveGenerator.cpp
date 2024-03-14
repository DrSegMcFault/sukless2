#include "MoveGenerator.hxx"

namespace chess {

/*******************************************************************************
 *
 * Method: MoveGenerator()
 *
 *******************************************************************************/
MoveGenerator::MoveGenerator()
  : pawn_attacks(initPawnAttacks())
  , knight_attacks(initKnightAttacks())
  , king_attacks(initKingAttacks())
  , bishop_masks(initBishopMasks())
  , rook_masks(initRookMasks())
  , bishop_attacks(initSliderAttacks<512, true>(bishop_masks, bishop_magics, bishop_bits))
  , rook_attacks(initSliderAttacks<4096, false>(rook_masks, rook_magics, rook_bits))
{
}

/*******************************************************************************
 *
 * Method: addMove()
 *
 *******************************************************************************/
inline void MoveGenerator::addMove(std::vector<HashedMove>& moves,
                                   uint32_t source, uint32_t target,
                                   uint32_t piece, uint32_t promotion,
                                   uint32_t capture, uint32_t double_push,
                                   uint32_t enpassant, uint32_t castling) const
{
  HashedMove move;
  move.m.source = source;
  move.m.target = target;
  move.m.piece = piece;
  move.m.promoted = promotion;
  move.m.capture = capture;
  move.m.double_push = double_push;
  move.m.enpassant = enpassant;
  move.m.castling = castling;

  moves.push_back(move);
}

/*******************************************************************************
 *
 * Method: generateMoves(Board&, BoardState&, HashedMove& moves)
 *
 *******************************************************************************/
void MoveGenerator::generateMoves(const Board& b,
                                  const BoardState& s,
                                  std::vector<HashedMove>& moves) const
{
  switch (s.side_to_move) {
    case White:
    {
      generateWhitePawnMoves(b, s, moves);
      generateCastlingMoves<White>(b, s, moves);
      generateKnightMoves<White>(b, moves);
      generateBishopMoves<White>(b, moves);
      generateRookMoves<White>(b, moves);
      generateQueenMoves<White>(b, moves);
      generateKingMoves<White>(b, moves);
      break;
    }
    case Black:
    {
      generateBlackPawnMoves(b, s, moves);
      generateCastlingMoves<Black>(b, s, moves);
      generateKnightMoves<Black>(b, moves);
      generateBishopMoves<Black>(b, moves);
      generateRookMoves<Black>(b, moves);
      generateQueenMoves<Black>(b, moves);
      generateKingMoves<Black>(b, moves);
      break;
    }
  }

}

/*******************************************************************************
 *
 * Method: initPawnAttacks()
 *
 *******************************************************************************/
std::array<std::array<Bitboard, 64>, 2> MoveGenerator::initPawnAttacks()
{
  std::array<std::array<Bitboard, 64>, 2> result;

    constexpr auto get_mask = []<Color C>(uint8_t square) {
    Bitboard attacks {0ULL};
    Bitboard b {0ULL};

    set_bit(square, b);

    if constexpr (C == White) {
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

  for (const auto i : util::range(NoSquare)) {
    result[White][i] = get_mask.template operator()<White>(i);
    result[Black][i] = get_mask.template operator()<Black>(i);
  }

  return result;
}

/*******************************************************************************
 *
 * Method: initKnightAttacks()
 *
 *******************************************************************************/
std::array<Bitboard, 64> MoveGenerator::initKnightAttacks()
{
  std::array<Bitboard, 64> result;

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

    result[square] = attacks;
  }

  return result;
}

/*******************************************************************************
 *
 * Method: initKingAttacks()
 *
 *******************************************************************************/
std::array<Bitboard, 64> MoveGenerator::initKingAttacks()
{
  std::array<Bitboard, 64> result;

  for (const auto square : util::range(NoSquare)) {
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

    result[square] = attacks;
  }

  return result;
}

/*******************************************************************************
 *
 * Method: initBishopMasks()
 *
 *******************************************************************************/
std::array<Bitboard, 64> MoveGenerator::initBishopMasks()
{
  std::array<Bitboard, 64> result;

  for (const auto square : util::range(NoSquare)) {
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

    result[square] = attacks;
  }
  return result;
}

/*******************************************************************************
 *
 * Method: initRookMasks()
 *
 *******************************************************************************/
std::array<Bitboard, 64> MoveGenerator::initRookMasks()
{
  std::array<Bitboard, 64> result;

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

    result[square] = attacks;
  }
  return result;
}

/*******************************************************************************
 *
 * Method: calcBishopAttacks(uint8_t square, Bitboard occ)
 *
 *******************************************************************************/
Bitboard MoveGenerator::calcBishopAttacks(uint8_t square, Bitboard occ) const
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
 * Method: calcRookAttacks(uint8_t square, Bitboard occ)
 *
 *******************************************************************************/
Bitboard MoveGenerator::calcRookAttacks(uint8_t square, Bitboard occ) const
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
  // check if the opposing colors piece can attack it
  // even if the opposing piece isnt there, this by definition
  // gives us the desired result
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
void MoveGenerator::generateWhitePawnMoves(const Board& board_,
                                           const BoardState& state,
                                           std::vector<HashedMove>& moves) const
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
void MoveGenerator::generateBlackPawnMoves(const Board& board_,
                                           const BoardState& state,
                                           std::vector<HashedMove>& moves) const
{
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
template<Color c>
void MoveGenerator::generateCastlingMoves(const Board& board_,
                                          const BoardState& state,
                                          std::vector<HashedMove>& moves) const
{
  using namespace util;
  if constexpr (c == White) {
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
  }

  else {
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

  }
}

/*******************************************************************************
 *
 * Method: generateKnightMoves(Color side_to_move)
 *
 *******************************************************************************/
template<Color side>
void MoveGenerator::generateKnightMoves(const Board& board_,
                                        std::vector<HashedMove>& moves) const
{
  constexpr Piece piece_t = (side == White) ? WhiteKnight : BlackKnight;
  constexpr Piece all_color = (side == White) ? WhiteAll : BlackAll;
  constexpr Piece opp_color = (side == White) ? BlackAll : WhiteAll;

  Bitboard board = board_[piece_t];
  Bitboard attacks = 0ULL;

  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = knight_attacks[source_square] & ~(board_[all_color]);

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      auto is_capture =
          static_cast<bool>(is_set(target_square, board_[opp_color]));

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
template<Color side>
void MoveGenerator::generateBishopMoves(const Board& board_,
                                        std::vector<HashedMove>& moves) const
{
  constexpr Piece piece_t = (side == White) ? WhiteBishop : BlackBishop;
  constexpr Piece all_color = (side == White) ? WhiteAll : BlackAll;
  constexpr Piece opp_color = (side == White) ? BlackAll : WhiteAll;

  Bitboard board = board_[piece_t];
  Bitboard attacks = 0ULL;

  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = getBishopAttacks(source_square, board_[All]) & ~(board_[all_color]);

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      auto is_capture =
          static_cast<bool>(is_set(target_square, board_[opp_color]));

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
template<Color side>
void MoveGenerator::generateRookMoves(const Board& board_,
                                      std::vector<HashedMove>& moves) const
{
  constexpr Piece piece_t = (side == White) ? WhiteRook : BlackRook;
  constexpr Piece all_color = (side == White) ? WhiteAll : BlackAll;
  constexpr Piece opp_color = (side == White) ? BlackAll : WhiteAll;

  Bitboard board = board_[piece_t];
  Bitboard attacks = 0ULL;

  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = getRookAttacks(source_square, board_[All]) & ~(board_[all_color]);

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      auto is_capture =
          static_cast<bool>(is_set(target_square, board_[opp_color]));

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
template<Color side>
void MoveGenerator::generateQueenMoves(const Board& board_,
                                       std::vector<HashedMove>& moves) const
{
  constexpr Piece piece_t = (side == White) ? WhiteQueen : BlackQueen;
  constexpr Piece all_color = (side == White) ? WhiteAll : BlackAll;
  constexpr Piece opp_color = (side == White) ? BlackAll : WhiteAll;

  Bitboard board = board_[piece_t];
  Bitboard attacks = 0ULL;

  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = getQueenAttacks(source_square, board_[All]) & ~(board_[all_color]);

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      auto is_capture =
          static_cast<bool>(is_set(target_square, board_[opp_color]));

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
 * Method: generateKingMoves(const Board&, std::vector& moves)
 *
 *******************************************************************************/
template<Color side>
void MoveGenerator::generateKingMoves(const Board& board_,
                                      std::vector<HashedMove>& moves) const
{
  constexpr Piece piece_t = (side == White) ? WhiteKing : BlackKing;
  constexpr Piece all_color = (side == White) ? WhiteAll : BlackAll;
  constexpr Piece opp_color = (side == White ) ? BlackAll : WhiteAll;

  Bitboard board = board_[piece_t];
  Bitboard attacks = 0ULL;

  uint8_t source_square = 0;
  uint8_t target_square = 0;

  while (board) {
    source_square = util::bits::get_lsb_index(board);
    attacks = king_attacks[source_square] & ~(board_[all_color]);

    while (attacks) {
      target_square = util::bits::get_lsb_index(attacks);

      auto is_capture =
          static_cast<bool>(is_set(target_square, board_[opp_color]));

      addMove(moves,
              source_square,
              target_square,
              piece_t, NoPiece, is_capture, 0, 0, 0);

      clear_bit(target_square, attacks);
    }

    clear_bit(source_square, board);
  }
}
} // namespace chess
