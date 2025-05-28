#include "headers/eventbus.h"
#include <QObject>

eventbus::eventbus(QObject *parent)
    : QObject{parent}
{
    currentPane = "";
    currentLyricindex = -1;
}
