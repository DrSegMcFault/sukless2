#include "App.hxx"
#include <optional>
#include <iostream>
#include <cassert>

/******************************************************************************
 *
 * Method: App::App()
 *
 *****************************************************************************/
App::App() 
{

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 );

  _window = SDL_CreateWindow("Chess",
              SDL_WINDOWPOS_CENTERED,
              SDL_WINDOWPOS_CENTERED,
              _screenW,
              _screenH,
              SDL_WINDOW_SHOWN);


  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
  auto* surface = IMG_Load("resources/circle.png");

  if (surface) {
    _circleTexture = SDL_CreateTextureFromSurface(_renderer, surface);
  }

  if (!_window || !_renderer ) {
    assert(false);
  }

  _move_sound = Mix_LoadWAV("resources/place_down.wav"); 

  // something is not working right with the win sound
  _win_sound = Mix_LoadWAV("resources/game_win.wav");
  _lose_sound = Mix_LoadWAV("resources/game_lose.wav");

  p_textures.insert( { Piece::w_pawn,   load_texture("resources/pawn_white.png") } );
  p_textures.insert( { Piece::b_pawn,   load_texture("resources/pawn_black.png") } );
  p_textures.insert( { Piece::w_king,   load_texture("resources/king_white.png") } );
  p_textures.insert( { Piece::b_king,   load_texture("resources/king_black.png") } );
  p_textures.insert( { Piece::w_queen,  load_texture("resources/queen_white.png") } );
  p_textures.insert( { Piece::b_queen,  load_texture("resources/queen_black.png") } );
  p_textures.insert( { Piece::w_rook,   load_texture("resources/rook_white.png") } );
  p_textures.insert( { Piece::b_rook,   load_texture("resources/rook_black.png") } );
  p_textures.insert( { Piece::w_bishop, load_texture("resources/bishop_white.png") } );
  p_textures.insert( { Piece::b_bishop, load_texture("resources/bishop_black.png") } );
  p_textures.insert( { Piece::w_knight, load_texture("resources/knight_white.png") } );
  p_textures.insert( { Piece::b_knight, load_texture("resources/knight_black.png") } );

  _game = std::make_shared<Game>(); 
  _current_board = _game->get_current_board();
  _possible_moves.reserve(55);
}

/******************************************************************************
 *
 * Method: App::run()
 *
 *****************************************************************************/
void App::run()
{
  // cached piece that has been previously clicked
  std::optional<uint8_t> clicked;

  // is_first_frame
  bool first = true;

  auto game_over = [&](auto sound) {
  // check for checkmate
    Mix_PlayChannel(1, sound, 0);
    SDL_Delay(2000);
    first = true;
    Mix_PlayChannel(0, _move_sound, 0);
  };

  auto handle_move = [&](chess::Move move) {
    auto result = _game->try_move(move);
    if (result != MoveResult::Illegal) {
      _current_board = _game->get_current_board();
    }

    switch (result) {
      case MoveResult::Success:
      {
        Mix_PlayChannel( 0, _move_sound, 0 );
        _possible_moves.clear();
        break;
      }

      case MoveResult::Illegal:
      {
        break;
      }

      case MoveResult::Checkmate:
      {
        game_over(_win_sound);
        break;
      }

      case MoveResult::Draw:
      {
        game_over(_lose_sound);
        break;
      }

      default:
      {
        break;
      }
    }

    clicked.reset();
    display();
    return result;
  };

  // main loop
  // we only render if there is a click on the screen for performance
  while (_state != AppState::EXIT) {

    // render the first frame
    if (first) {
      display();
      first = false;
    }

    SDL_Event ev;
    
    SDL_WaitEvent(&ev);

    switch (ev.type) {
      case SDL_QUIT:
      {
        _state = AppState::EXIT;
        break;
      }

      case SDL_MOUSEBUTTONDOWN:
      {
        display();
        auto x = ev.button.x;
        auto y = ev.button.y;
        int grid_x = floor(x / (_screenW / 8));
        int grid_y = floor((_screenH - y) / (_screenH / 8));
        uint8_t square = grid_x + 8 * grid_y;

        // get the piece that was clicked on
        if (!clicked) {
          if (auto moves = _game->get_pseudo_legal_moves(square);
              moves.size())
          {
            clicked.emplace(square);
            _possible_moves = moves;
            display_possible_moves();
          }
        } else {
          handle_move({ clicked.value(), square });
        }
        break;
      }

      case SDL_KEYDOWN:
      {
        switch (ev.key.keysym.sym) {
          case SDLK_q:
          {
            _state = AppState::EXIT;
            break;
          }
        }

        break;
      }

      default:
        break;
    }
  }
}

