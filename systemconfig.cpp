#include "systemconfig.h"

systemconfig::systemconfig()
{

}

void systemconfig::write(QJsonObject &json) const
{
    json["mpu_id"]=mpu_id;
    json["hw_id"]=hw_id;
    json["coach_id"]=coach_id;
    json["coach_no"]=coach_no;
    json["gsm_status"]=gsm_status;
    json["gsm_imei"]=gsm_imei;
    json["gsm_mdisdn"]=gsm_mdisdn;
    json["gsm_apn"]=gsm_apn;
    json["eth_ip_mode"]=eth_ip_mode;
    json["eth_ip_addr"]=eth_ip_addr;
    json["eth_net_mask"]=eth_net_mask;
    json["eth_gateway"]=eth_gateway;
    json["eth_dns"]=eth_dns;
    json["wifi_mode"]=wifi_mode;
    json["wifi_ap_ssid"]=wifi_ap_ssid;
    json["wifi_ap_pswd"]=wifi_ap_pswd;
    json["wifi_st_ssid"]=wifi_st_ssid;
    json["wifi_st_pswd"]=wifi_st_pswd;
    json["wifi_ip_mode"]=wifi_ip_mode;
    json["wifi_ip_addr"]=wifi_ip_addr;
    json["wifi_net_mask"]=wifi_net_mask;
    json["wifi_gateway"]=wifi_gateway;
    json["wifi_dns"]=wifi_dns;
    json["gps_source"]=gps_source;
    json["gps_port"]=gps_port;
    json["dvi_active"]=dvi_active;
    json["mem_int_ssd_mnt"]=mem_int_ssd_mnt;
    json["mem_ext_ssd1_mnt"]=mem_ext_ssd1_mnt;
    json["mem_ext_ssd2_mnt"]=mem_ext_ssd2_mnt;
    json["mem_sd_mnt"]=mem_sd_mnt;
    json["mem_usb1_mnt"]=mem_usb1_mnt;
    json["mem_usb2_mnt"]=mem_usb2_mnt;
    json["ws_status"]=ws_status;
    json["ws_ser_url"]=ws_ser_url;
    json["ws_ftp_url"]=ws_ftp_url;
    json["bmu_status"]=bmu_status;
    json["bmu_ser_ip"]=bmu_ser_ip;
    json["bmu_ftp_ip"]=bmu_ftp_ip;
    json["bmu_ftp_user"]=bmu_ftp_user;
    json["bmu_ftp_pswd"]=bmu_ftp_pswd;
    json["bmu_ftp_path"]=bmu_ftp_path;
    json["pa_vol"]=pa_vol;
    json["led_pis_speed"]=led_pis_speed;
}

