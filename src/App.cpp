#include "App.h"

/******************************************************************************
 *
 * Method: App()
 *
 *****************************************************************************/
App::App(int argc, char* argv[])
  : QApplication(argc, argv)
  , _engine(new QQmlApplicationEngine())
  , _game()
  , _ai_thread()
{
  connect(this, &App::aboutToQuit, this,
    [&]()
  {
    if (_engine) {
      _engine->deleteLater();
    }
  });

  connect(_engine, &QQmlEngine::quit, this, &App::quit);

  // setup communication between the game and the ai thread
  connect(&_game, &Game::moveConfirmed, &_ai_thread, &AIRunner::onMoveConfirmed);
  connect(&_game, &Game::gameStart, &_ai_thread, &AIRunner::onGameStart);
  connect(&_ai_thread, &AIRunner::moveReady, &_game, &Game::onMoveReady);
  connect(&_ai_thread, &AIRunner::suggestionReady, &_game, &Game::onSuggestionReady);

  auto context = _engine->rootContext();
  context->setContextProperty("app", QVariant::fromValue(this));
  context->setContextProperty("game", QVariant::fromValue(&_game));

  _engine->load("qrc:/qml/Main.qml");
}
