#include "util.hxx"

namespace chess {

/*******************************************************************************
 *
 * Function: chess::print_board(const Bitboard& b)
 *
 *******************************************************************************/
void print_board(const Bitboard& b) {
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
  std::cout << "\n    a b c d e f g h\n\n";
}

/*******************************************************************************
 *
 * Function: chess::piece_at(const Board& b, uint8_t square )
 *
 *******************************************************************************/
std::optional<Piece> piece_at(const Board& b, uint8_t square)
{
  for (auto p : AllPieces) {
    if (is_set(square, b[p])) {
      return p;
    }
  }
  return std::nullopt;
}

/*******************************************************************************
 *
 * Function: chess::to_string(const Bitboard& m)
 *
 *******************************************************************************/
std::string to_string(const Bitboard& b) {
  std::string ret;
  for (int rank = 7; rank >= 0; rank--) {
    for (int file = 0; file < 8; file++) {
      uint8_t square = rank * 8 + file;
      if (!file) {
        ret.append("  " + std::to_string(rank + 1) + " ");
      }
      std::string c = ((is_set(square, b)) ? "1 " : "0 ");
      ret.append(c);
    }
    if (rank != 0) {
      ret.append("\n");
    }
  }
  ret.append("\n    a b c d e f g h\n\n");
  return ret;
}
/*******************************************************************************
 *
 * Function: chess::to_string(const HashedMove& m)
 *
 *******************************************************************************/
std::string to_string(const HashedMove& m) {

  const auto&& [ source_square, target_square,
                 piece, promoted_to,
                 capture, double_push,
                 was_en_passant, castling ] = m.explode();
  std::string str;

  if (castling) {
    if (target_square == chess::G8 || target_square == chess::G1)
    {
      str = "O-O";
    }
    else {
      str = "O-O-O";
    }
  }
  else {
    auto str_target = *chess::util::fen::index_to_algebraic(target_square);

    if (!(piece == chess::WhitePawn || piece == chess::BlackPawn))
    {
      str += std::toupper(chess::util::fen::piece_to_char(piece));
    }

    if (auto alg = chess::util::fen::index_to_algebraic(source_square))
    {
      // only name the file if its a pawn capture
      if ((piece == chess::WhitePawn || piece == chess::BlackPawn) && capture)
      {
        str += chess::util::fen::index_to_algebraic(source_square).value().at(0);
      }
    }

    if (capture) {
      str.append("x");
    }

    str.append(str_target);

    if (chess::util::toul(promoted_to) != 0) {
      str.append("=");
      str += chess::util::fen::piece_to_char(promoted_to);
    }
  }
  return str;
}

/*******************************************************************************
 *
 * Function: chess::to_array(const Board& b)
 *
 *******************************************************************************/
std::array<std::optional<Piece>, 64> to_array(const Board& b)
{
  std::array<std::optional<Piece>, 64> board;
  for (auto i : util::range(NoSquare)) {
    board[i] = piece_at(b, i);
  }
  return board;
}

} // namespace chess

namespace chess::util::fen {

/*******************************************************************************
 *
 * Function: util::fen::algebraic_to_index(const std::string& alg)
 *
 *******************************************************************************/
std::optional<uint8_t> algebraic_to_index(const std::string& alg)
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
 * Function: util::fen::algebraic_to_index(const std::string& alg)
 * converts algebraic notation to the associated index
 *******************************************************************************/
std::optional<std::string> index_to_algebraic(uint8_t index)
{
  if (index < A1 || index > H8) {
    return std::nullopt;
  }

  std::string ret;
  ret += (index % 8) + 'a';
  ret += (index / 8) + '1';

  return ret;
}

/*******************************************************************************
 *
 * Function: util::fen::piece_from_char(const std::string& alg)
 *
 *******************************************************************************/
std::optional<Piece> char_to_piece(char c) {
  switch (c) {
    case 'P': return WhitePawn;
    case 'N': return WhiteKnight;
    case 'B': return WhiteBishop;
    case 'R': return WhiteRook;
    case 'Q': return WhiteQueen;
    case 'K': return WhiteKing;
    case 'p': return BlackPawn;
    case 'n': return BlackKnight;
    case 'b': return BlackBishop;
    case 'r': return BlackRook;
    case 'q': return BlackQueen;
    case 'k': return BlackKing;
    default:
      return std::nullopt;
  }
}

/*******************************************************************************
 *
 * Function: util::fen::char_from_piece(Piece p)
 *
 *******************************************************************************/
char piece_to_char(Piece p)
{
  switch (p) {
    case WhitePawn: return 'P';
    case WhiteKnight: return 'N';
    case WhiteBishop: return 'B';
    case WhiteRook: return 'R';
    case WhiteQueen: return 'Q';
    case WhiteKing: return 'K';
    case BlackPawn: return 'p';
    case BlackKnight: return 'n';
    case BlackBishop: return 'b';
    case BlackRook: return 'r';
    case BlackQueen: return 'q';
    case BlackKing: return 'k';
    case NoPiece:
    case BlackAll:
    case WhiteAll:
    case All:
      return ' ';
  }
  return ' ';
}

/*******************************************************************************
 *
 * Function: util::fen::generate(const Board& b, uint8_t square)
 *
 *******************************************************************************/
std::string generate(const Board& b, const State& state) {
  std::string fen;
  fen.reserve(40);

  uint8_t cur_missing_count = 0;
  static constexpr auto ranges = {56, 48, 40, 32, 24, 16, 8, 0};

  for (int start : ranges) {
    for (uint8_t square : util::range(start, start + 8)) {

      if (auto piece_at_sq = piece_at(b, square)) {
        std::string temp = "";
        if (cur_missing_count) {
          temp += std::to_string(cur_missing_count);
        }

        temp += piece_to_char(*piece_at_sq);
        fen += temp;

        cur_missing_count = 0;
      }
      else {
        cur_missing_count++;
      }
    }

    if (cur_missing_count) {
      fen += std::to_string(cur_missing_count);
      cur_missing_count = 0;
    }

    if (start != *(ranges.end() - 1)) {
      fen.append("/");
    }
  }

  fen.append(" ");

  fen += state.side_to_move == White ? 'w' : 'b';

  fen.append(" ");

  if (!state.castling_rights) {
    fen.append("-");
  }
  else {
    if (state.castling_rights & util::toul(CastlingRights::WhiteKingSide))
    {
      fen.append("K");
    }
    if (state.castling_rights & util::toul(CastlingRights::WhiteQueenSide))
    {
      fen.append("Q");
    }
    if (state.castling_rights & util::toul(CastlingRights::BlackKingSide))
    {
      fen.append("k");
    }
    if (state.castling_rights & util::toul(CastlingRights::BlackQueenSide))
    {
      fen.append("q");
    }
  }

  fen.append(" ");

  if (state.en_passant_target == chess::NoSquare) {
    fen.append("-");
  }
  else if (auto str = index_to_algebraic(state.en_passant_target)) {
    fen.append(*str);
  }

  fen.append(" ");

  fen.append(std::to_string(state.half_move_clock) + " " +
             std::to_string(state.full_move_count));

  return fen;
}
} // namespace chess::util::fen
