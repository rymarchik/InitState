#pragma once

#include <QtWidgets>
#include <QtSql>
#include <QFont>

namespace Ui {
class HitTargets;
}

class HitTargetsTabForm : public QWidget
{
    Q_OBJECT

public:
    explicit HitTargetsTabForm(QWidget *parent = 0);
    ~HitTargetsTabForm();
    Ui::HitTargets *ui;

    void onAddSetup();
    void onEditSetup(QTableWidget*);
    bool onSaveSetup();

    void addCommonFormData();
    void addFilledPoints();
    QStringList getDataSourceBatteries();
    QStringList getDataSourceWeaponry();
    void getHitTargets();
    void getCoverDegrees();
    void getDamageDegrees();
    void getRocketTypes();

    QString getTargetNumberString();
    QString getTargetNameString();

private:
    QFont font = QFont("MS Shell Dlg 2", 9, QFont::Bold);

private slots:
    void slotChangeDataSourceBattery(int);

    void slotAddPoint();
    void slotRemovePoint();

    void slotToggleRandomRB();
    void slotToggleSquareRB();
    void slotToggleRoundRB();
    void slotToggleLaunchCB();
    void slotToggleExplosionCB();
};
