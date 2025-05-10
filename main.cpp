#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "player.h"
#include "database.h"
#include "queuemodel.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    player *mediaplayer = new player(nullptr);
    database *db = new database(nullptr);
    queuemodel *model = new queuemodel(mediaplayer);

    QQmlApplicationEngine engine;

    db->init();
    mediaplayer->init(model);
    //db->add_to_library("D:/Songs/Vampire");

    qmlRegisterSingletonInstance<player>("Nebula.Media", 1, 0, "MediaPlayer", mediaplayer);
    qmlRegisterSingletonInstance<database>("Nebula.Database", 1, 0, "Keeper", db);
    qmlRegisterSingletonInstance<queuemodel>("Nebula.QueueModel", 1, 0, "Model", model);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("nebula-music", "Main");

    return app.exec();
}
