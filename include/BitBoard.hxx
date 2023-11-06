#pragma once

#include <iostream>

class BitBoard {

  private:
    uint64_t _b;

  public:
    BitBoard() = default;

    constexpr BitBoard(uint64_t i) : _b(i) {}

    constexpr void set(int i) {
      _b |= (1ULL << i);
    }

    constexpr bool is_set(int i) const {
      return (_b & (1ULL << i));
    }

    constexpr void clear(int i) {
      _b &= ~(1ULL << i);
    }

    constexpr void move(int from, int to) {
      clear(from);
      set(to);
    }

    uint64_t get() const { return _b; }

    constexpr uint32_t count() const {
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

    operator bool() const {
        return _b != 0;
    }
    
    bool operator==(const BitBoard& other) const {
      return _b == other._b;
    }

    BitBoard operator&(const BitBoard& other) const {
      BitBoard result(_b & other._b);
      return result;
    }

    BitBoard operator|(const BitBoard& other) const {
      BitBoard result(_b | other._b);
      return result;
    }

    BitBoard operator^(const BitBoard& other) const {
      BitBoard result(_b ^ other._b);
      return result;
    }

    BitBoard operator~() const {
      BitBoard result(~_b);
      return result;
    }

    BitBoard& operator&=(const BitBoard& other) {
      _b &= other._b;
      return *this;
    }

    BitBoard& operator|=(const BitBoard& other) {
      _b |= other._b;
      return *this;
    }

    BitBoard& operator^=(const BitBoard& other) {
      _b ^= other._b;
      return *this;
    }

};
