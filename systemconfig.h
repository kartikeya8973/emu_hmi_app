#ifndef SYSTEMCONFIG_H
#define SYSTEMCONFIG_H

#include <QObject>
#include <QJsonObject>

class systemconfig
{
//    Q_OBJECT
public:
    systemconfig();

    QString mpu_id="";
    QString hw_id="";
    QString coach_id="";
    QString coach_no="";

    QString gsm_status="";
    QString gsm_imei="";
    QString gsm_mdisdn="";
    QString gsm_apn="";

    QString eth_ip_mode="";
    QString eth_ip_addr="";
    QString eth_net_mask="";
    QString eth_gateway="";
    QString eth_dns="";

    QString wifi_mode="";
    QString wifi_ap_ssid="";
    QString wifi_ap_pswd="";
    QString wifi_st_ssid="";
    QString wifi_st_pswd="";
    QString wifi_ip_mode="";
    QString wifi_ip_addr="";
    QString wifi_net_mask="";
    QString wifi_gateway="";
    QString wifi_dns="";

    QString gps_source="";
    QString gps_port="";

    QString dvi_active="";

    QString mem_int_ssd_mnt="";
    QString mem_ext_ssd1_mnt="";
    QString mem_ext_ssd2_mnt="";
    QString mem_sd_mnt="";
    QString mem_usb1_mnt="";
    QString mem_usb2_mnt="";

    QString ws_status="";
    QString ws_ser_url="";
    QString ws_ftp_url="";

    QString bmu_status="";
    QString bmu_ser_ip="";
    QString bmu_ftp_ip="";
    QString bmu_ftp_user="";
    QString bmu_ftp_pswd="";
    QString bmu_ftp_path="";

    QString pa_vol="";

    int led_pis_speed=50;

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

};

#endif // SYSTEMCONFIG_H
