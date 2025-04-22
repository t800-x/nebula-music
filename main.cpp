#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "player.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    player *mediaplayer = new player(nullptr);
    QQmlApplicationEngine engine;


    qmlRegisterSingletonInstance<player>("Nebula.Media", 1, 0, "MediaPlayer", mediaplayer);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("nebula-music", "Main");

    return app.exec();
}
