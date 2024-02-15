#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  QQmlApplicationEngine engine;

  // Register the CustomListModel with QML
  // qmlRegisterType<BoardModel>("BoardModel", 1, 0, "BoardModel");
  // qmlRegisterType<Game>("Game", 1, 0, "Game");

  const QUrl url(u"qrc:/qml/Main.qml"_qs);
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
      &app, []() { QCoreApplication::exit(-1); },
      Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}
