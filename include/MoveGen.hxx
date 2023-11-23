#pragma once

#include "util.hxx"
#include "BoardConstants.hxx"

namespace chess {

class MoveGen {
  public:
    MoveGen();
    MoveGen(const MoveGen&) = delete;
    MoveGen(MoveGen&&) = delete;
    MoveGen& operator=(const MoveGen&) = delete;

    bool is_square_attacked(uint8_t square,
                            Color side,
                            const Board& board) const;

    void generate_moves(const Board& board,
                        const State& state,
                        std::vector<util::bits::HashedMove>& moves);
  private:

    // pre-calculated attack Bitboards
    std::array<std::array<Bitboard, 64>, 2> pawn_attacks;
    std::array<Bitboard, 64> knight_attacks;
    std::array<Bitboard, 64> king_attacks;
    std::array<std::array<Bitboard, 512>, 64> bishop_attacks;
    std::array<std::array<Bitboard, 4096>, 64> rook_attacks; 

    // attack masks per square
    std::array<Bitboard, 64> bishop_masks;
    std::array<Bitboard, 64> rook_masks;


    // attack retrieval functions
    Bitboard get_bishop_attacks(uint8_t square, Bitboard occ) const;
    Bitboard get_rook_attacks(uint8_t square, Bitboard occ) const;
    Bitboard get_queen_attacks(uint8_t square, Bitboard occ) const
    {
      return (get_bishop_attacks(square, occ) | get_rook_attacks(square, occ));
    }

    // initialization functions
    constexpr void init_attack_tables();
    constexpr void init_pawn_attacks();
    constexpr void init_knight_attacks();
    constexpr void init_king_attacks();
    constexpr void init_bishop_masks();
    constexpr void init_rook_masks();
    void init_slider_attacks();

    constexpr Bitboard calc_bishop_attacks(uint8_t square, Bitboard occ) const;
    constexpr Bitboard calc_rook_attacks(uint8_t square, Bitboard occ) const;

    // move generation
    inline void add_move(std::vector<util::bits::HashedMove>& moves,
                         uint32_t source, uint32_t target,
                         uint32_t piece, uint32_t promotion,
                         uint32_t capture, uint32_t double_push,
                         uint32_t enpassant, uint32_t castling);


    void generate_white_moves(const Board& board,
                              const State& state,
                              std::vector<util::bits::HashedMove>& moves);

    void generate_black_moves(const Board& board,
                              const State& state,
                              std::vector<util::bits::HashedMove>& moves);

    void generate_white_pawn_moves(const Board& board,
                                   const State& state,
                                   std::vector<util::bits::HashedMove>& moves);

    void generate_black_pawn_moves(const Board& board,
                                   const State& state,
                                   std::vector<util::bits::HashedMove>& moves);

    void generate_white_castling_moves(const Board& board,
                                       const State& state,
                                       std::vector<util::bits::HashedMove>& moves);

    void generate_black_castling_moves(const Board& board,
                                       const State& state,
                                       std::vector<util::bits::HashedMove>& moves);

    void generate_king_moves(const Board& b, Color side_to_move,
                             std::vector<util::bits::HashedMove>& moves);

    void generate_knight_moves(const Board& b, Color side_to_move,
                                std::vector<util::bits::HashedMove>& moves);

    void generate_bishop_moves(const Board& b, Color side_to_move,
                                std::vector<util::bits::HashedMove>& moves);

    void generate_rook_moves(const Board& b, Color side_to_move,
                              std::vector<util::bits::HashedMove>& moves);

    void generate_queen_moves(const Board& b, Color side_to_move,
                               std::vector<util::bits::HashedMove>& moves);

    // tests
    void test_attack_lookup();
};
}
