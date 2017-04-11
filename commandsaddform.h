#ifndef COMMANDSADDFORM_H
#define COMMANDSADDFORM_H

#include <QWidget>
#include <QString>
#include <QtSql>
#include <QTreeWidget>
#include "addparamform.h"

namespace Ui {
class CommandsAddForm;
}

class CommandsAddForm : public QWidget
{
    Q_OBJECT

public:
    explicit CommandsAddForm(QWidget *parent = 0);
    ~CommandsAddForm();
    Ui::CommandsAddForm *ui;

private:
    void setCommandsSignals();
    void setAttributeExecution();

    AddParamForm *addParamForm;

private slots:
    void changeEnabledTimerExec();
    void changeContent();
    void addRecivers();
    void recieveData(QString parametr, QString value);
};

#endif // COMMANDSADDFORM_H
