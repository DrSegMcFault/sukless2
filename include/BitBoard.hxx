#pragma once

#include <cstdint>
#include <type_traits>
#include <iostream>

class BitBoard {

  private:
    uint64_t _b;

  public:
    BitBoard() :_b(0ULL) {}

    BitBoard(uint64_t i) : _b(i) {}

    void set(int i) { 
      _b |= (1ULL << i);
    }

    const bool is_set(int i) const { 
      return (_b & (1ULL << i)); 
    }

    void clear(int i) {
      _b &= ~(1ULL << i);
    }

    void move(int from, int to) {
      clear(from);
      set(to);
    }

    uint32_t count() const {
      uint32_t count = 0;
      uint64_t b = _b;

      while (b) {
        count++;
        b &= b - 1;
      }
      return count;
    }

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
};
