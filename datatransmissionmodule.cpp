#include "datatransmissionmodule.h"

DataTransmissionModule::DataTransmissionModule(QSqlDatabase db)
{
    this->myPort = "5824";
    this->targetPort = "5825";
    this->targetIp.setAddress("192.168.1.87");
    this->myIp.setAddress("192.168.1.87");
    this->db=db;
    udpSocket.bind(5824);

}

void DataTransmissionModule::sendCommand(QString q)
{
    QString data = makeDatagramCommand(q);
    QStringList list;
    list << myIp.toString()
         << targetIp.toString()
        << "0"
         << "17"
         << QString::number( data.length() + 224 )
         << myPort
         << targetPort
         << QString::number( data.length() )
         << "14"
         << "0001"
         << QString::number( unicumMessageId )
         << "1"
         << "1"
         << data;
    unicumMessageId++;
    QByteArray datagram = converter->encodeDatagram(list);
    udpSocket.writeDatagram(datagram, targetIp, targetPort.toLong(Q_NULLPTR, 10));
}

void DataTransmissionModule::sendDocument(QString q)
{
    QString data = makeDatagramDocument(q);
    qDebug() << data;
    QStringList list;
    list << myIp.toString()
         << targetIp.toString()
         << "0"
         << "17"
         << QString::number( data.length() + 224 )
         << myPort
         << targetPort
         << QString::number( data.length() )
         << "14"
         << "0001"
         << QString::number( unicumMessageId )
         << "1"
         << "1"
         << data;
    unicumMessageId++;
    QByteArray datagram = converter->encodeDatagram(list);
    udpSocket.writeDatagram( datagram, targetIp, targetPort.toLong( Q_NULLPTR, 10) );
}

void DataTransmissionModule::sendCoord(QString id) //id записи в БД
{
    QString data = makeDatagramCoord( id );
    QStringList list;
    list << myIp.toString()
         << targetIp.toString()
         << "0"
         << "17"
         << QString::number( data.length() + 224 )
         << myPort
         << targetPort
         << QString::number( data.length() )
         << "14"
         << "0001"
         << QString::number( unicumMessageId )
         << "1"
         << "1"
         << data;
    unicumMessageId++;
    QByteArray datagram = converter->encodeDatagram( list );
    qDebug() << targetPort.toLong( Q_NULLPTR, 10 );
    udpSocket.writeDatagram( datagram, targetIp, targetPort.toLong( Q_NULLPTR, 10) );
}

void DataTransmissionModule::sendRocket(QString id) //id записи (какой надо уточнить)
{
    QString data = makeDatagramRocket( id );
    QStringList list;
    list << myIp.toString()
         << targetIp.toString()
         << "0"
         << "17"
         << QString::number( data.length() + 224 )
         << myPort
         << targetPort
         << QString::number( data.length() )
         << "14"
         << "0001"
         << QString::number( unicumMessageId )
         << "1"
         << "1"
         << data;
    unicumMessageId++;
    QByteArray datagram = converter->encodeDatagram( list );
    qDebug() << targetPort.toLong( Q_NULLPTR, 10 );
    udpSocket.writeDatagram( datagram, targetIp, targetPort.toLong( Q_NULLPTR, 10) );
}

void DataTransmissionModule::sendMode(QString id) {
    QString data = makeDatagramMode( id );
    QStringList list;
    list << myIp.toString()
         << targetIp.toString()
         << "0"
         << "17"
         << QString::number( data.length() + 224 )
         << myPort
         << targetPort
         << QString::number( data.length() )
         << "14"
         << "0001"
         << QString::number( unicumMessageId )
         << "1"
         << "1"
         << data;
    unicumMessageId++;
    QByteArray datagram = converter->encodeDatagram( list );
    qDebug() << targetPort.toLong( Q_NULLPTR, 10 );
    udpSocket.writeDatagram( datagram, targetIp, targetPort.toLong( Q_NULLPTR, 10) );
}

QString DataTransmissionModule::makeDatagramMode( QString q )
{
    QString answer = "";
    answer.append( "0" );                        //метод сжатия
    answer.append( converter->dobei( q, 6 ) );      //отправитель добить до 6
    answer.append( converter->dobei( "mbu" , 6) );  //получатель
    answer.append( "0" );                        //категория данных
    answer.append( "C" );                        //данные о сообщении
    answer.append( "C1" );                       //Идентификатор приложения, которое  должно обрабатывать переданные данные.
    answer.append( "=" );                        //Признак начала передаваемых данных
    QString request = getCoordInformation(q);
    if (request.compare("error") == 0) {
        return "error";
    }
    else {
        answer.append(request);
    }
    return answer;
}

