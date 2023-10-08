#pragma once
#include "BitBoard.hxx"
#include "enums.hxx"

namespace chess {
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
     BitBoard bishop_attacks;
     BitBoard rook_attacks;


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
 
     const auto& operator[](Piece piece) {
      return _board[c(piece)];
     }

     auto get_piece_count(Piece piece) const {
       return _board[c(piece)].count();
     }

     void print() {
       _board[c(Piece::All)].print();
     }
};
}