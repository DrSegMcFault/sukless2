#include "Game.hxx"

namespace chess {
  class AI
  {
    public:
      AI() = delete;
      AI(Game* game, int depth) : _game(game), _depth(depth)  {}
      AI(const AI&) = delete;
      AI(AI&&) = delete;
      ~AI() = default;

    private:
      Game* _game;
      int _depth;
  };
}
