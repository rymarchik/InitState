#ifndef DOCMESSAGEBOX_H
#define DOCMESSAGEBOX_H


class DocMessageBox
{
public:
    DocMessageBox();

private:
    QString docNumber;
    QString timeAdd;
    QString timeExec;
    QString AttributeExec;
    QStringList parametrs;
    QStringList parametrsValue;
    QStringList receivers;
    QStringList receiversMarks;
    QStringList receiversTime;
};

#endif // DOCMESSAGEBOX_H
