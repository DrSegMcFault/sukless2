#include "AI.hxx"

using namespace chess;

/******************************************************************************
 *
 * Method: AI::AI()
 *
 *****************************************************************************/
AI::AI(std::shared_ptr<MoveGen> g, AIConfig cfg)
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
 * Method: AI::evaluate(const BoardManager&)
 *
 *****************************************************************************/
int AI::evaluate(const BoardManager& b)
{
  return 0;
}

/******************************************************************************
 *
 * Method: AI::get_best_move()
 *
 *****************************************************************************/
std::optional<util::bits::HashedMove> AI::get_best_move(const BoardManager& cpy)
{
  std::vector<util::bits::HashedMove> legal_moves = {};

  for (auto m : cpy._move_list) {
    BoardManager temp = cpy;

    if (temp.try_move({static_cast<uint8_t>(m.source),static_cast<uint8_t>(m.target)}) != MoveResult::Illegal) {
      legal_moves.push_back(m);
    }
  }

  if (legal_moves.size()) {
    return legal_moves[rand() % legal_moves.size()];
  }
  else {
    // was checkmate or stalemate
    return std::nullopt;
  }
}
