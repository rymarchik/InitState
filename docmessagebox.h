#ifndef DOCMESSAGEBOX_H
#define DOCMESSAGEBOX_H

#include <QString>
#include <QStringList>


class DocMessageBox
{
public:
    DocMessageBox();

    QString getDocNumber() const;
    void setDocNumber(const QString &value);
    QString getTimeRegister() const;
    void setTimeRegister(const QString &value);
    QString getDocType() const;
    void setDocType(const QString &value);
    QString getDocTheme() const;
    void setDocTheme(const QString &value);
    QString getText() const;
    void setText(const QString &value);
    QStringList getReceivers() const;
    void setReceivers(const QStringList &value);
    QStringList getReceiversMarks() const;
    void setReceiversMarks(const QStringList &value);
    QStringList getReceiversTime() const;
    void setReceiversTime(const QStringList &value);
    QString getTimeAdd() const;
    void setTimeAdd(const QString &value);

private:
    QString docNumber;
    QString timeRegister;
    QString docType;
    QString docTheme;
    QString text;
    QString timeAdd;
    QStringList receivers;
    QStringList receiversMarks;
    QStringList receiversTime;
};

#endif // DOCMESSAGEBOX_H
