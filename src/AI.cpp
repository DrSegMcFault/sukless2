#include "AI.hxx"

#include <ranges>

using namespace chess;

/******************************************************************************
 *
 * Method: AI::AI()
 *
 *****************************************************************************/
AI::AI(const MoveGenerator* g, AIConfig cfg)
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

  _controlling_color = cfg.controlling;
  _cfg = cfg;
}

/******************************************************************************
 *
 * Method: AI::calc_material_score(const BoardManager&, Color c)
 *
 *****************************************************************************/
int AI::calcMaterialScore(const BoardManager& b, Color eval_for) const
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

  switch (eval_for) {
    case White:
      return white_eval - black_eval;

    case Black:
      return black_eval - white_eval;
  }
}

/******************************************************************************
 *
 * Method: AI::get_best_move()
 *
 *****************************************************************************/
std::optional<util::bits::HashedMove> AI::getBestMove(const BoardManager& cpy)
{
  std::vector<std::pair<util::bits::HashedMove, int>> legal_moves = {};

  for (const auto m : cpy._move_list) {

    BoardManager temp = cpy;
    Move to_try = {static_cast<uint8_t>(m.source),
                   static_cast<uint8_t>(m.target),
                   static_cast<Piece>(m.promoted)};

    if (auto&& [result, move] = temp.tryMove(to_try); result != MoveResult::Illegal)
    {
      legal_moves.push_back(std::make_pair(m, calcMaterialScore(temp, _controlling_color)));
    }
  }

  std::ranges::sort(legal_moves, [](const auto& m1, const auto& m2) {
    return m1.second > m2.second;
  });

  if (legal_moves.size()) {
    return legal_moves.front().first;
  }
  else {
    // was checkmate or stalemate
    return std::nullopt;
  }
}
