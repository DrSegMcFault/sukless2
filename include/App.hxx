#pragma once

#include <memory>
#include <unordered_map>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "Game.hxx"

using namespace chess;
class App {
  public:
    App();
    ~App();

    void run();

  private:
    const int _screenW = 720;
    const int _screenH = 720;

    SDL_Window* _window;
    SDL_Renderer* _renderer;
    SDL_Texture* _circleTexture;
    Mix_Chunk* _move_sound;
    Mix_Chunk* _win_sound;
    Mix_Chunk* _lose_sound;
    std::unordered_map<Piece, SDL_Texture*> p_textures;
    std::array<std::optional<Piece>, 64> _current_board;

    std::vector<int> _possible_moves;

    enum AppState {
      PLAY = 0,
      EXIT = 1
    };

    AppState _state;

    std::shared_ptr<Game> _game;

    SDL_Texture* load_texture(const char* filepath);

    void display();
    void display_possible_moves();
    void render_background();
    void render_piece(SDL_Texture* txture, int square);
    void render_all_pieces();
};
