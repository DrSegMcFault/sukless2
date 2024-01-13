#include "util.hxx"

/*******************************************************************************
 *
 * Function: chess::print_board(const Bitboard& b)
 *
 *******************************************************************************/
void chess::print_board(const Bitboard& b) {
  for (int rank = 7; rank >= 0; rank--) {
    for (int file = 0; file < 8; file++) {
      uint8_t square = rank * 8 + file;
      if (!file)
        std::cout << "  " << rank + 1 << " ";
      std::cout << ((is_set(square, b)) ? "1" : "0") << " ";
    }
    if (rank != 0) {
      std::cout << "\n";
    }
  }
  std::cout << "\n    a b c d e f g h\n\n" << std::endl;
}

/*******************************************************************************
 *
 * Function: algebraic_to_index(const std::string& alg)
 * converts algebraic notation to the associated index
 *******************************************************************************/
std::optional<uint8_t> chess::util::fen::algebraic_to_index(const std::string& alg)
{
  std::optional<uint8_t> index;

  if (alg.length() != 2) {
    return index;
  }

  const char& file = alg[0];
  const char& rank = alg[1];

  if (file < 'a' || file > 'h' || rank < '1' || rank > '8') {
      std::cerr << "Invalid algebraic notation: "
        << alg << std::endl;
      return index;
  }
  index.emplace((rank - '1') * 8 + (file - 'a'));
  return index;
}

/*******************************************************************************
 *
 * Function: algebraic_to_index(const std::string& alg)
 * converts algebraic notation to the associated index
 *******************************************************************************/
std::optional<std::string> chess::util::fen::index_to_algebraic(uint8_t index)
{
  if (index < chess::A1 || index > chess::H8) {
    return std::nullopt;
  }

  std::string ret;
  ret += (index / 8) + '1';
  ret += (index % 8) + 'a';

  return ret;
}

/*******************************************************************************
 *
 * Function: piece_from_char(const std::string& alg)
 *
 *******************************************************************************/
chess::Piece chess::util::fen::char_to_piece(char c) {
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

/*******************************************************************************
 *
 * Function: char_from_piece(Piece p)
 *
 *******************************************************************************/
char chess::util::fen::piece_to_char(Piece p)
{
  switch (p) {
    case w_pawn: return 'P';
    case w_knight: return 'N';
    case w_bishop: return 'B';
    case w_rook: return 'R';
    case w_queen: return 'Q';
    case w_king: return 'K';
    case b_pawn: return 'p';
    case b_knight: return 'n';
    case b_bishop: return 'b';
    case b_rook: return 'r';
    case b_queen: return 'q';
    case b_king: return 'k';
    case b_all:
    case w_all:
    case All:
      throw std::runtime_error("in function: char_from_piece()::invalid piece");
  }
}