void systemconfig::read(const QJsonObject &json)
{
    if (json.contains("mpu_id") && json["mpu_id"].isString())
        mpu_id = json["mpu_id"].toString();

    if (json.contains("hw_id") && json["hw_id"].isString())
        hw_id = json["hw_id"].toString();

    if (json.contains("coach_id") && json["coach_id"].isString())
        coach_id = json["coach_id"].toString();

    if (json.contains("coach_no") && json["coach_no"].isString())
        coach_no = json["coach_no"].toString();

    if (json.contains("gsm_status") && json["gsm_status"].isString())
        gsm_status = json["gsm_status"].toString();

    if (json.contains("gsm_imei") && json["gsm_imei"].isString())
        gsm_imei = json["gsm_imei"].toString();

    if (json.contains("gsm_mdisdn") && json["gsm_mdisdn"].isString())
        gsm_mdisdn = json["gsm_mdisdn"].toString();

    if (json.contains("gsm_apn") && json["gsm_apn"].isString())
        gsm_apn = json["gsm_apn"].toString();

    if (json.contains("eth_ip_mode") && json["eth_ip_mode"].isString())
        eth_ip_mode = json["eth_ip_mode"].toString();

    if (json.contains("eth_ip_addr") && json["eth_ip_addr"].isString())
        eth_ip_addr = json["eth_ip_addr"].toString();

    if (json.contains("eth_net_mask") && json["eth_net_mask"].isString())
        eth_net_mask = json["eth_net_mask"].toString();

    if (json.contains("eth_gateway") && json["eth_gateway"].isString())
        eth_gateway = json["eth_gateway"].toString();

    if (json.contains("eth_dns") && json["eth_dns"].isString())
        eth_dns = json["eth_dns"].toString();

    if (json.contains("wifi_mode") && json["wifi_mode"].isString())
        wifi_mode = json["wifi_mode"].toString();

    if (json.contains("wifi_ap_ssid") && json["wifi_ap_ssid"].isString())
        wifi_ap_ssid = json["wifi_ap_ssid"].toString();

    if (json.contains("wifi_ap_pswd") && json["wifi_ap_pswd"].isString())
        wifi_ap_pswd = json["wifi_ap_pswd"].toString();

    if (json.contains("wifi_st_ssid") && json["wifi_st_ssid"].isString())
        wifi_st_ssid = json["wifi_st_ssid"].toString();

    if (json.contains("wifi_st_pswd") && json["wifi_st_pswd"].isString())
        wifi_st_pswd = json["wifi_st_pswd"].toString();

    if (json.contains("wifi_ip_mode") && json["wifi_ip_mode"].isString())
        wifi_ip_mode = json["wifi_ip_mode"].toString();

    if (json.contains("wifi_ip_addr") && json["wifi_ip_addr"].isString())
        wifi_ip_addr = json["wifi_ip_addr"].toString();

    if (json.contains("wifi_net_mask") && json["wifi_net_mask"].isString())
        wifi_net_mask = json["wifi_net_mask"].toString();

    if (json.contains("wifi_gateway") && json["wifi_gateway"].isString())
        wifi_gateway = json["wifi_gateway"].toString();

    if (json.contains("wifi_dns") && json["wifi_dns"].isString())
        wifi_dns = json["wifi_dns"].toString();

    if (json.contains("gps_source") && json["gps_source"].isString())
        gps_source = json["gps_source"].toString();

    if (json.contains("gps_port") && json["gps_port"].isString())
        gps_port = json["gps_port"].toString();

    if (json.contains("dvi_active") && json["dvi_active"].isString())
        dvi_active = json["dvi_active"].toString();

    if (json.contains("mem_int_ssd_mnt") && json["mem_int_ssd_mnt"].isString())
        mem_int_ssd_mnt = json["mem_int_ssd_mnt"].toString();

    if (json.contains("mem_ext_ssd1_mnt") && json["mem_ext_ssd1_mnt"].isString())
        mem_ext_ssd1_mnt = json["mem_ext_ssd1_mnt"].toString();

    if (json.contains("mem_ext_ssd2_mnt") && json["mem_ext_ssd2_mnt"].isString())
        mem_ext_ssd2_mnt = json["mem_ext_ssd2_mnt"].toString();

    if (json.contains("mem_sd_mnt") && json["mem_sd_mnt"].isString())
        mem_sd_mnt = json["mem_sd_mnt"].toString();

    if (json.contains("mem_usb1_mnt") && json["mem_usb1_mnt"].isString())
        mem_usb1_mnt = json["mem_usb1_mnt"].toString();

    if (json.contains("mem_usb2_mnt") && json["mem_usb2_mnt"].isString())
        mem_usb2_mnt = json["mem_usb2_mnt"].toString();

    if (json.contains("ws_status") && json["ws_status"].isString())
        ws_status = json["ws_status"].toString();

    if (json.contains("ws_ser_url") && json["ws_ser_url"].isString())
        ws_ser_url = json["ws_ser_url"].toString();

    if (json.contains("ws_ftp_url") && json["ws_ftp_url"].isString())
        ws_ftp_url = json["ws_ftp_url"].toString();

    if (json.contains("bmu_status") && json["bmu_status"].isString())
        bmu_status = json["bmu_status"].toString();

    if (json.contains("bmu_ser_ip") && json["bmu_ser_ip"].isString())
        bmu_ser_ip = json["bmu_ser_ip"].toString();

    if (json.contains("bmu_ftp_ip") && json["bmu_ftp_ip"].isString())
        bmu_ftp_ip = json["bmu_ftp_ip"].toString();

    if (json.contains("bmu_ftp_user") && json["bmu_ftp_user"].isString())
        bmu_ftp_user = json["bmu_ftp_user"].toString();

    if (json.contains("bmu_ftp_pswd") && json["bmu_ftp_pswd"].isString())
        bmu_ftp_pswd = json["bmu_ftp_pswd"].toString();

    if (json.contains("bmu_ftp_path") && json["bmu_ftp_path"].isString())
        bmu_ftp_path = json["bmu_ftp_path"].toString();

    if (json.contains("pa_vol") && json["pa_vol"].isString())
        pa_vol = json["pa_vol"].toString();

    if (json.contains("led_pis_speed") && json["led_pis_speed"].isDouble())
        led_pis_speed = json["led_pis_speed"].toInt();
}
