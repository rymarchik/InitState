#ifndef UTILITY_H
#define UTILITY_H

#include <QtWidgets>

class Utility {

public:
    Utility();

    static void initializeTableSettings(QTableWidget*);
    static void closeNewEditTab(QTabWidget*);
    static void clearLayout(QLayout*);
    static QList<double> convertCoordToDecimalFormat(QString);
    static QString convertCoordToDegreeFormat(double);
};

#endif // UTILITY_H
