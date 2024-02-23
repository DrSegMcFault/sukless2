#pragma once

#include <QObject>
#include <QApplication>
#include <QQmlApplicationEngine>
#include "Game.h"
#include "AIRunner.h"

class App : public QApplication
{
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

private:
  inline static App* _instance = nullptr;

public:
  App() = delete;

  static App* create(QQmlEngine*, QJSEngine*) {
    QJSEngine::setObjectOwnership(_instance, QJSEngine::CppOwnership);
    return _instance;
  }

  static auto& instance() { return *_instance; }

  App(int argc, char* argv[]);

private:
  QPointer<QQmlApplicationEngine> _engine;
  Game _game;
  AIRunner _ai_thread;

};
