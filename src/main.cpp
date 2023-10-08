#include <iostream>
#include "BoardManager.hxx"
#include "BitBoard.hxx"
#include "Game.hxx"


int main() {
  chess::Game g;
  g.is_move_pseudo_legal(chess::Move {12, 20, chess::Piece::w_pawn, chess::Color::white});
  return 0;
}