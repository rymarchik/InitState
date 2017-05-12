#include "commandsmessagebox.h"

CommandsMessageBox::CommandsMessageBox()
{

}

QString CommandsMessageBox::getCommandName() const
{
    return commandName;
}

void CommandsMessageBox::setCommandName(const QString &value)
{
    commandName = value;
}

QString CommandsMessageBox::getTimeAdd() const
{
    return timeAdd;
}

void CommandsMessageBox::setTimeAdd(const QString &value)
{
    timeAdd = value;
}

QString CommandsMessageBox::getTimeExec() const
{
    return timeExec;
}

void CommandsMessageBox::setTimeExec(const QString &value)
{
    timeExec = value;
}

QString CommandsMessageBox::getAttributeExec() const
{
    return AttributeExec;
}

void CommandsMessageBox::setAttributeExec(const QString &value)
{
    AttributeExec = value;
}

QStringList CommandsMessageBox::getParametrs() const
{
    return parametrs;
}

void CommandsMessageBox::setParametrs(const QStringList &value)
{
    parametrs = value;
}

QStringList CommandsMessageBox::getParametrsValue() const
{
    return parametrsValue;
}

void CommandsMessageBox::setParametrsValue(const QStringList &value)
{
    parametrsValue = value;
}

QStringList CommandsMessageBox::getReceivers() const
{
    return receivers;
}

void CommandsMessageBox::setReceivers(const QStringList &value)
{
    receivers = value;
}

QStringList CommandsMessageBox::getReceiversMarks() const
{
    return receiversMarks;
}

void CommandsMessageBox::setReceiversMarks(const QStringList &value)
{
    receiversMarks = value;
}

QStringList CommandsMessageBox::getReceiversTime() const
{
    return receiversTime;
}

void CommandsMessageBox::setReceiversTime(const QStringList &value)
{
    receiversTime = value;
}

bool CommandsMessageBox::getMarkExec() const
{
    return markExec;
}

void CommandsMessageBox::setMarkExec(bool value)
{
    markExec = value;
}