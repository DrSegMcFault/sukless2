#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "BoardModel.h"
#include "MoveGen.hxx"

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  QQmlApplicationEngine engine;

  // Register the CustomListModel with QML
  qmlRegisterType<BoardModel>("BoardModel", 1, 0, "BoardModel");

  const QUrl url(u"qrc:/chess2/qml/Main.qml"_qs);
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
      &app, []() { QCoreApplication::exit(-1); },
      Qt::QueuedConnection);
  engine.load(url);

  // make members of MoveGen public and add to this to see if everything truly
  // is getting initialized at compile time
  // constexpr MoveGen g;
  // static_assert(g.knight_attacks[1] != 0);
  // static_assert(g.knight_attacks[2] != 0);
  // static_assert(g.knight_attacks[3] != 0);
  // static_assert(g.knight_attacks[4] != 0);
  // static_assert(g.knight_attacks[5] != 0);
  // static_assert(g.knight_attacks[6] != 0);
  // static_assert(g.knight_attacks[7] != 0);
  // static_assert(g.knight_attacks[8] != 0);

  // static_assert(g.pawn_attacks[0][0] != 0);
  // static_assert(g.pawn_attacks[0][1] != 0);
  // static_assert(g.pawn_attacks[0][2] != 0);
  // static_assert(g.pawn_attacks[0][3] != 0);
  // static_assert(g.pawn_attacks[0][4] != 0);
  // static_assert(g.pawn_attacks[0][5] != 0);
  // static_assert(g.pawn_attacks[0][6] != 0);
  // static_assert(g.pawn_attacks[0][7] != 0);

  // static_assert(g.king_attacks[1] != 0);
  // static_assert(g.king_attacks[2] != 0);
  // static_assert(g.king_attacks[3] != 0);
  // static_assert(g.king_attacks[4] != 0);
  // static_assert(g.king_attacks[5] != 0);
  // static_assert(g.king_attacks[6] != 0);
  // static_assert(g.king_attacks[7] != 0);
  // static_assert(g.king_attacks[8] != 0);

  // static_assert(g.bishop_masks[1] != 0);
  // static_assert(g.bishop_masks[2] != 0);
  // static_assert(g.bishop_masks[3] != 0);
  // static_assert(g.bishop_masks[4] != 0);
  // static_assert(g.bishop_masks[5] != 0);
  // static_assert(g.bishop_masks[6] != 0);
  // static_assert(g.bishop_masks[7] != 0);
  // static_assert(g.bishop_masks[8] != 0);

  // static_assert(g.rook_masks[1] != 0);
  // static_assert(g.rook_masks[2] != 0);
  // static_assert(g.rook_masks[3] != 0);
  // static_assert(g.rook_masks[4] != 0);
  // static_assert(g.rook_masks[5] != 0);
  // static_assert(g.rook_masks[6] != 0);
  // static_assert(g.rook_masks[7] != 0);
  // static_assert(g.rook_masks[8] != 0);

  return app.exec();
}
