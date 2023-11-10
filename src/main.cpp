#include "BoardManager.hxx"
#include "Game.hxx"
#include "App.hxx"
#include <iostream>

using namespace chess;

int main() {
  std::cout << "Starting chess engine...\n";
  App app;
  app.run();

  return 0;
}
