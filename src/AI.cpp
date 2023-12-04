#include "AI.hxx"
#include <QDebug>

using namespace chess;

/******************************************************************************
 *
 * Method: AI::AI()
 *
 *****************************************************************************/
AI::AI(Game* game, std::shared_ptr<MoveGen> g, AIConfig cfg)
  : _game(game), _generator(g)
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
}

/******************************************************************************
 *
 * Method: AI::evaluate(const Board&, const State&)
 *
 *****************************************************************************/
int AI::evaluate(const Board& b, const State& s)
{
  return 0;
}

/******************************************************************************
 *
 * Method: AI::get_best_move()
 *
 *****************************************************************************/
std::optional<util::bits::HashedMove> AI::get_best_move()
{
  BoardManager b_copy = _game->get_board_copy();

  std::vector<util::bits::HashedMove> legal_moves = {};
  for (auto m : b_copy._move_list) {
    BoardManager temp(b_copy._generator,
                      b_copy._board,
                      b_copy._state,
                      b_copy._history);

    if (temp.try_move(m) != MoveResult::Illegal) {
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
