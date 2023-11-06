#pragma once

#include <iostream>
namespace chess {

class Bitboard {

  private:
    uint64_t _b;

  public:
    Bitboard() = default;
    constexpr Bitboard(uint64_t i) : _b(i) {}

    // set the bit at index i
    constexpr void set(int i) { _b |= (1ULL << i); }

    // clear the bit at index i
    constexpr void clear(int i) { _b &= ~(1ULL << i); }

    // is the bit at index i set
    constexpr bool is_set(int i) const {
      return (_b & (1ULL << i));
    }

    // move the bit at index 'from' to index 'to'
    constexpr void move(int from, int to) {
      clear(from);
      set(to);
    }

    // count the number of set bits
    constexpr uint32_t count() const {
      uint32_t count = 0;
      uint64_t b = _b;

      while (b) {
        count++;
        b &= b - 1;
      }
      return count;
    }

    // print the bitboard in a human readable format
    void print() const {
      for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            if (!file)
                std::cout << "  " << rank + 1 << " ";
            std::cout << is_set(square) << " ";
        }
        if (rank != 0) {
          std::cout << "\n";
        }
      }
      std::cout << "\n    a b c d e f g h\n\n" << std::endl;
    }

    Bitboard& operator&=(const Bitboard& other) {
        _b &= other._b;
        return *this;
    }

    Bitboard& operator|=(const Bitboard& other) {
        _b |= other._b;
        return *this;
    }

    operator bool() const { return _b != 0; }
};
} // namespace chess
