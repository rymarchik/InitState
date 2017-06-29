#include "datatransmissionmodule.h"

DataTransmissionModule::DataTransmissionModule(QString myIp, QString targetIp, QString myPort, QString targetPort, QSqlDatabase db)
{
    this->myIp.setAddress(myIp);
    this->targetIp.setAddress(targetIp);
    this->myPort = myPort;
    this->targetPort = targetPort;
    this->db=db;

}

void DataTransmissionModule::sendCommand(QString q)
{
    QString trash;
    QString data = makeDatagramCommand(q);
    QStringList list;
    list << myIp.toString()
         << targetIp.toString()
         << "17"
         << QString::number( data.length() + 224 )
         << myPort
         << targetPort
         << QString::number( data.length() )
         << ""
         << "0001"
         << QString::number( unicumMessageId )
         << "1"
         << "1"
         << data;
    unicumMessageId++;
    QByteArray datagram = converter->encodeDatagram(list);
    udpSocket.writeDatagram( datagram, targetIp, targetPort.toLong( Q_NULLPTR, 10) );
}

void DataTransmissionModule::sendDocument(QString q)
{
    QString trash;
    QString data = makeDatagramDocument(q);
    qDebug() << data;
    QStringList list;
    list << myIp.toString()
         << targetIp.toString()
         << "17"
         << QString::number( data.length() + 224 )
         << myPort
         << targetPort
         << QString::number( data.length() )
         << ""
         << "0001"
         << QString::number( unicumMessageId )
         << "1"
         << "1"
         << data;
    unicumMessageId++;
    QByteArray datagram = converter->encodeDatagram(list);
    udpSocket.writeDatagram( datagram, targetIp, targetPort.toLong( Q_NULLPTR, 10) );
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
