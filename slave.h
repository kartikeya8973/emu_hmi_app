#ifndef SLAVE_H
#define SLAVE_H

#include "common.h"
#include <QtNetwork>
class slave
{
public:
    slave();
    slave(int,int,QString,device_type_t,activeStatus_t,installStatus_t,QTcpSocket*,QString);

    int sno;
//    int rs485_addr;
    QString ip_addr="";
    device_type_t device_type;
    QString device_type_desc="";
    installStatus_t installStatus;
    activeStatus_t activeStatus;
    QTcpSocket  *socket;
    QString hw_id="";
//    QString conn_mode="";

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

};

#endif // SLAVE_H
