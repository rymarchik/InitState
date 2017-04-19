#pragma once

#include <QWidget>
#include <QString>
#include <QtSql>

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

private:


private slots:

};
