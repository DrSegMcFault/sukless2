#pragma once

#include <vector>
#include "ChessUtil.hxx"
#include "Util.hxx"

namespace chess {

class MoveGenerator {

public:
  MoveGenerator();
  MoveGenerator(const MoveGenerator&) = delete;
  MoveGenerator(MoveGenerator&&) = delete;
  MoveGenerator& operator=(const MoveGenerator&) = delete;

  bool isSquareAttacked(uint8_t square,
                        Color side,
                        const Board& board) const;

  void generateMoves(const Board& board,
                     const BoardState& state,
                     std::vector<HashedMove>& moves) const;
private:

  // pre-calculated attack Bitboards
  const std::vector<std::vector<Bitboard>> pawn_attacks;
  const std::vector<Bitboard> knight_attacks;
  const std::vector<Bitboard> king_attacks;

  // attack masks per square
  const std::vector<Bitboard> bishop_masks;
  const std::vector<Bitboard> rook_masks;

  const std::vector<std::vector<Bitboard>> bishop_attacks;
  const std::vector<std::vector<Bitboard>> rook_attacks;

  // initialization functions
  std::vector<std::vector<Bitboard>> initPawnAttacks();

  std::vector<Bitboard> initKnightAttacks();
  std::vector<Bitboard> initKingAttacks();
  std::vector<Bitboard> initBishopMasks();
  std::vector<Bitboard> initRookMasks();

  Bitboard calcBishopAttacks(uint8_t square, Bitboard occ) const;
  Bitboard calcRookAttacks(uint8_t square, Bitboard occ) const;

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
                              const BoardState& state,
                              std::vector<HashedMove>& moves) const;

  void generateBlackPawnMoves(const Board& board,
                              const BoardState& state,
                              std::vector<HashedMove>& moves) const;

  template<Color side>
  void generateCastlingMoves(const Board& board,
                             const BoardState& state,
                             std::vector<HashedMove>& moves) const;

  template<Color side>
  void generateKingMoves(const Board& b,
                         std::vector<HashedMove>& moves) const;

  template<Color side>
  void generateKnightMoves(const Board& b,
                           std::vector<HashedMove>& moves) const;

  template<Color side>
  void generateBishopMoves(const Board& b,
                           std::vector<HashedMove>& moves) const;

  template<Color side>
  void generateRookMoves(const Board& b,
                         std::vector<HashedMove>& moves) const;

  template<Color side>
  void generateQueenMoves(const Board& b,
                          std::vector<HashedMove>& moves) const;

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

  // used to initialize a big array of attacks
  // currently for rook and bishop attack tables
  template <size_t N, bool is_bishop>
  auto initSliderAttacks(const std::vector<Bitboard> masks,
                         const std::array<Bitboard, 64> magics,
                         const std::array<uint8_t, 64> bits)
  {
    std::vector<std::vector<Bitboard>> result(64, std::vector<Bitboard>(N));

    // generates the ith permutation of the attack mask
    auto ith_permutation =
      [](uint64_t index, uint8_t bits_in_mask, Bitboard attack_mask)
    {
      Bitboard occupancy = 0ULL;

      for (const auto count : util::range(bits_in_mask))
      {
        uint8_t square = bits::get_lsb_index(attack_mask);
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
};
} // namespace chess
