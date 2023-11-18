#include "AI.hxx"
using namespace chess;

/******************************************************************************
 *
 * Method: AI::AI()
 *
 *****************************************************************************/
AI::AI(std::shared_ptr<Game> game, Color color, int depth)
  : _game(game), _depth(depth), _controlling_color(color)
{
  _white_eval = 0;
  _black_eval = 0;
  _white_material_score = 0;
  _black_material_score = 0;
}

/******************************************************************************
 *
 * Method: AI::get_best_move()
 *
 *****************************************************************************/
Move AI::get_best_move() const
{
  return {0,0};
}
