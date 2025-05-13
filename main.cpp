#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "player.h"
#include "database.h"
#include "songmodel.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    songmodel main_table = new songmodel(nullptr);

    player *mediaplayer = new player(nullptr);
    database *db = new database(nullptr);

    QQmlApplicationEngine engine;

    db->init();
    songmodel *queuemodel = new songmodel(mediaplayer);
    mediaplayer->init(queuemodel);
    db->add_to_library("D:/Songs/Vampire");


    qmlRegisterSingletonInstance<player>("Nebula.Media", 1, 0, "MediaPlayer", mediaplayer);
    qmlRegisterSingletonInstance<database>("Nebula.Database", 1, 0, "Keeper", db);
    qmlRegisterSingletonInstance<songmodel>("Nebula.SongModel", 1, 0, "Model", queuemodel);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("nebula-music", "Main");

    return app.exec();
}
