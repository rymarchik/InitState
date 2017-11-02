#include "gpsModule.h"
#include <QMessageBox>

GPSModule::GPSModule(QSqlDatabase db, QString combatHierarchy, QObject *parent) :
    db(db),
    combatHierarchy(combatHierarchy),
    QObject(parent)
{
    serial = new QSerialPort(this);
    readPortTimer = new QTimer;
    connect(readPortTimer, SIGNAL(timeout()), this, SLOT(slotParseInput()));
}

void GPSModule::setPortName(QString portName) {
    serial->setPortName(portName);
}

void GPSModule::openSerialPort() {
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setStopBits(QSerialPort::OneStop);
    if (serial->open(QIODevice::ReadOnly)) {
        readPortTimer->start(3000);
    } else {
        QMessageBox::critical(0, tr("Error"), serial->errorString());
    }
}

void GPSModule::closeSerialPort() {
    if (serial->isOpen()) {
        serial->close();
        readPortTimer->stop();
    }
}

void GPSModule::slotParseInput() {
    QRegExp ggaRegExp("(GP|GN|GL)GGA", Qt::CaseInsensitive);
    QRegExp rmcRegExp("(GP|GN|GL)RMC", Qt::CaseInsensitive);
    QRegExp timeRegExp("(\\d{6})(\\.\\d+)", Qt::CaseInsensitive);
    QRegExp latRegExp("(\\d{2})(\\d{2}\\.\\d+)", Qt::CaseInsensitive);
    QRegExp lonRegExp("(\\d{3})(\\d{2}\\.\\d+)", Qt::CaseInsensitive);
    QSqlQuery query = QSqlQuery(db);
    QDateTime dateTime;
    double latitude = 0.0;
    double longitude = 0.0;
    double altitude = 0.0;

    QString line = serial->readAll();
    QStringList lines = line.split('$');

    foreach (line, lines) {
        QStringList fields = line.split(",");

        if (fields[0].contains(ggaRegExp)) {
            // GPS Quality field cannot be NULL
            if (fields[6].isEmpty())
                continue;
            if (fields[1].contains(timeRegExp)) {
                QString time = timeRegExp.cap(1);
                dateTime.setTime(QTime::fromString(time, "hhmmss"));
                dateTime.setTimeSpec(Qt::UTC);
            }
            if (fields[2].contains(latRegExp)) {
                int deg = latRegExp.cap(1).toInt();
                double min = latRegExp.cap(2).toDouble();
                latitude = deg+min/60.0;
            }
            if(fields[3] == "S") {
                latitude *= -1;
            }
            if(fields[4].contains(lonRegExp)) {
                int deg = lonRegExp.cap(1).toInt();
                double min = lonRegExp.cap(2).toDouble();
                longitude = deg+min/60.0;
            }
            if(fields[5] == "W") {
                longitude *= -1;
            }
            altitude = fields[9].toDouble();
        }
        else if (fields[0].contains(rmcRegExp)) {
            QString date = fields[9];
            dateTime.setDate(QDate::fromString(date, "ddMMyy"));
            dateTime = dateTime.addYears(100);
        }
    }

    if (latitude != 0 && longitude != 0 && altitude !=0) {
        updateDatabaseGeoInfo(latitude, longitude, altitude);
    }

//    qDebug() << "longitude: " << longitude;
//    qDebug() << "latitude: " << latitude;
//    qDebug() << "altitude: " << altitude;
//    qDebug() << "utc time: " << dateTime.toString("hh:mm:ss dd-MMM-yyyy");
//    qDebug() << "local time: " << dateTime.toLocalTime().toString("hh:mm:ss dd-MMM-yyyy") << endl;
}

void GPSModule::updateDatabaseGeoInfo(double latitude, double longitude, double altitude) {
    QSqlQuery query(db);
    db.transaction();
    QString updateRow = "UPDATE own_forces.combatobject_location "
                        "SET date_delete = now() "
                        "WHERE combat_hierarchy = ? "
                        "AND date_add = (SELECT MAX(date_add) FROM own_forces.combatobject_location)";
    query.prepare(updateRow);
    query.addBindValue(combatHierarchy);
    if (!query.exec()) {
        qDebug() << query.lastError();
    }

    QString insertRow = "INSERT INTO own_forces.combatobject_location (combat_hierarchy, obj_location, "
                        "       direction, tid, date_add, id_manager)"
                        "VALUES (?, ST_SetSRID(ST_MakePoint(?, ?, ?), 4326), ?, txid_current(), now(), ?)";
    query.prepare(insertRow);
    query.addBindValue(combatHierarchy);
    query.addBindValue(latitude);
    query.addBindValue(longitude);
    query.addBindValue(altitude);
    query.addBindValue(getLastDirectionValue());
    query.addBindValue(10); //костыль
    if (!query.exec()) {
        qDebug() << query.lastError();
    }
    db.commit();
}

int GPSModule::getLastDirectionValue() {
    QSqlQuery query(db);
    QString selectLastDirection = "SELECT direction "
                                  "FROM own_forces.combatobject_location "
                                  "WHERE combat_hierarchy = ? "
                                  "     AND tid = (SELECT MAX(tid) FROM own_forces.combatobject_location)";
    query.prepare(selectLastDirection);
    query.addBindValue(combatHierarchy);
    if (!query.exec()) {
        qDebug() << query.lastError();
    }
    query.next();
    int direction = query.value(0).toInt();
    return direction;
}
