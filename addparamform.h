#ifndef ADDPARAMFORM_H
#define ADDPARAMFORM_H

#include <QWidget>
#include <QString>
#include <QtSql>
#include <QMessageBox>
#include <QErrorMessage>

namespace Ui {
class AddParamForm;
}

class AddParamForm : public QWidget
{
    Q_OBJECT

public:
    explicit AddParamForm(QWidget *parent = 0);
    ~AddParamForm();

signals:
    void sendData(QString parametr, QString value);

private slots:
    void on_reverseButton_clicked();
    void on_okButton_clicked();

private:
    Ui::AddParamForm *ui;
};

#endif // ADDPARAMFORM_H
