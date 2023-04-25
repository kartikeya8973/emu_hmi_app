#include "camera.h"

camera::camera()
{

}
camera::camera(int _sno,int _csno,int _bu, int _busno,QString _ip_addr,camera_type_t _camera_type,car_type_t _car_type, activeStatus_t _activeStatus, installStatus_t _installStatus, QTcpSocket *_socket)
{
    sno=_sno;
    csno=_csno;
    bu=_bu;
    busno=_busno;
    //    rs485_addr=_rs485_addr;
    ip_addr=_ip_addr;
    camera_type=_camera_type;
    car_type =_car_type;
    activeStatus=_activeStatus;
    installStatus = _installStatus;
    //    socket = _socket;

    switch(camera_type)
    {
    case CAM_CCTV_F:
        camera_type_desc ="CCTV FRONT";
        break;
    case CAM_CCTV_FR:
        camera_type_desc ="CCTV FRONT RIGHT";
        break;
    case CAM_CCTV_FL:
        camera_type_desc ="CCTV FRONT LEFT";
        break;
    case CAM_CCTV_R:
        camera_type_desc ="CCTV REAR";
        break;
    case CAM_CCTV_RR:
        camera_type_desc ="CCTV REAR RIGHT";
        break;
    case CAM_CCTV_RL:
        camera_type_desc ="CCTV REAR LEFT";
        break;
    case CAM_CCTV_SL:
        camera_type_desc ="CCTV SALOON";
        break;
    case CAM_CCTV_CAB:
        camera_type_desc ="CCTV CAB";
        break;
    }
    switch(car_type)
    {
    case CAR_DTC:
        car_type_desc ="CAR DTC";
        break;
    case CAR_MC:
        camera_type_desc ="CAR MC";
        break;
    case CAR_TC:
        camera_type_desc ="CAR TC";
        break;
    case CAR_NDTC:
        camera_type_desc ="CAR NDTC";
        break;
    }
}

void camera::write(QJsonObject &json) const
{
    json["sno"]=sno;
    json["csno"]=csno;
    json["bu"]=bu;
    json["busno"]=busno;
    json["ip_addr"]=ip_addr;

    switch(camera_type)
    {
    case CAM_CCTV_F:
    {
        json["camera_type"]="CAM_CCTV_F";
    }
        break;
    case CAM_CCTV_FR:
    {
        json["camera_type"]="CAM_CCTV_FR";
    }
        break;
    case CAM_CCTV_FL:
    {
        json["camera_type"]="CAM_CCTV_FL";
    }
        break;
    case CAM_CCTV_R:
    {
        json["camera_type"]="CAM_CCTV_R";
    }
        break;
    case CAM_CCTV_RR:
    {
        json["camera_type"]="CAM_CCTV_RR";
    }
        break;
    case CAM_CCTV_RL:
    {
        json["camera_type"]="CAM_CCTV_RL";
    }
        break;
    case CAM_CCTV_SL:
    {
        json["camera_type"]="CAM_CCTV_SL";
    }
        break;
    case CAM_CCTV_CAB:
    {
        json["camera_type"]="CAM_CCTV_CAB";
    }
        break;

    }

    switch(installStatus)
    {
    case NOT_INSTALLED:
        json["installStatus"]="NOT_INSTALLED";
        break;
    case INSTALLED:
        json["installStatus"]="INSTALLED";
        break;
    }

    switch(car_type)
    {
    case CAR_DTC:
    {
        json["car_type"]="CAR_DTC";
    }
        break;
    case CAR_MC:
    {
        json["car_type"]="CAR_MC";
    }
        break;
    case CAR_TC:
    {
        json["car_type"]="CAR_TC";
    }
        break;
    case CAR_NDTC:
    {
        json["car_type"]="CAR_NDTC";
    }
        break;
    }
}

void camera::read(const QJsonObject &json)
{
    if (json.contains("sno") && json["sno"].isDouble())
        sno = json["sno"].toInt();

    if (json.contains("csno") && json["csno"].isDouble())
        csno = json["csno"].toInt();

    if (json.contains("bu") && json["bu"].isDouble())
        bu = json["bu"].toInt();

    if (json.contains("busno") && json["busno"].isDouble())
        busno = json["busno"].toInt();

    if (json.contains("ip_addr") && json["ip_addr"].isString())
        ip_addr = json["ip_addr"].toString();

    if (json.contains("camera_type") && json["camera_type"].isString())
    {
        QString dt = json["camera_type"].toString();

        if(dt=="CAM_CCTV_F")
        {
            camera_type=CAM_CCTV_F;
        }
        else
            if(dt=="CAM_CCTV_FR")
            {
                camera_type=CAM_CCTV_FR;
            }
            else
                if(dt=="CAM_CCTV_FL")
                {
                    camera_type=CAM_CCTV_FL;
                }
                else
                    if(dt=="CAM_CCTV_R")
                    {
                        camera_type=CAM_CCTV_R;
                    }
                    else
                        if(dt=="CAM_CCTV_RR")
                        {
                            camera_type=CAM_CCTV_RR;
                        }
                        else
                            if(dt=="CAM_CCTV_RL")
                            {
                                camera_type=CAM_CCTV_RL;
                            }
                            else
                                if(dt=="CAM_CCTV_SL")
                                {
                                    camera_type=CAM_CCTV_SL;
                                }
                                else
                                    if(dt=="CAM_CCTV_CAB")
                                    {
                                        camera_type=CAM_CCTV_CAB;
                                    }
    }

    switch(camera_type)
    {
    case CAM_CCTV_F:
        camera_type_desc ="CAM_CCTV_F";
        break;
    case CAM_CCTV_FR:
        camera_type_desc ="CAM_CCTV_FR";
        break;
    case CAM_CCTV_FL:
        camera_type_desc ="CAM_CCTV_FL";
        break;
    case CAM_CCTV_R:
        camera_type_desc ="CAM_CCTV_R";
        break;
    case CAM_CCTV_RR:
        camera_type_desc ="CAM_CCTV_RR";
        break;
    case CAM_CCTV_RL:
        camera_type_desc ="CAM_CCTV_RL";
        break;
    case CAM_CCTV_SL:
        camera_type_desc ="CAM_CCTV_SL";
        break;
    case CAM_CCTV_CAB:
        camera_type_desc ="CAM_CCTV_CAB";
        break;
    }

    if (json.contains("car_type") && json["car_type"].isString())
    {
        QString dt = json["car_type"].toString();

        if(dt=="CAR_DTC")
        {
            car_type=CAR_DTC;
        }
        else
            if(dt=="CAR_MC")
            {
                car_type=CAR_MC;
            }
            else
                if(dt=="CAR_TC")
                {
                    car_type=CAR_TC;
                }
                else
                    if(dt=="CAR_NDTC")
                    {
                        car_type=CAR_NDTC;
                    }

    }

    switch(car_type)
    {
    case CAR_DTC:
        car_type_desc ="CAR_DTC";
        break;
    case CAR_MC:
        car_type_desc ="CAR_MC";
        break;
    case CAR_TC:
        car_type_desc ="CAR_TC";
        break;
    case CAR_NDTC:
        car_type_desc ="CAR_NDTC";
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

    //    if (json.contains("hw_id") && json["hw_id"].isString())
    {
        //        hw_id = json["hw_id"].toString();
    }

    //    if (json.contains("conn_mode") && json["conn_mode"].isString())
    //    {
    //        conn_mode = json["conn_mode"].toString();
    //    }
}
