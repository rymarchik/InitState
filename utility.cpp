#include <utility.h>

Utility::Utility()
{

}

void Utility::initializeTableSettings(QTableWidget* table) {
    table->horizontalHeader()->setFrameShape(QFrame::StyledPanel);
    table->horizontalHeader()->setFrameShadow(QFrame::Raised);
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setShowGrid(false);
}

void Utility::closeNewEditTab(QTabWidget* tabwgt) {
    for (int i = tabwgt->count() - 1; i >= 0; i--) {
        if (tabwgt->tabText(i) == "Новый" || tabwgt->tabText(i).contains("Цель")) {
            tabwgt->removeTab(i);
            return;
        }
    }
}

void Utility::clearLayout(QLayout* layout) {
    while (QLayoutItem* item = layout->takeAt(0))
    {
        if (QWidget* widget = item->widget())
            delete widget;
        if (QLayout* childLayout = item->layout())
            clearLayout(childLayout);
        delete item;
    }
}

QList<double> Utility::convertCoordToDecimalFormat(QString coord) {
    QString latitude = coord.split("     ").first();
    QString longitude = coord.split("     ").last();

    QRegExp regex("[°'\"\\s]+");
    QStringList latitudeParts = latitude.split(regex);
    QStringList longitudeParts = longitude.split(regex);
    double latitudeParsed = latitudeParts.at(0).toDouble() +
            (latitudeParts.at(1).toDouble() + latitudeParts.at(2).toDouble() / 60) / 60;
    double longitudeParsed = longitudeParts.at(0).toDouble() +
            (longitudeParts.at(1).toDouble() + longitudeParts.at(2).toDouble() / 60) / 60;

    QList<double> list;
    list << latitudeParsed << longitudeParsed;
    return list;
}

QString Utility::convertCoordToDegreeFormat(double coord) {
    int degrees = floor(coord);
    int minutes = floor((coord - degrees) * 60);
    QString seconds = QString::number(((coord - degrees) * 60 - minutes) * 60, 'f', 2);

    if (seconds.contains("60")) {
        seconds = "0.00";
        minutes++;
        if (minutes == 60) {
            minutes = 0;
            degrees++;
        }
    }
    qDebug() << degrees << "° " << minutes << "' " << seconds.toDouble() << "\"";
    return QString::number(degrees) + " " + QString::number(minutes) + " " + seconds;
}

QString Utility::convertCodeToReferenceName(QSqlDatabase db, QString code)
{
    QSqlQuery query = QSqlQuery(db);
    QString selectQuery = "SELECT termname "
                          "FROM reference_data.terms "
                          "WHERE termhierarchy = ?";
    query.prepare(selectQuery);
    query.addBindValue(code);
    query.exec();
    query.next();
    return query.value(0).toString();
}

QString Utility::convertReferenceNameTOCode(QSqlDatabase db, QString referenceName)
{
    QSqlQuery query = QSqlQuery(db);
    QString selectQuery = "SELECT termhierarchy "
                          "FROM reference_data.terms "
                          "WHERE termname = ?";
    query.prepare(selectQuery);
    query.addBindValue(referenceName);
    query.exec();
    query.next();
    return query.value(0).toString();
}

int Utility::getTid(QSqlDatabase db) {
    QSqlQuery query = QSqlQuery(db);
    QString selectQuery = "SELECT txid_current()";
    query.prepare(selectQuery);
    query.exec();
    query.next();
    return query.value(0).toInt();
}
