#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "headers/player.h"
#include "headers/database.h"
#include "models/songmodel.h"
#include "headers/eventbus.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    player *mediaplayer = new player();
    database *db = new database();
    songmodel* queuemodel = new songmodel();
    eventbus* bus = new eventbus();

    QQmlApplicationEngine engine;

    db->init();
    mediaplayer->init(queuemodel);
    db->add_to_library("D:/Songs/The Music");


    qmlRegisterSingletonInstance<player>("Nebula.Media", 1, 0, "MediaPlayer", mediaplayer);
    qmlRegisterSingletonInstance<database>("Nebula.Database", 1, 0, "Keeper", db);
    qmlRegisterSingletonInstance<songmodel>("Nebula.SongModel", 1, 0, "Model", queuemodel);
    qmlRegisterSingletonInstance<eventbus>("Nebula.Events", 1, 0, "EventBus", bus);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("nebula-music", "Main");

    return app.exec();
}
