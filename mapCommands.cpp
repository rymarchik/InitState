#include "mapCommands.h"

MapCommands::MapCommands(QSqlDatabase db, QObject *parent) :
    QObject(parent),
    db(db)
{

}
