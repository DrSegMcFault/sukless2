#pragma once

#include <cstdint>
#include <string>

namespace chess {

  const static std::string starting_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  // useful constants
  static constexpr uint64_t not_a_file = 18374403900871474942ULL;
  static constexpr uint64_t not_h_file = 9187201950435737471ULL;
  static constexpr uint64_t not_hg_file = 4557430888798830399ULL;
  static constexpr uint64_t not_ab_file = 18229723555195321596ULL;

  static constexpr uint8_t NoSquare = 64;
  static constexpr uint8_t A1 = 0;  static constexpr uint8_t B1 = 1;
  static constexpr uint8_t C1 = 2;  static constexpr uint8_t D1 = 3;
  static constexpr uint8_t E1 = 4;  static constexpr uint8_t F1 = 5;
  static constexpr uint8_t G1 = 6;  static constexpr uint8_t H1 = 7;
  static constexpr uint8_t A2 = 8;  static constexpr uint8_t B2 = 9;
  static constexpr uint8_t C2 = 10; static constexpr uint8_t D2 = 11;
  static constexpr uint8_t E2 = 12; static constexpr uint8_t F2 = 13;
  static constexpr uint8_t G2 = 14; static constexpr uint8_t H2 = 15;
  static constexpr uint8_t A3 = 16; static constexpr uint8_t B3 = 17;
  static constexpr uint8_t C3 = 18; static constexpr uint8_t D3 = 19;
  static constexpr uint8_t E3 = 20; static constexpr uint8_t F3 = 21;
  static constexpr uint8_t G3 = 22; static constexpr uint8_t H3 = 23;
  static constexpr uint8_t A4 = 24; static constexpr uint8_t B4 = 25;
  static constexpr uint8_t C4 = 26; static constexpr uint8_t D4 = 27;
  static constexpr uint8_t E4 = 28; static constexpr uint8_t F4 = 29;
  static constexpr uint8_t G4 = 30; static constexpr uint8_t H4 = 31;
  static constexpr uint8_t A5 = 32; static constexpr uint8_t B5 = 33;
  static constexpr uint8_t C5 = 34; static constexpr uint8_t D5 = 35;
  static constexpr uint8_t E5 = 36; static constexpr uint8_t F5 = 37;
  static constexpr uint8_t G5 = 38; static constexpr uint8_t H5 = 39;
  static constexpr uint8_t A6 = 40; static constexpr uint8_t B6 = 41;
  static constexpr uint8_t C6 = 42; static constexpr uint8_t D6 = 43;
  static constexpr uint8_t E6 = 44; static constexpr uint8_t F6 = 45;
  static constexpr uint8_t G6 = 46; static constexpr uint8_t H6 = 47;
  static constexpr uint8_t A7 = 48; static constexpr uint8_t B7 = 49;
  static constexpr uint8_t C7 = 50; static constexpr uint8_t D7 = 51;
  static constexpr uint8_t E7 = 52; static constexpr uint8_t F7 = 53;
  static constexpr uint8_t G7 = 54; static constexpr uint8_t H7 = 55;
  static constexpr uint8_t A8 = 56; static constexpr uint8_t B8 = 57;
  static constexpr uint8_t C8 = 58; static constexpr uint8_t D8 = 59;
  static constexpr uint8_t E8 = 60; static constexpr uint8_t F8 = 61;
  static constexpr uint8_t G8 = 62; static constexpr uint8_t H8 = 63;

}
