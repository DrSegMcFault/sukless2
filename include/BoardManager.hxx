#pragma once
#include "BitBoard.hxx"
#include "enums.hxx"

namespace chess {

// c is short for cast if anyone cares
#define c(x) static_cast<std::underlying_type_t<chess::Piece>>(x)

class BoardManager
{
  private:
   BitBoard _board[13];

   // pre-calculated attack bitboards
   BitBoard pawn_attacks[2][64];
   BitBoard knight_attacks[64];
   BitBoard king_attacks[64];

   // TODO: bishop, rook, and queen are special cases i dont
   // want to mess with right now, but it should be 
   // noted that the queen pre-calculated attack bitboard
   // is the union of the bishop and rook attack bitboards
   BitBoard queen_attacks;

   // the below relavent occupancy bbs are the possible
   // moves for each piece at each of the 64 squares
   // exluding the edges of the board
   // https://www.chessprogramming.org/Magic_Bitboards
   // under the 'How it Works' section
   BitBoard relavent_bishop_occ[64];
   BitBoard relavent_rook_occ[64];

   // useful constants
   static constexpr uint64_t not_a_file = 18374403900871474942ULL;
   static constexpr uint64_t not_h_file = 9187201950435737471ULL;
   static constexpr uint64_t not_hg_file = 4557430888798830399ULL;
   static constexpr uint64_t not_ab_file = 18229723555195321596ULL;

   // flags for the game state
   struct State {
     bool white_can_castle = true;
     bool black_can_castle = true;
     // target enpassant square
     int en_passant = -1;
   } _state;

 public:

  BoardManager();
  BoardManager(const BoardManager&);
  BoardManager(BoardManager&&) = delete;
  ~BoardManager() = default;

  bool is_attack_valid(const Move& m) const;

  constexpr void init_pawn_attacks();
  constexpr void init_knight_attacks();
  constexpr void init_king_attacks();
  constexpr void init_bishop_rel_occ();
  constexpr void init_rook_rel_occ();


  bool is_square_attacked(int square, Color side) const;
 
  const auto& operator[](Piece piece) {
    return _board[c(piece)];
  }

  auto get_piece_count(Piece piece) const {
    return _board[c(piece)].count();
  }

  void print(Piece p = Piece::All) {
    _board[c(p)].print();
  }
  // the All bit board will be updated after
  // each valid move, so no need to calculate it here
  const BitBoard& all() {
    return _board[c(Piece::All)];
  }
};
}