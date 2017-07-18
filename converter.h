#ifndef CONVERTER_H
#define CONVERTER_H

#include <QString>
#include <QStringList>
#include <QDebug>


class Converter
{
public:
    Converter();
    QByteArray encodeDatagram( QStringList ); //закодировать датаграмму
    QStringList decodeDatagram(QByteArray , QByteArray *, QByteArray *confirm); //раскодировать датаграмму
    QString dobei(QString s, int len);
    QByteArray convertToDoubleHex(QString s);
    QByteArray convertIpHex(QString s);
    QByteArray convertToHex(QString s);
    QString convertByteToDec(QByteArray q);
    QString convertToBinaryNew(QString s);
    QString convertIpBinary( QString ); //конвертирует обычный ip адрес в бинарную строку
    QString convertToBinary( QString, int ); //преобразование заданной строки в бинарную, и подгонка её к нужному размеру
    QString convertIpDex( QString ); //преобразование бинарной строки в обычный ip адрес
    QString convertToDex( QString ); //преобразование бинарной строки в обычную
};

#endif // CONVERTER_H