QString DataTransmissionModule::makeDatagramCoord( QString q )
{
    QString answer = "";
    answer.append( "0" );                        //метод сжатия
    answer.append( converter->dobei( q, 6 ) );      //отправитель добить до 6
    answer.append( converter->dobei( "mbu" , 6) );  //получатель
    answer.append( "0" );                        //категория данных
    answer.append( "C" );                        //данные о сообщении
    answer.append( "C1" );                       //Идентификатор приложения, которое  должно обрабатывать переданные данные.
    answer.append( "=" );                        //Признак начала передаваемых данных
    QString request = getCoordInformation(q);
    if (request.compare("error") == 0) {
        return "error";
    }
    else {
        answer.append(request);
    }
    return answer;
}

QString DataTransmissionModule::makeDatagramRocket( QString q )
{
    QString answer = "";
    answer.append( "0" );                        //метод сжатия
    answer.append( converter->dobei( q , 6 ) );      //отправитель добить до 6
    answer.append( converter->dobei( "mbu" , 6) );  //получатель
    answer.append( "0" );                        //категория данных
    answer.append( "C" );                        //данные о сообщении
    answer.append( "T1" );                       //Идентификатор приложения, которое  должно обрабатывать переданные данные.
    answer.append( "=" );                        //Признак начала передаваемых данных
    QString request = getRocketInformation(q);
    if (request.compare("error") == 0) {
        return "error";
    }
    else {
        answer.append(request);
    }
    return answer;
}

QString DataTransmissionModule::makeDatagramDocument( QString q )
{
    QString answer = "";
    answer.append( "0" );                        //метод сжатия
    answer.append( converter->dobei( q, 6 ) );      //отправитель добить до 6
    answer.append( converter->dobei( "mbu" , 6) );  //получатель
    answer.append( "0" );                        //категория данных
    answer.append( "X" );                        //данные о сообщении
    answer.append( "D1" );                       //Идентификатор приложения, которое  должно обрабатывать переданные данные.
    answer.append( "=" );                        //Признак начала передаваемых данных
    QString request = getDocumentInformation(q);
    if (request.compare("error") == 0) {
        return "error";
    }
    else {
        answer.append(request);
    }
    return answer;
}

QString DataTransmissionModule::makeDatagramCommand( QString q )
{
    QString answer = "";
    answer.append( "0" );                        //метод сжатия
    answer.append( converter->dobei( q, 6 ) );      //отправитель добить до 6
    answer.append( converter->dobei( "mbu" , 6) );  //получатель
    answer.append( "0" );                        //категория данных
    answer.append( "P" );                        //данные о сообщении
    answer.append( "K1" );                       //Идентификатор приложения, которое  должно обрабатывать переданные данные.
    answer.append( "=" );                        //Признак начала передаваемых данных
    QString request = getCommandInformation(q);
    if (request.compare("error") == 0) {
        return "error";
    }
    else {
        answer.append(request);
    }
    return answer;
}

QString DataTransmissionModule::getCoordInformation(QString object)
{
    QString answer = "";
    QSqlQuery query= QSqlQuery(db);
    QString s;
    s = "SELECT st_x(obj_location), st_y(obj_location), st_z(obj_location), direction FROM own_forces.combatobject_location WHERE combat_hierarchy='"+object+"';";
    if ( !query.exec( s ) ) {
        return "error";
    }
    else {
        if ( query.size() == 0 ) return "error";
        while ( query.next() ) {
            answer.append( query.value(0).toString() );
            answer.append(";");
            answer.append( query.value(1).toString() );
            answer.append( ";" );
            answer.append( query.value(2).toString() );
            answer.append(";");
            answer.append( query.value(3).toString() );
            answer.append(";");
        }
    }
    answer.append( "\r" );
    return answer;
}

