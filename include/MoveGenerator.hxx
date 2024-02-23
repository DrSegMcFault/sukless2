#pragma once

#include <vector>

#include "util.hxx"

namespace chess {

class MoveGenerator {
  public:

    MoveGenerator(const MoveGenerator&) = delete;
    MoveGenerator(MoveGenerator&&) = delete;
    MoveGenerator& operator=(const MoveGenerator&) = delete;

    bool isSquareAttacked(uint8_t square,
                          Color side,
                          const Board& board) const;

    void generateMoves(const Board& board,
                       const State& state,
                       std::vector<HashedMove>& moves) const;
  private:

    // pre-calculated attack Bitboards
    const std::array<std::array<Bitboard, 64>, 2> pawn_attacks;
    const std::array<Bitboard, 64> knight_attacks;
    const std::array<Bitboard, 64> king_attacks;

    // attack masks per square
    const std::array<Bitboard, 64> bishop_masks;
    const std::array<Bitboard, 64> rook_masks;

    const std::array<std::array<Bitboard, 512>, 64> bishop_attacks;
    const std::array<std::array<Bitboard, 4096>, 64> rook_attacks;

    // initialization functions
    constexpr std::array<std::array<Bitboard, 64>, 2> initPawnAttacks() {
      std::array<std::array<Bitboard, 64>, 2> result;

      constexpr auto get_mask = [](Color side, uint8_t square) {
        Bitboard attacks {0ULL};
        Bitboard b {0ULL};

        set_bit(square, b);

        if (side == White) {
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
        result[White][i] = get_mask(White, i);
        result[Black][i] = get_mask(Black, i);
      }

      return result;
    }

    constexpr std::array<Bitboard, 64> initKnightAttacks()
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

    constexpr std::array<Bitboard, 64> initKingAttacks()
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

    constexpr std::array<Bitboard, 64> initBishopMasks()
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

    constexpr std::array<Bitboard, 64> initRookMasks()
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

    constexpr Bitboard calcBishopAttacks(uint8_t square, Bitboard occ) const
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

    constexpr Bitboard calcRookAttacks(uint8_t square, Bitboard occ) const
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

    // attack retrieval functions
    Bitboard getBishopAttacks(uint8_t square, Bitboard occ) const;
    Bitboard getRookAttacks(uint8_t square, Bitboard occ) const;
    Bitboard getQueenAttacks(uint8_t square, Bitboard occ) const
    {
      return (getBishopAttacks(square, occ) | getRookAttacks(square, occ));
    }

    // move generation
    inline void addMove(std::vector<HashedMove>& moves,
                        uint32_t source, uint32_t target,
                        uint32_t piece, uint32_t promotion,
                        uint32_t capture, uint32_t double_push,
                        uint32_t enpassant, uint32_t castling) const;

    void generateWhitePawnMoves(const Board& board,
                                const State& state,
                                std::vector<HashedMove>& moves) const;

    void generateBlackPawnMoves(const Board& board,
                                const State& state,
                                std::vector<HashedMove>& moves) const;

    void generateCastlingMoves(const Board& board,
                               const State& state,
                               std::vector<HashedMove>& moves) const;

    void generateKingMoves(const Board& b,
                           Color side_to_move,
                           std::vector<HashedMove>& moves) const;

    void generateKnightMoves(const Board& b,
                             Color side_to_move,
                             std::vector<HashedMove>& moves) const;

    void generateBishopMoves(const Board& b,
                             Color side_to_move,
                             std::vector<HashedMove>& moves) const;

    void generateRookMoves(const Board& b,
                           Color side_to_move,
                           std::vector<HashedMove>& moves) const;

    void generateQueenMoves(const Board& b,
                            Color side_to_move,
                            std::vector<HashedMove>& moves) const;

    void testAttackLookup();

    static constexpr std::array<Bitboard, 64> bishop_magics = {
      0x40040844404084ULL,  0x2004208a004208ULL,
      0x10190041080202ULL,  0x108060845042010ULL,
      0x581104180800210ULL, 0x2112080446200010ULL,
      0x1080820820060210ULL,0x3c0808410220200ULL,
      0x4050404440404ULL,   0x21001420088ULL,
      0x24d0080801082102ULL,0x1020a0a020400ULL,
      0x40308200402ULL,     0x4011002100800ULL,
      0x401484104104005ULL, 0x801010402020200ULL,
      0x400210c3880100ULL,  0x404022024108200ULL,
      0x810018200204102ULL, 0x4002801a02003ULL,
      0x85040820080400ULL,  0x810102c808880400ULL,
      0xe900410884800ULL,   0x8002020480840102ULL,
      0x220200865090201ULL, 0x2010100a02021202ULL,
      0x152048408022401ULL, 0x20080002081110ULL,
      0x4001001021004000ULL,0x800040400a011002ULL,
      0xe4004081011002ULL,  0x1c004001012080ULL,
      0x8004200962a00220ULL,0x8422100208500202ULL,
      0x2000402200300c08ULL,0x8646020080080080ULL,
      0x80020a0200100808ULL,0x2010004880111000ULL,
      0x623000a080011400ULL,0x42008c0340209202ULL,
      0x209188240001000ULL, 0x400408a884001800ULL,
      0x110400a6080400ULL,  0x1840060a44020800ULL,
      0x90080104000041ULL,  0x201011000808101ULL,
      0x1a2208080504f080ULL,0x8012020600211212ULL,
      0x500861011240000ULL, 0x180806108200800ULL,
      0x4000020e01040044ULL,0x300000261044000aULL,
      0x802241102020002ULL, 0x20906061210001ULL,
      0x5a84841004010310ULL,0x4010801011c04ULL,
      0xa010109502200ULL,   0x4a02012000ULL,
      0x500201010098b028ULL,0x8040002811040900ULL,
      0x28000010020204ULL,  0x6000020202d0240ULL,
      0x8918844842082200ULL,0x4010011029020020ULL
    };

    static constexpr std::array<Bitboard, 64> rook_magics {
      0x8a80104000800020ULL, 0x140002000100040ULL,
      0x2801880a0017001ULL,  0x100081001000420ULL,
      0x200020010080420ULL,  0x3001c0002010008ULL,
      0x8480008002000100ULL, 0x2080088004402900ULL,
      0x800098204000ULL,     0x2024401000200040ULL,
      0x100802000801000ULL,  0x120800800801000ULL,
      0x208808088000400ULL,  0x2802200800400ULL,
      0x2200800100020080ULL, 0x801000060821100ULL,
      0x80044006422000ULL,   0x100808020004000ULL,
      0x12108a0010204200ULL, 0x140848010000802ULL,
      0x481828014002800ULL,  0x8094004002004100ULL,
      0x4010040010010802ULL, 0x20008806104ULL,
      0x100400080208000ULL,  0x2040002120081000ULL,
      0x21200680100081ULL,   0x20100080080080ULL,
      0x2000a00200410ULL,    0x20080800400ULL,
      0x80088400100102ULL,   0x80004600042881ULL,
      0x4040008040800020ULL, 0x440003000200801ULL,
      0x4200011004500ULL,    0x188020010100100ULL,
      0x14800401802800ULL,   0x2080040080800200ULL,
      0x124080204001001ULL,  0x200046502000484ULL,
      0x480400080088020ULL,  0x1000422010034000ULL,
      0x30200100110040ULL,   0x100021010009ULL,
      0x2002080100110004ULL, 0x202008004008002ULL,
      0x20020004010100ULL,   0x2048440040820001ULL,
      0x101002200408200ULL,  0x40802000401080ULL,
      0x4008142004410100ULL, 0x2060820c0120200ULL,
      0x1001004080100ULL,    0x20c020080040080ULL,
      0x2935610830022400ULL, 0x44440041009200ULL,
      0x280001040802101ULL,  0x2100190040002085ULL,
      0x80c0084100102001ULL, 0x4024081001000421ULL,
      0x20030a0244872ULL,    0x12001008414402ULL,
      0x2006104900a0804ULL,  0x1004081002402ULL
    };

    // bitcounts for each mask
    static constexpr std::array<uint8_t, 64> bishop_bits =
    { 6, 5, 5, 5, 5, 5, 5, 6,
      5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 7, 7, 7, 7, 5, 5,
      5, 5, 7, 9, 9, 7, 5, 5,
      5, 5, 7, 9, 9, 7, 5, 5,
      5, 5, 7, 7, 7, 7, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5,
      6, 5, 5, 5, 5, 5, 5, 6 };

    // bitcounts for each mask
    static constexpr std::array<uint8_t, 64> rook_bits =
    { 12, 11, 11, 11, 11, 11, 11, 12,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      12, 11, 11, 11, 11, 11, 11, 12 };

    // used to initialize a big array of attacks at compile time,
    // currently for rook and bishop attack tables
    template <size_t N, bool is_bishop>
    constexpr auto initSliderAttacks(const std::array<Bitboard, 64> masks,
                                     const std::array<Bitboard, 64> magics,
                                     const std::array<uint8_t, 64> bits)
    {
      std::array<std::array<Bitboard, N>, 64> result = {};

      // generates the ith permutation of the attack mask
      constexpr auto ith_permutation =
        [](uint64_t index, uint8_t bits_in_mask, Bitboard attack_mask)
      {
         Bitboard occupancy = 0ULL;

         for (const auto count : util::range(bits_in_mask))
         {
           uint8_t square = util::bits::get_lsb_index(attack_mask);
           clear_bit(square, attack_mask);

           if (is_set(count, index)) {
             set_bit(square, occupancy);
           }
        }

        return occupancy;
      };

      for (const auto square : util::range(NoSquare)) {

        Bitboard attack_mask = masks[square];
        uint8_t bit_count = bits[square];
        uint64_t permutations = (1ULL << bit_count);

        for (const auto index : util::range(permutations))
        {
          Bitboard occupancy = ith_permutation(index, bit_count, attack_mask);

          uint16_t magic_index = (occupancy * magics[square]) >> (64ULL - bit_count);

          if constexpr (is_bishop) {
            result[square][magic_index] = calcBishopAttacks(square, occupancy);
          }
          else {
            result[square][magic_index] = calcRookAttacks(square, occupancy);
          }
        }
      }

      return result;
    }

  public:
    constexpr MoveGenerator()
      : pawn_attacks(initPawnAttacks())
      , knight_attacks(initKnightAttacks())
      , king_attacks(initKingAttacks())
      , bishop_masks(initBishopMasks())
      , rook_masks(initRookMasks())
      , bishop_attacks(initSliderAttacks<512, true>(bishop_masks, bishop_magics, bishop_bits))
      , rook_attacks(initSliderAttacks<4096, false>(rook_masks, rook_magics, rook_bits))
    {
    }
};
} // namespace chess
