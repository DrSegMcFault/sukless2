#include <iostream>
#include "App.hxx"

using namespace chess;

int main() {
  std::cout << "Starting chess engine...\n";
  App app;
  app.run();

  return 0;
}
