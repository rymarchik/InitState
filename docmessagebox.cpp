#include "docmessagebox.h"

DocMessageBox::DocMessageBox()
{

}

QString DocMessageBox::getDocNumber() const
{
    return docNumber;
}

void DocMessageBox::setDocNumber(const QString &value)
{
    docNumber = value;
}

QString DocMessageBox::getTimeRegister() const
{
    return timeRegister;
}

void DocMessageBox::setTimeRegister(const QString &value)
{
    timeRegister = value;
}

QString DocMessageBox::getDocType() const
{
    return docType;
}

void DocMessageBox::setDocType(const QString &value)
{
    docType = value;
}

QString DocMessageBox::getDocTheme() const
{
    return docTheme;
}

void DocMessageBox::setDocTheme(const QString &value)
{
    docTheme = value;
}

QString DocMessageBox::getText() const
{
    return text;
}

void DocMessageBox::setText(const QString &value)
{
    text = value;
}

QStringList DocMessageBox::getReceivers() const
{
    return receivers;
}

void DocMessageBox::setReceivers(const QStringList &value)
{
    receivers = value;
}

QStringList DocMessageBox::getReceiversMarks() const
{
    return receiversMarks;
}

void DocMessageBox::setReceiversMarks(const QStringList &value)
{
    receiversMarks = value;
}

QStringList DocMessageBox::getReceiversTime() const
{
    return receiversTime;
}

void DocMessageBox::setReceiversTime(const QStringList &value)
{
    receiversTime = value;
}

QString DocMessageBox::getTimeAdd() const
{
    return timeAdd;
}

void DocMessageBox::setTimeAdd(const QString &value)
{
    timeAdd = value;
}
