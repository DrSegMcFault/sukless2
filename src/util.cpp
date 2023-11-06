#include "util.hxx"

void chess::print_board(const Bitboard& b) {
  for (int rank = 7; rank >= 0; rank--) {
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      if (!file)
        std::cout << "  " << rank + 1 << " ";
      std::cout << is_set(square, b) << " ";
    }
    if (rank != 0) {
      std::cout << "\n";
    }
  }
  std::cout << "\n    a b c d e f g h\n\n" << std::endl;
}

int chess::get_lsb_index(Bitboard b)
{
  if (b) {
    return count((b & -b) - 1);
  } else {
    throw std::runtime_error("get_lsb_index: bitboard is empty");
  }
}
