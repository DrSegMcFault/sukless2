#pragma once
#include "ChessTypes.hxx"
#include <cstdint>
#include <vector>

struct MoveList {

  MoveList() {
    moves.reserve(256);
  }

  std::vector<chess::HashedMove> moves;

  inline void add(
    uint32_t source, uint32_t target,
    uint32_t piece, uint32_t promotion = 0,
    uint32_t capture = 0,
    uint32_t double_push = 0,
    uint32_t enpassant = 0,
    uint32_t castling = 0)
  {
    moves.push_back({source, target, piece, promotion, capture, double_push, enpassant, castling});
  }

  auto begin() const { return moves.begin(); }
  auto end() const { return moves.end(); }

  auto cbegin() { return moves.cbegin(); }
  auto cend() { return moves.cend(); }

  auto size() const { return moves.size();}

  auto rbegin() { return moves.rbegin();}
  auto rend() { return moves.rend(); }

  void clear() { moves.clear(); }

  void sort(auto&& f) {
    std::sort(moves.begin(), moves.end(), f);
  }

  inline void add(uint32_t hashed) { moves.push_back(hashed); }
  inline void add(chess::HashedMove hashed) { moves.push_back(hashed); }
  inline void add(chess::HashedMove&& hashed) { moves.push_back(std::move(hashed)); }
};
