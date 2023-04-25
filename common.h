#ifndef COMMON_H
#define COMMON_H

#include <QString>

typedef enum device_type_e{

    DEV_DDC,
    DEV_HMI,
    DEV_SIPS,
    DEV_ETB,
    DEV_NVR

}device_type_t;

typedef enum camera_type_e{

    CAM_CCTV_F,
    CAM_CCTV_FR,
    CAM_CCTV_FL,
    CAM_CCTV_R,
    CAM_CCTV_RR,
    CAM_CCTV_RL,
    CAM_CCTV_SL,
    CAM_CCTV_CAB

}camera_type_t;


typedef enum car_type_e{

    CAR_DTC,
    CAR_MC,
    CAR_TC,
    CAR_NDTC

}car_type_t;

typedef enum activeStatus_e{

    INACTIVE,
    ACTIVE

}activeStatus_t;

typedef enum installStatus_e{

    NOT_INSTALLED,
    INSTALLED

}installStatus_t;

typedef enum slave_tasks_e{

    SLAVE_NO_TASK,
    SLAVE_INIT,
    SLAVE_SCAN_IP,
    SLAVE_SCAN_ALL

}slave_tasks_t;

typedef enum languages_e{

        L_ENGLISH,
        L_BANGLA,
        L_GUJARATI,
        L_HINDI,
        L_KANNADA,
        L_MALAYALAM,
        L_MARATHI,
        L_NEPALI,
        L_ODIA,
        L_PUNJABI,
        L_SINDHI,
        L_TAMIL,
        L_TELUGU,
        L_URDU

}languages_t;

//keypad key type
typedef enum keypad_press_e{

    KEY_LEFT,
    KEY_RIGHT,
    KEY_UP,
    KEY_DOWN,
    KEY_MENU

}keypad_press_t;

bool load_config();
bool w_log(QString msg);
bool w_fault(QString msg);

void populate_default_config();

//Change device to csemi for native / hmi for target
static QString device = "hmi";

//Path to directory where device status json is saved
static QString deviceStatusJsonPath = "/home/"+device+"/Downloads/deviceStatus.json";

//Path to Video Archives folder
static QString pathToVidArchives ="/home/"+device+"/VidArchives/";

//Path to driver login database (passdb_driver)
static QString pathTopassdb_driver = "/home/"+device+"/HMI/password_driver.db";
//static QString pathTopassdb_driver = "/home/csemi/qtworkspace_new/HMItemplate/password_driver.db";

//Path to ETB Archives folder
static QString pathToEtbArchives = "/home/"+device+"/ETBArchives/";

//Path to external storage devices on device
static QString pathToExternalStorage = "/media/"+device+"/";

//Path to home directory on device
static QString pathToHome = "/home/"+device+"/";

//Path to directory where stream list json is saved (downloaded from NVR)
static QString streamListJsonPath="/home/"+device+"/Downloads/streamList.json";

//Path to maintaince login database (passdb)
static QString pathTopassdb = "/home/"+device+"/HMI/password.db";
//static QString pathTopassdb = "/home/csemi/qtworkspace_new/HMItemplate/password.db";

//Path to logs directory
static QString pathToLogs = "/home/"+device+"/logs/";

//Path for Creating VidArchives folder for current date
static QString createVidArchivesFolder ="mkdir /home/"+device+"/VidArchives/$(date +""%Y.%m.%d"")_recordings/";

//Path for Creating EtbArchives folder for current date
static QString createEtbArchivesFolder ="mkdir /home/"+device+"/ETBArchives/$(date +""%Y.%m.%d"")_recordings/";

//Path for Creating logs folder for current date
static QString createLogsArchivesFolder ="mkdir /home/"+device+"/logs/$(date +""%Y.%m.%d"")/";

//Path to base directory
static QString base_dir ="/home/"+device+"/HMI/";
//static QString base_dir ="/home/"+device+"/qtworkspace_new/HMItemplate/";

#endif // COMMON_H
