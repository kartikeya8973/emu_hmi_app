#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"
#include <QtNetwork>
class camera
{
public:
    camera();
    camera(int,int,int,int,QString,camera_type_t,car_type_t,activeStatus_t,installStatus_t,QTcpSocket*);

    int sno;
    int csno;
    int bu;
    int busno;
//    int rs485_addr;
    QString ip_addr="";
    camera_type_t camera_type;
    car_type_t car_type;
    installStatus_t installStatus;
    activeStatus_t activeStatus;
    QTcpSocket  *socket;
    QString camera_type_desc="";
    QString car_type_desc="";
//    QString conn_mode="";

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

};

#endif // CAMERA_H