QString DataTransmissionModule::getRocketInformation(QString object)
{
    QString answer = "";
    QSqlQuery query= QSqlQuery(db);
    QString s;
    s = "SELECT type_tid FROM own_forces.rocket WHERE combatobjectid='"+object+"';";
    if ( !query.exec( s ) ) {
        return "error";
    }
    else {
        if ( query.size() == 0 ) return "error";
        while ( query.next() ) {
            if (QString::compare( query.value( 0 ).toString(), "51.50.10") == 0) {
                answer.append( "11" );
                answer.append( ";" );
            }
            if (QString::compare( query.value( 0 ).toString(), "51.50.15") == 0) {
                answer.append( "12" );
                answer.append( ";" );
            }
            if (QString::compare( query.value( 0 ).toString(), "51.50.20") == 0) {
                answer.append( "13" );
                answer.append( ";" );
            }
            if (QString::compare( query.value( 0 ).toString(), "51.50.25") == 0) {
                answer.append( "14" );
                answer.append( ";" );
            }
            if (QString::compare( query.value( 0 ).toString(), "51.50.30") == 0) {
                answer.append( ";" );
            }
        }
    }
    answer.append( "\r" );
    return answer;
}

QString DataTransmissionModule::getDocumentInformation(QString object)
{
    QString answer = "";
    QSqlQuery query= QSqlQuery(db);
    QString s;
    s =s+"SELECT cinf.outgoing_reg_number, cinf.outgoing_reg_datetime, ctyp.doctype_tid, cthm.doctheme_tid "+
        "FROM combatdocs.combatdocs_info cinf JOIN combatdocs.combatdocs_type ctyp ON cinf.cmbdid = ctyp.cmbdid "+
        "JOIN combatdocs.combatdocs_theme cthm ON cinf.cmbdid = cthm.cmbdid WHERE cinf.cmbdid='"+object+"';";
    if ( !query.exec( s ) ) {
        return "error";
    }
    else {
        if ( query.size() == 0 ) return "error";
        while ( query.next() ) {
            answer.append( query.value( 0 ).toString() );
            answer.append( ";" );
            answer.append( query.value( 1 ).toString() );
            answer.append( ";" );
            answer.append( query.value( 2 ).toString() );
            answer.append( ";" );
            answer.append( query.value( 3 ).toString() );
            answer.append( ";" );
        }
    }
    //преобразование времени в нужный формат
    answer.append( "\r" );
    return answer;
}

QString DataTransmissionModule::getCommandInformation(QString object)
{
    QString answer = "";
    QSqlQuery query= QSqlQuery(db);
    QString s;
    s =s+"SELECT orinf.order_id, orinf.date_add, order_tid, orattr.execution_time, orattr.execution_indication_tid "+
        "FROM orders_alerts.orders_alerts_info orinf JOIN orders_alerts.orders_alerts_attrib orattr "+
        "ON orattr.order_id = orinf.order_id WHERE orinf.order_id='"+object+"';";
    if ( !query.exec( s ) ) {
        return "error";
    }
    else {
        if ( query.size() == 0 ) return "error";
        while ( query.next() ) {
            answer.append( query.value( 0 ).toString() );
            answer.append( ";" );
            answer.append( query.value( 1 ).toString() );
            answer.append( ";" );
            answer.append( query.value( 2 ).toString() );
            answer.append( ";" );
            answer.append( query.value( 3 ).toString() );
            answer.append( ";" );
            answer.append( query.value( 4 ).toString() );
            answer.append( ";" );
        }
    }
    s = "SELECT param_tid, param_value FROM orders_alerts.orders_alerts_param WHERE order_id='";
    s = s + object +"';";
    if ( !query.exec( s ) ) {
        return "error";
    }
    else {
        int size = query.size();
        answer.append(QString::number(size));
        answer.append( ";" );
        if ( query.size() != 0 ) {
            while ( query.next() ) {
                answer.append( query.value( 0 ).toString() );
                answer.append( ";" );
                answer.append( query.value( 1 ).toString() );
                answer.append( ";" );
            }
        }
    }
    answer.append( "\r" );
    return answer;
}

void DataTransmissionModule::readDatagram()
{
    QByteArray datagram;
    QByteArray package;
    QByteArray confirm;
    datagram.resize( udpSocket.pendingDatagramSize() );
    udpSocket.readDatagram( datagram.data(), datagram.size() );
    qDebug() << datagram;
    qDebug() << datagram.size();
    QStringList messageMembersList = converter->decodeDatagram(datagram, &package, &confirm);
    qDebug() << "package" << package;
    parsingMessage( package );
}

