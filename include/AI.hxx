#pragma once

#include <memory>

#include "Game.hxx"

namespace chess {
  class AI
  {
    public:
      AI() = delete;

      AI(std::shared_ptr<Game> game, Color color, int depth)
        : _game(game), _depth(depth), _controlling_color(color) {}

      AI(const AI&) = delete;
      AI(AI&&) = delete;
      ~AI() = default;

    private:
      std::shared_ptr<Game> _game;
      int _depth;
      Color _controlling_color;
  };
}
