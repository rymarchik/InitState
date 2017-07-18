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
#include "docmessagebox.h"

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
    CommandsMessageBox getCommandtInformationBox();
    DocMessageBox getDocumentInformationBox();
    void setDataCommand(QString code);
    void setDataDocument(QString code);

    int getSaveMode() const;
    void setSaveMode(int value);

private:
    QSqlDatabase db;
    AddParamForm *addParamForm;
    AddReciversForm *addReciversForm;
    int saveMode; //0 новый, 1 изменить
    QString idCommand;

    void setCommandsSignals();
    void setAttributeExecution();

    void setDocumentsTheme();
    void setDocumentsType();
    void setDocNumber();
private slots:

    void changeEnabledTimerExec();
    void changeContent();
    void receiveDataParametrs(QString parametr, QString value);
    void receiveDataReceivers(QStringList receiver, QString mark);
};

#endif // COMMANDSADDFORM_H
