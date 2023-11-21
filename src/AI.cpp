#include "AI.hxx"
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
 * Method: AI::get_best_move()
 *
 *****************************************************************************/
Move AI::get_best_move() const
{
  auto&& [ board_copy, state_copy ] = _game->get_board_info();
  std::vector<util::bits::HashedMove> moves;
  moves.reserve(256);
  _generator->generate_moves(board_copy, state_copy, moves);
  for (auto& m : moves) {
    (void)m;
  }

  return {0,0};
}
