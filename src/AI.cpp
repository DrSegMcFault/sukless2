#include "AI.hxx"

#include <future>
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
int AI::calcPositionalScore(const BoardManager& b, Color side_to_move)
{
  int white_position_score = 0;
  int black_position_score = 0;

  for (auto square : util::range(NoSquare)) {
    if (auto piece = b.pieceAt(square)) {
      int score = positional_map[*piece][square];

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
int AI::evaluate(MoveResult last_move, const BoardManager& b, int depth)
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

  if (last_move == MoveResult::Checkmate) {
    return color() == side_to_move ? -100'000 * depth : 100'000 * depth;

  } else if (last_move == MoveResult::Stalemate) {
    return -10000;
  }

  return material_score + calcPositionalScore(b, side_to_move);
}

/******************************************************************************
 *
 * Method: AI::miniMax(BoardManager m, HashedMove, depth )
 *
 *****************************************************************************/
int AI::miniMax(MoveResult last, BoardManager& m, int alpha, int beta, int cur_depth, bool is_max)
{
  if (cur_depth == 0 ||
      last == MoveResult::Checkmate ||
      last == MoveResult::Stalemate)
  {
    return evaluate(last, m, cur_depth);
  }

  auto legal_moves = getLegalMoves(m);

  if (is_max) {
    int maxEval = std::numeric_limits<int>::min();
    // Loop through possible moves and apply them
    for (const auto& move : legal_moves) {

      // Apply the move
      BoardManager temp(_generator, m._board, m._state, legal_moves);

      auto&& [result, u] = temp.tryMove(move.toMove());

      maxEval = std::max(maxEval, miniMax(result, temp, alpha, beta, cur_depth - 1, false));
      alpha = std::max(alpha, maxEval);

      if (beta <= alpha) {
        break;
      }
    }
    return maxEval;
  }
  else {
    int minEval = std::numeric_limits<int>::max();
    for (const auto& move : legal_moves) {

      // Apply the move
      BoardManager temp(_generator, m._board, m._state, legal_moves);

      auto&& [result, u] = temp.tryMove(move.toMove());

      minEval = std::min(minEval, miniMax(result, temp, alpha, beta, cur_depth - 1, true));
      beta = std::min(beta, minEval);

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

  std::ranges::sort(legal_moves, [](auto& a, auto& b) {
    return static_cast<bool>(a.capture);
  });

  return legal_moves;
}

/******************************************************************************
 *
 * Method: AI::getBestMove()
 *
 *****************************************************************************/
std::optional<HashedMove> AI::getBestMove(const BoardManager& cpy)
{
  auto startTime = std::chrono::high_resolution_clock::now();

  auto legal_moves = getLegalMoves(cpy);
  std::vector<std::pair<HashedMove, int>> move_scores = {};

  if (legal_moves.size()) {
    constexpr size_t num_threads = 10;
    const size_t items_per_thread = legal_moves.size() / num_threads;

    auto process = [&] (int thread_num,
                        std::vector<HashedMove>::iterator begin,
                        std::vector<HashedMove>::iterator end)
    {
      std::vector<std::pair<HashedMove, int>> ret;
      auto roots_evaluated = 0;

      auto startTime = std::chrono::high_resolution_clock::now();

      for (auto it = begin; it != end; ++it) {
        BoardManager initial_board (_generator, cpy._board, cpy._state, legal_moves);
        auto&& [result, move_made] = initial_board.tryMove(it->toMove());
        ret.push_back({*it,
                       miniMax(result, initial_board,
                               std::numeric_limits<int>::min(),
                               std::numeric_limits<int>::max(), 5, false)});
        roots_evaluated++;
      }

      auto endTime = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

      qDebug() << "Thread " << thread_num << " executed in " << duration.count() << " msec(s).\n";
      qDebug() << "\t evaluated "  << roots_evaluated << " roots\n";
      return ret;
    };

    std::vector<std::future<std::vector<std::pair<HashedMove,int>>>> futures;
    for (auto i : util::range(num_threads)) {

      auto begin = std::begin(legal_moves) + i * items_per_thread;
      auto end = (i == num_threads - 1) ? std::end(legal_moves) : begin + items_per_thread;

      futures.emplace_back(std::async(std::launch::async, process, i, begin, end));
    }

    // Collect results from each thread
    for (auto& future : futures) {
      auto vec = future.get();
      move_scores.insert(move_scores.end(), vec.begin(), vec.end());
    }

    std::ranges::sort(move_scores, [&](const auto& a, const auto& b) {
      return a.second > b.second;
    });

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);

    qDebug() << "findBestMove executed in " << duration.count() << " second(s).\n";

    for (auto [m, score] : move_scores) {
      qDebug() << to_string(m) << " " << score;
    }

    return move_scores.front().first;
  }
  else {
    // was checkmate or stalemate
    return std::nullopt;
  }
}

} // namespace chess
