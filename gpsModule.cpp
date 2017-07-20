#include "gpsModule.h"
#include "QMessageBox"

GPSModule::GPSModule(QSqlDatabase db, QObject *parent) :
    QObject(parent),
    db(db)
{
    if (QSerialPortInfo::availablePorts().size() != 0) {
        serial = new QSerialPort(this);
        readPortTimer = new QTimer;

        serial->setPortName(QSerialPortInfo::availablePorts().at(0).portName());
        serial->setBaudRate(QSerialPort::Baud115200);
        serial->setDataBits(QSerialPort::Data8);
        serial->setStopBits(QSerialPort::OneStop);
        if (serial->open(QIODevice::ReadOnly)) {
            readPortTimer->start(5000);
        } else {
            QMessageBox::critical(0, tr("Error"), serial->errorString());
        }

        connect(readPortTimer, SIGNAL(timeout()), this, SLOT(slotParseInput()));
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
    double latitude;
    double longitude;
    double altitude;

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

    QString updateRow = "UPDATE own_forces.combatobject_location "
                        "SET obj_location = ST_SetSRID(ST_MakePoint(?, ?, ?), 4326), "
                        "       date_edit = ? "
                        "WHERE combat_hierarchy = ? "
//                        "       AND date_delete is null "
                        "       AND tid = ?";
    query.prepare(updateRow);
    query.addBindValue(longitude);
    query.addBindValue(latitude);
    query.addBindValue(altitude);
    query.addBindValue(dateTime);
    query.addBindValue("1.11"); //значение взято для теста
    query.addBindValue(10341); //значение взято для теста
    if (!query.exec()) {
        qDebug() << query.lastError();
    }

//    qDebug() << "longitude: " << longitude;
//    qDebug() << "latitude: " << latitude;
//    qDebug() << "altitude: " << altitude;
//    qDebug() << "utc time: " << dateTime.toString("hh:mm:ss dd-MMM-yyyy");
//    qDebug() << "local time: " << dateTime.toLocalTime().toString("hh:mm:ss dd-MMM-yyyy") << endl;
}
