#ifndef ADDRECIVERSFORM_H
#define ADDRECIVERSFORM_H

#include <QWidget>
#include <QCheckBox>
#include <QString>
#include <QtSql>
#include <QMessageBox>
#include <QErrorMessage>

namespace Ui {
class AddReciversForm;
}

class AddReciversForm : public QWidget
{
    Q_OBJECT

public:
    explicit AddReciversForm(QWidget *parent = 0);
    ~AddReciversForm();

signals:
    void sendData(QStringList receivers, QString mark);

private slots:
    void on_reverseButton_clicked();
    void on_okButton_clicked();
private:
    QList<QCheckBox*> checkBoxList;
    Ui::AddReciversForm *ui;
};

#endif // ADDRECIVERSFORM_H
