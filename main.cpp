#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "player.h"
#include "database.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    player *mediaplayer = new player(nullptr);
    database *db = new database(nullptr);

    QQmlApplicationEngine engine;

    db->init();
    //db->add_to_library("D:/Songs/Vampire");

    qmlRegisterSingletonInstance<player>("Nebula.Media", 1, 0, "MediaPlayer", mediaplayer);
    qmlRegisterSingletonInstance("Nebula.Database", 1, 0, "Keeper", db);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("nebula-music", "Main");

    return app.exec();
}
