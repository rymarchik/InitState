#ifndef COMMANDSADDFORM_H
#define COMMANDSADDFORM_H

#include <QWidget>
#include <QString>
#include <QtSql>
#include <QTreeWidget>
#include <QDateTime>
#include "addparamform.h"
#include "addreciversform.h"
#include "utility.h"
#include "commandsmessagebox.h"

namespace Ui {
class CommandsAddForm;
}

class CommandsAddForm : public QWidget
{
    Q_OBJECT

public:
    explicit CommandsAddForm(QString ownName, QSqlDatabase db, QWidget *parent = 0);
    ~CommandsAddForm();
    Ui::CommandsAddForm *ui;

    bool getCommandOrDoc();
    CommandsMessageBox getInformationBox();

private:

    QSqlDatabase db;
    AddParamForm *addParamForm;
    AddReciversForm *addReciversForm;

    void setCommandsSignals();
    void setAttributeExecution();

    void setDocumentsTheme();
    void setDocumentsType();
private slots:
    void changeEnabledTimerExec();
    void changeContent();
    void receiveDataParametrs(QString parametr, QString value);
    void receiveDataReceivers(QStringList receiver, QString mark);
};

#endif // COMMANDSADDFORM_H
