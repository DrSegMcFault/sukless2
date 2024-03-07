#include "AI.hxx"

#include <ranges>
#include <QDebug>

namespace chess {

/******************************************************************************
 *
 * Method: AI::AI()
 *
 *****************************************************************************/
AI::AI(const MoveGenerator* g, AIConfig c)
  : _generator(g)
{
  _white_eval = 0;
  _black_eval = 0;
  _white_material_score = 0;
  _black_material_score = 0;

  switch (cfg.difficulty) {

    case AIDifficulty::Easy:
      _depth = 1;
      break;

    case AIDifficulty::Medium:
      _depth = 3;
      break;

    case AIDifficulty::Hard:
      _depth = 5;
      break;
  }

  cfg = c;
}

/******************************************************************************
 *
 * Method: AI::calcMaterialScore(const BoardManager&, Color c)
 *
 *****************************************************************************/
std::pair<int,int> AI::calcMaterialScore(const BoardManager& b) const
{
  int white_eval = 0;
  int black_eval = 0;

  for (auto p : chess::WhitePieces)
  {
    white_eval +=
        piece_values.at(p) * b.pieceCount(p);
  }

  for (auto p : chess::BlackPieces)
  {
    black_eval +=
        piece_values.at(p) * b.pieceCount(p);
  }

  return std::make_pair(white_eval, black_eval);
}

/******************************************************************************
 *
 * Method: AI::calcPositionalScore(const BoardManager&)
 *
 *****************************************************************************/
int AI::calcPositionalScore(const BoardManager& b, Color side_to_move) const
{
  int white_position_score = 0;
  int black_position_score = 0;

  for (auto square : util::range(NoSquare)) {
    if (auto piece = b.pieceAt(square)) {
      int score = positional_map.at(*piece)[square];

      if (*piece < BlackPawn) {
        white_position_score += score;
      } else {
        black_position_score += score;
      }
    }
  }

  return side_to_move == White ? white_position_score - black_position_score
                               : black_position_score - white_position_score;
}

/******************************************************************************
 *
 * Method: AI::evaluate(const BoardManager&, Color c)
 *
 *****************************************************************************/
int AI::evaluate(const BoardManager& b) const
{
  int evaluation = 0;
  int material_score = 0;

  auto side_to_move = b.getSideToMove();
  auto [white_mat, black_mat] = calcMaterialScore(b);

  switch (side_to_move) {
    case White:
      material_score = white_mat - black_mat;
      break;

    case Black:
      material_score = black_mat - white_mat;
      break;
  }

  return material_score + calcPositionalScore(b, side_to_move);
}

/******************************************************************************
 *
 * Method: AI::alphaBeta(BoardManager m, HashedMove, depth )
 *
 *****************************************************************************/
int AI::alphaBeta(BoardManager& m, int alpha, int beta, int cur_depth, bool is_max)
{
  if (cur_depth == 0) {
    return evaluate(m);
  }

  auto legal_moves = getLegalMoves(m);

  if (is_max) {
    int maxEval = std::numeric_limits<int>::min();
    // Loop through possible moves and apply them
    for (const auto& move : legal_moves) {

      // Apply the move
      BoardManager temp(_generator, m._board, m._state, legal_moves);

      auto&& [result, u] = temp.tryMove(move.toMove());

      int eval = alphaBeta(temp, alpha, beta, cur_depth -1, false);

      // Update the maximum evaluation
      maxEval = std::max(maxEval, eval);
      // Update alpha
      alpha = std::max(alpha, maxEval);

      // Perform alpha-beta pruning
      if (beta <= alpha) {
        break;
      }
    }
    return maxEval;
  }
  else {
    int minEval = std::numeric_limits<int>::max();
    // Loop through possible moves and apply them
    for (const auto& move : legal_moves) {

      // Apply the move
      BoardManager temp(_generator, m._board, m._state, legal_moves);

      auto&& [result, u] = temp.tryMove(move.toMove());

      int eval = alphaBeta(temp, alpha, beta, cur_depth - 1, true);

      // Update the minimum evaluation
      minEval = std::min(minEval, eval);
      // Update beta
      beta = std::min(beta, minEval);

      // Perform alpha-beta pruning
      if (beta <= alpha) {
        break;
      }
    }
    return minEval;
  }
}

/******************************************************************************
 *
 * Method: AI::getLegalMoves(const BoardManager& cpy)
 *
 *****************************************************************************/
std::vector<HashedMove> AI::getLegalMoves(const BoardManager& cpy)
{
  std::vector<HashedMove> legal_moves = {};

  for (const auto m : cpy._move_list) {

    BoardManager temp(_generator, cpy._board, cpy._state, cpy._move_list);

    if (auto&& [result, move] = temp.tryMove(m.toMove());
        result != MoveResult::Illegal)
    {
      legal_moves.push_back(m);
    }
  }

  return legal_moves;
}

/******************************************************************************
 *
 * Method: AI::getBestMove()
 *
 *****************************************************************************/
std::optional<HashedMove> AI::getBestMove(const BoardManager& cpy)
{
  auto legal_moves = getLegalMoves(cpy);
  std::vector<std::pair<HashedMove, int>> move_scores = {};

  if (legal_moves.size()) {
    for (const auto& move : legal_moves) {
      BoardManager initial_board (_generator, cpy._board, cpy._state, legal_moves);
      initial_board.tryMove(move.toMove());

      move_scores.push_back({move,
                             alphaBeta(initial_board, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 3, false)});
    }

    std::ranges::sort(move_scores, [&](const auto& a, const auto& b) {
      return a.second > b.second;
    });

    for (const auto& s : move_scores) {
       qDebug() << "Move: " << to_string(s.first) << " " << s.second;
    }

    return move_scores.front().first;
  }
  else {
    // was checkmate or stalemate
    return std::nullopt;
  }
}

} // namespace chess