/******************************************************************************
 *
 * Method: App::display()
 * 
 * - render and display the background and pieces 
 *****************************************************************************/
void App::display()
{
   SDL_RenderClear(_renderer);

   render_background();
   render_all_pieces();

   SDL_RenderPresent(_renderer);
}

/******************************************************************************
 *
 * Method: App::render_all_pieces()
 * 
 *****************************************************************************/
void App::render_all_pieces() {
  int index = 0;
  for (auto piece : _current_board) {
    if (piece) {
      render_piece(p_textures.at(piece.value()), index);
    }
    index++;
  }
}

/******************************************************************************
 *
 * Method: App::display_possible_moves()
 * 
 *****************************************************************************/
void App::display_possible_moves()
{
  SDL_RenderClear(_renderer);

  render_background();
  render_all_pieces();

  SDL_Rect src = { 0, 0, 30, 30 };
 
  for (auto move : _possible_moves) {

    SDL_Rect dest = {
      (move % 8) * (_screenW / 8) + 30,
      _screenH - ((move / 8 + 1) * (_screenH / 8)) + 30, 
      30,
      30
    };

    SDL_RenderCopy(_renderer, _circleTexture, &src, &dest);
  }

  SDL_RenderPresent(_renderer);
}

/******************************************************************************
 *
 * Method: App::render_background()
 * 
 *****************************************************************************/
void App::render_background()
{
  bool white = true;
  SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (white) {
        SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
      } else {
        SDL_SetRenderDrawColor(_renderer, 83, 132, 172, 255);
      }

      white = !white;
      SDL_Rect rectangle = { i * _screenW / 8,
                             j * _screenH / 8,
                             _screenW / 8,
                             _screenH / 8 };
      SDL_RenderFillRect(_renderer, &rectangle);
    }

    white = !white;
  }
}

/******************************************************************************
 *
 * Method: App::render_piece(*texture, int square)
 *
 *****************************************************************************/
void App::render_piece(SDL_Texture *texture, int square)
{
  SDL_Rect src = {0, 0, 80, 80};
  SDL_Rect dest = { (square % 8) * (_screenW / 8) + 5,
                    _screenH - ((square / 8 + 1) * (_screenH / 8)) + 5,
                    _screenW / 8 - 10,
                    _screenH / 8 - 10};
  SDL_RenderCopy(_renderer, texture, &src, &dest);
}

/******************************************************************************
 *
 * Method: App::load_texture()
 *
 *****************************************************************************/
SDL_Texture* App::load_texture(const char* filepath)
{
  auto* img = IMG_Load(filepath);

  if (!img) {
    assert(false);
  }
  SDL_Texture* txt = SDL_CreateTextureFromSurface(_renderer, img);

  return txt;
}

/******************************************************************************
 *
 * Method: App::~App()
 *
 *****************************************************************************/
App::~App()
{
  SDL_DestroyRenderer(_renderer);
  SDL_DestroyWindow(_window);
  Mix_FreeChunk(_move_sound);
  Mix_FreeChunk(_win_sound);
  IMG_Quit();
  Mix_Quit();
  SDL_Quit();
}
