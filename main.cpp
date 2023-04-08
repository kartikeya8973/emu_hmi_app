#include "mainwindow.h"
#include "pingthread.h"
#include "common.h"
#include "slave.h"
#include "systemconfig.h"
#include "slavesyncthread.h"
#include "ipcamsyncthread.h"


#include <QApplication>

systemconfig systemconfig;
QList <camera*> hmi_slaves;

QList <QList <camera*>> rake;


QList <slave*> papis_slaves;
slave_tasks_t   slave_task;
SlaveSyncThread *slaveSyncThread;
IpCamSyncThread *ipcamsyncthread;

int inactive=0;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //check for config, if not found then
    if(!load_config())
    {
        populate_default_config();
    }

    MainWindow w;
    w.setWindowFlag(Qt::FramelessWindowHint);
    //    w.show();
    w.showFullScreen(); //opens the application in fullscreen
    return a.exec();
}

bool w_log(QString msg)
{
    //    l_log.append(msg);
    //    QFile file(base_dir+"logs/system.log");
    //    if (!file.open(QIODevice::Append)) {
    //        return false;
    //    }

    //    QTextStream ts(&file);
    //    ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<","<<systemconfig.mpu_id<<","<<systemconfig.coach_id<<"/"
    //                        <<systemconfig.coach_no<<","<<msg<<"\n";
    //    file.close();

    return true;
}

bool w_fault(QString msg)
{
    //    l_fault.append(msg);
    //    QFile file(base_dir+"faults/fault.log");
    //    if (!file.open(QIODevice::Append)) {
    //        return false;
    //    }

    //    QTextStream ts(&file);
    //    ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<","<<systemconfig.mpu_id<<","<<systemconfig.coach_id<<"/"
    //                        <<systemconfig.coach_no<<","<<msg<<"\n";
    //    file.close();

    return true;
}



bool load_config()
{
    QFile loadFile(QString(base_dir+"config/config.json"));

    if (!loadFile.open(QIODevice::ReadOnly)) {
        w_fault("System Config file not found on disk!");
        return false;
    }
    else
    {
        QByteArray saveData = loadFile.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
        QJsonObject json = loadDoc.object();

        //load config
        if (json.contains("systemconfig") && json["systemconfig"].isObject())
        {
            systemconfig.read(json["systemconfig"].toObject());
        }

        //load users
//        if (json.contains("systemusers") && json["systemusers"].isArray())
//        {
//            QJsonArray userArray = json["systemusers"].toArray();
//            users.clear();
//            users.reserve(userArray.size());

//            for(int uIdx=0;uIdx<userArray.size();uIdx++)
//            {
//                QJsonObject userObject = userArray[uIdx].toObject();
//                user user;
//                user.read(userObject);
//                users.append(user);
//            }
//        }

        //load cameras
        if (json.contains("cameras") && json["cameras"].isArray())
        {
            QJsonArray slavesArray = json["cameras"].toArray();
            hmi_slaves.clear();
            hmi_slaves.reserve(slavesArray.size());

            for(int sIdx=0;sIdx<slavesArray.size();sIdx++)
            {
                QJsonObject slaveObject = slavesArray[sIdx].toObject();
                camera *s = new camera();
                s->read(slaveObject);
                hmi_slaves.append(s);
            }
        }


        //load slaves
        if (json.contains("slaves") && json["slaves"].isArray())
        {
            QJsonArray slavesArray = json["slaves"].toArray();
            papis_slaves.clear();
            papis_slaves.reserve(slavesArray.size());

            for(int sIdx=0;sIdx<slavesArray.size();sIdx++)
            {
                QJsonObject slaveObject = slavesArray[sIdx].toObject();
                slave *s = new slave();
                s->read(slaveObject);
                papis_slaves.append(s);
            }
        }
        loadFile.close();
    }
    return true;
}

void populate_default_config()
{
     systemconfig.mpu_id="";
     systemconfig.hw_id="";
     systemconfig.coach_id="";
     systemconfig.coach_no= "";

     systemconfig.gsm_status="ACTIVE";
     systemconfig.gsm_imei="";
     systemconfig.gsm_mdisdn="";
     systemconfig.gsm_apn="";

     systemconfig.eth_ip_mode="STATIC";
     systemconfig.eth_ip_addr="192.168.0.100";
     systemconfig.eth_net_mask="255.255.255.0";
     systemconfig.eth_gateway="192.168.0.1";
     systemconfig.eth_dns="192.168.0.1";

     systemconfig.wifi_mode="STATION";
     systemconfig.wifi_ap_ssid="";
     systemconfig.wifi_ap_pswd="";
     systemconfig.wifi_st_ssid="BMU_WIFI";
     systemconfig.wifi_st_pswd="123456789";
     systemconfig.wifi_ip_mode="DHCP";
     systemconfig.wifi_ip_addr="";
     systemconfig.wifi_net_mask="";
     systemconfig.wifi_gateway="";
     systemconfig.wifi_dns="";

     systemconfig.gps_source="GPS";
     systemconfig.gps_port="";

     systemconfig.dvi_active="INACTIVE";

     systemconfig.mem_int_ssd_mnt="ACTIVE";
     systemconfig.mem_ext_ssd1_mnt="ACTIVE";
     systemconfig.mem_ext_ssd2_mnt="ACTIVE";
     systemconfig.mem_sd_mnt="ACTIVE";
     systemconfig.mem_usb1_mnt="ACTIVE";
     systemconfig.mem_usb2_mnt="ACTIVE";

     systemconfig.ws_status="ACTIVE";
     systemconfig.ws_ser_url="";
     systemconfig.ws_ftp_url="";

     systemconfig.bmu_status="ACTIVE";
     systemconfig.bmu_ser_ip="";
     systemconfig.bmu_ftp_ip="";
     systemconfig.bmu_ftp_user="";
     systemconfig.bmu_ftp_pswd="";
     systemconfig.bmu_ftp_path="";

     systemconfig.pa_vol="100";
}
