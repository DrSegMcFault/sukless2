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
  ret += (index % 8) + 'a';
  ret += (index / 8) + '1';

  return ret;
}

/*******************************************************************************
 *
 * Function: piece_from_char(const std::string& alg)
 *
 *******************************************************************************/
std::optional<chess::Piece> chess::util::fen::char_to_piece(char c) {
  using enum Piece;
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
 * Function: char_from_piece(Piece p)
 *
 *******************************************************************************/
char chess::util::fen::piece_to_char(Piece p)
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
 * Function: piece_at(const Board& b, uint8_t square )
 *
 *******************************************************************************/
std::optional<chess::Piece> chess::piece_at(const chess::Board& b, uint8_t square)
{
  for (auto p : chess::AllPieces) {
    if (is_set(square, b[p])) {
      return p;
    }
  }
  return std::nullopt;
}

/*******************************************************************************
 *
 * Function: to_array(const Board& b)
 *
 *******************************************************************************/
std::array<std::optional<chess::Piece>, 64> chess::to_array(const chess::Board& b)
{
  std::array<std::optional<chess::Piece>, 64> board;
  for (auto i : chess::util::range(NoSquare)) {
    board[i] = chess::piece_at(b, i);
  }
  return board;
}

/*******************************************************************************
 *
 * Function: util::fen::generate(const Board& b, uint8_t square )
 *
 *******************************************************************************/
std::string chess::util::fen::generate(const chess::Board& b, const State& state) {
  std::string fen = "";
  fen.reserve(40);

  uint8_t cur_missing_count = 0;
  static constexpr auto ranges = {56, 48, 40, 32, 24, 16, 8, 0};

  for (int start : ranges) {
    for (uint8_t square : std::views::iota(start, start + 8)) {

      if (auto piece_at_sq = chess::piece_at(b, square)) {
        std::string temp = "";
        if (cur_missing_count) {
          temp += std::to_string(cur_missing_count);
        }

        temp += util::fen::piece_to_char(*piece_at_sq);
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
  else if (auto str = util::fen::index_to_algebraic(state.en_passant_target)) {
    fen.append(*str);
  }

  fen.append(" ");

  fen.append(std::to_string(state.half_move_clock) + " " +
             std::to_string(state.full_move_count));

  return fen;
}
