#pragma once

#include <QWidget>
#include <QString>
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

    void reinitializeFormData();
    void addFilledPoints();
    QStringList getDataSourceBatteries();
    QStringList getDataSourceWeaponry();
    QStringList getHitTargets();
    QStringList getCoverDegrees();
    QStringList getDamageDegrees();
    QStringList getRocketTypes();

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