void DataTransmissionModule::sendConfirm(QByteArray datagram) {
    udpSocket.writeDatagram( datagram, targetIp, targetPort.toLong( Q_NULLPTR, 10) );
}

void DataTransmissionModule::parsingMessage( QByteArray s )
{
    QString source = "";
    for (int i=0; i<1; i++) {
        QByteArray q;
        q[0]=s[i];
        QString str(q);
        source.append(str);
    }
    qDebug() << "метод сжатия данных " << source;
    source = "";
    for (int i=1; i<7; i++) {
        QByteArray q;
        q[0]=s[i];
        QString str(q);
        source.append(str);
    }
    qDebug() << "отправитель " << source;
    source = "";
    for (int i=7; i<13; i++) {
        QByteArray q;
        q[0]=s[i];
        QString str(q);
        source.append(str);
    }
    qDebug() << "получатель " << source;
    source = "";
    for (int i=13; i<14; i++) {
        QByteArray q;
        q[0]=s[i];
        QString str(q);
        source.append(str);
    }
    qDebug() << "категория данных " << source;
    source = "";
    for (int i=14; i<15; i++) {
        QByteArray q;
        q[0]=s[i];
        QString str(q);
        source.append(str);
    }
    qDebug() << "тип протокола " << source;
    source = "";
    for (int i=15; i<17; i++) {
        QByteArray q;
        q[0]=s[i];
        QString str(q);
        source.append(str);
    }
    qDebug() << "тип сообщения " << source;
    if ( s.at( 0 ) == '1' ) {
        return;
    }
    bool trigger = false;
    QString object = "";
    QString messageCode=source;
    if (QString::compare( messageCode, "K1") == 0) {
        parsingCommand(s);
    }
    else {
    }
}

void DataTransmissionModule::parsingCommand( QByteArray s)
{
    QByteArray data = "";
    CommandsMessageBox box;
    for ( int i = 18; i < s.size(); i++ )
    {
        data += s.at( i );
    }
    qDebug() << data;
    QString source = "";
    for (int i=0; i<6; i++) {
        QByteArray q;
        q[0]=data[i];
        QString str(q);
        source.append(str);
    }
    qDebug() << "идентификатор команды " << source;
    box.setIdCommand(source);
    source = "";
    for (int i=7; i<11; i++) {
        QByteArray q;
        q[0]=data[i];
        source.append(converter->convertToBinaryNew(converter->convertByteToDec(q)));
        qDebug() << source;
    }
    source = converter->convertToDex(source);
    qDebug() << "время формирования команды " << source;
    box.setTimeAdd(source);
    source = "";
    for (int i=12; i<14; i++) {
        QByteArray q;
        q[0]=data[i];
        QString str(q);
        source.append(str);
    }
    qDebug() << "наименование команды " << source;
    box.setCommandName(source);
    source = "";
    for (int i=15; i<19; i++) {
        QByteArray q;
        q[0]=data[i];
        source.append(converter->convertToBinaryNew(converter->convertByteToDec(q)));
        qDebug() << source;
    }
    source = converter->convertToDex(source);
    qDebug() << "время исполнения команды " << source;
    box.setTimeExec(source);
    source = "";
    for (int i=20; i<21; i++) {
        QByteArray q;
        q[0]=data[i];
        QString str(q);
        source.append(str);
    }
    qDebug() << "признак исполнения " << source;
    box.setAttributeExec(source);
    source = "";
    for (int i=22; i<23; i++) {
        QByteArray q;
        q[0]=data[i];
        QString str(q);
        source.append(str);
    }
    qDebug() << "Наименование параметра " << source;
    QStringList paramList;
    QStringList paramValueList;
    paramList << source;
    source = "";
    for (int i=24; i<27; i++) {
        QByteArray q;
        q[0]=data[i];
        QString str(q);
        source.append(str);
    }
    qDebug() << "Значение параметра " << source;
    paramValueList << source;
    source = "";
    for (int i=28; i<data.size()-1; i++) {
        QByteArray q;
        q[0]=data[i];
        QString str(q);
        source.append(str);
    }
    qDebug() << "уточняющий текст " << source;
    box.setParametrs(paramList);
    box.setParametrsValue(paramValueList);
    //dbConnect.saveCommand("1.11", box); !!!вызов метода на занесение команды в БД
}
