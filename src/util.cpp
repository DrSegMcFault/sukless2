#include "util.hxx"

/*******************************************************************************
 *
 * Function: chess::print_board(const Bitboard& b)
 *
 *******************************************************************************/
void chess::print_board(const Bitboard& b) {
  for (int rank = 7; rank >= 0; rank--) {
    for (int file = 0; file < 8; file++) {
      int square = rank * 8 + file;
      if (!file)
        std::cout << "  " << rank + 1 << " ";
      std::cout << util::bits::is_set(square, b) << " ";
    }
    if (rank != 0) {
      std::cout << "\n";
    }
  }
  std::cout << "\n    a b c d e f g h\n\n" << std::endl;
}

/*******************************************************************************
 *
 * Function: chess::util::bits::get_lsb_index(Bitboard b)
 *
 *******************************************************************************/
int chess::util::bits::get_lsb_index(Bitboard b)
{
  if (b) {
    return count((b & -b) - 1);
  } else {
    throw std::runtime_error("get_lsb_index: bitboard is empty");
  }
}

/*******************************************************************************
 *
 * Function: algebraic_to_index(const std::string& alg)
 * converts algebraic notation to the associated index
 *******************************************************************************/
std::optional<int> chess::util::fen::algebraic_to_index(const std::string& alg)
{
  std::optional<int> index;

  if (alg.length() != 2) {
    return index;
  }

  const char& file = alg[0];
  const char& rank = alg[1];

  if (file < 'a' || file > 'h' || rank < '1' || rank > '8') {
      std::cerr << "Invalid algebraic notation: "
        << alg << std::endl;
      return -1;
  }
  index.emplace((rank - '1') * 8 + (file - 'a'));
  return index;
}

/*******************************************************************************
 *
 * Function: piece_from_char(const std::string& alg)
 *
 *******************************************************************************/
chess::Piece chess::util::fen::piece_from_char(char c) {
  using enum Piece;
  switch (c) {
    case 'P': return w_pawn;
    case 'N': return w_knight;
    case 'B': return w_bishop;
    case 'R': return w_rook;
    case 'Q': return w_queen;
    case 'K': return w_king;
    case 'p': return b_pawn;
    case 'n': return b_knight;
    case 'b': return b_bishop;
    case 'r': return b_rook;
    case 'q': return b_queen;
    case 'k': return b_king;
    default:
      throw std::runtime_error("in function: piece_from_char()::invalid FEN string");
  }
}
