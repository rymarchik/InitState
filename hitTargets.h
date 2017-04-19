#ifndef HITTARGETS_H
#define HITTARGETS_H

#include <QtWidgets>
#include <QtSql>
//#include <QtUiTools>
#include "basetoolclass.h"
#include "hittargetstabform.h"

class HitTargets : public BaseToolClass
{
    Q_OBJECT

public:
    HitTargets(QSqlDatabase db, QTableWidget *navigatorTable,
               QTableWidget *navigatorReceiversTable, QTableWidget *changesTable,
               QTableWidget *changesReceiversTable, QWidget* parent = 0);
    void fillNavigator();
    void fillChanges();
    QWidget *onAdd();
    QWidget *onEdit();
    bool onDelete();
    bool onSave();

//    QWidget* loadUiFile();
//    void reinitializeFormData();
//    void setCoordinateLE(QString);
//    void addFilledPoints();
//    QString constructMessage();
//    QStringList getDataSourceBatteries();
//    QStringList getDataSourceWeaponry();
//    QStringList getHitTargets();
//    QStringList getCoverDegrees();
//    QStringList getDamageDegrees();
//    QStringList getRocketTypes();

private:
    QTableWidget* navigatorTable;
    QTableWidget* changesTable;
//    QWidget* contentWidget;

//    QComboBox* dataSourceBatteryCB;
//    QComboBox* dataSourceWeaponryCB;
//    QLineEdit* targetNumberLE;
//    QComboBox* targetNameCB;
//    QLineEdit* importanceLE;
//    QDateTimeEdit* detectionTimeDTE;

//    QRadioButton* randomRB;
//    QRadioButton* squareRB;
//    QRadioButton* roundRB;
//    QVBoxLayout* extraCoordinatesLayout;
//    QLineEdit* coordinateLE;
//    QLineEdit* heightLE;
//    QPushButton* addPointBtn;
//    QPushButton* removePointBtn;
//    QLabel* depthLbl;
//    QLabel* frontLbl;
//    QLabel* deviationLbl;
//    QLabel* radiusLbl;
//    QLineEdit* depthLE;
//    QLineEdit* frontLE;
//    QLineEdit* deviationLE;
//    QLineEdit* radiusLE;
//    QComboBox* coverDegreeCB;

//    QCheckBox* launchChB;
//    QCheckBox* explosionChB;
//    QDateTimeEdit* launchTimeDTE;
//    QComboBox* damageDegreeCB;
//    QComboBox* rocketTypeCB;
//    QLineEdit* quantityLE;

public slots:
    //void slotChangeDataSourceBattery(int);

    /*void slotAddPoint();
    void slotRemovePoint();

    void slotToggleRandomRB();
    void slotToggleSquareRB();
    void slotToggleRoundRB();
    void slotToggleLaunchCB();
    void slotToggleExplosionCB();*/
};

#endif // HITTARGETS_H
