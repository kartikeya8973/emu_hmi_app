#include "slave.h"

slave::slave()
{

}

slave::slave(int _sno,int _rs485_addr,QString _ip_addr,device_type_t _device_type, activeStatus_t _activeStatus, installStatus_t _installStatus, QTcpSocket *_socket, QString _hw_id)
{
    sno=_sno;
    //    rs485_addr=_rs485_addr;
    ip_addr=_ip_addr;
    device_type=_device_type;
    activeStatus=_activeStatus;
    installStatus = _installStatus;
    //    socket = _socket;
    hw_id = _hw_id;

    switch(device_type)
    {
    case DEV_DDC:
        device_type_desc ="DRIVER DESK CONSOLE";
        break;
    case DEV_HMI:
        device_type_desc ="CCTV HMI UNIT";
        break;
    case DEV_SIPS:
        device_type_desc ="VoIP SERVER";
        break;
    case DEV_ETB:
        device_type_desc ="EMERGENCY TALK-=BACK UNIT";
        break;
    case DEV_NVR:
        device_type_desc ="NETWORK VIDEO RECORDER";
        break;
    }
}

void slave::write(QJsonObject &json) const
{
    json["sno"]=sno;
    //    json["rs485_addr"]=rs485_addr;
    json["ip_addr"]=ip_addr;

    switch(device_type)
    {
    case DEV_DDC:
    {
        json["device_type"]="DEV_DDC";
    }
        break;
    case DEV_HMI:
    {
        json["device_type"]="DEV_HMI";
    }
        break;
    case DEV_SIPS:
    {
        json["device_type"]="DEV_SIPS";
    }
        break;

    case DEV_ETB:
    {
        json["device_type"]="DEV_ETB";
    }
        break;
    case DEV_NVR:
    {
        json["device_type"]="DEV_NVR";
    }
        break;

        //    switch(activeStatus)
        //    {
        //    case INACTIVE:
        //        json["activeStatus"]="INACTIVE";
        //        break;
        //    case ACTIVE:
        //        json["activeStatus"]="ACTIVE";
        //        break;
        //    }

        switch(installStatus)
        {
        case NOT_INSTALLED:
            json["installStatus"]="NOT_INSTALLED";
            break;
        case INSTALLED:
            json["installStatus"]="INSTALLED";
            break;
        }

        json["hw_id"]=hw_id;
        //    json["conn_mode"]=conn_mode;
    }
}

void slave::read(const QJsonObject &json)
{
    if (json.contains("sno") && json["sno"].isDouble())
        sno = json["sno"].toInt();

    //    if (json.contains("rs485_addr") && json["rs485_addr"].isDouble())
    //        rs485_addr = json["rs485_addr"].toInt();

    if (json.contains("ip_addr") && json["ip_addr"].isString())
        ip_addr = json["ip_addr"].toString();

    if (json.contains("device_type") && json["device_type"].isString())
    {
        QString dt = json["device_type"].toString();

                                    if(dt=="DEV_DDC")
                                    {
                                        device_type=DEV_DDC;
                                    }
                                    else
                                        if(dt=="DEV_HMI")
                                        {
                                            device_type=DEV_HMI;
                                        }
                                        else
                                            if(dt=="DEV_SIPS")
                                            {
                                                device_type=DEV_SIPS;
                                            }
                                            else
                                                if(dt=="DEV_ETB")
                                                {
                                                    device_type=DEV_ETB;
                                                }
                                                else
                                                    if(dt=="DEV_NVR")
                                                    {
                                                        device_type=DEV_NVR;
                                                    }
    }

    switch(device_type)
    {
        case DEV_DDC:
        device_type_desc ="DRIVER DESK CONSOLE";
        break;
    case DEV_HMI:
        device_type_desc ="CCTV HMI UNIT";
        break;
    case DEV_SIPS:
        device_type_desc ="VoIP SERVER";
        break;
    case DEV_ETB:
        device_type_desc ="EMERGENCY TALKBACK UNIT";
        break;
    case DEV_NVR:
        device_type_desc ="NETWORK VIDEO RECORDER";
        break;
    }

    if (json.contains("installStatus") && json["installStatus"].isString())
    {
        QString is = json["installStatus"].toString();

        if(is=="NOT_INSTALLED")
        {
            installStatus=NOT_INSTALLED;
        }
        else
            if(is=="INSTALLED")
            {
                installStatus=INSTALLED;
            }
    }

    if (json.contains("activeStatus") && json["activeStatus"].isString())
    {
        QString as = json["activeStatus"].toString();

        if(as=="INACTIVE")
        {
            activeStatus=INACTIVE;
        }
        else
            if(as=="ACTIVE")
            {
                activeStatus=ACTIVE;
            }
    }

    if (json.contains("hw_id") && json["hw_id"].isString())
    {
        hw_id = json["hw_id"].toString();
    }

    //    if (json.contains("conn_mode") && json["conn_mode"].isString())
    //    {
    //        conn_mode = json["conn_mode"].toString();
    //    }
}

