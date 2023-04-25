#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "logindialog.h"
#include "infowindow.h"
#include "devicewindow.h"
#include "videoarchivewindow.h"
#include "etbarchivewindow.h"
#include "nvrwindow.h"
#include "settingswindow.h"
#include "logswindow.h"
#include "etbcallwindow.h"
#include "slave.h"

#include <QMediaPlayer>
#include <QVideoWidget>
#include <QtNetwork>

#include <QVBoxLayout>
#include <vlc/vlc.h>
#include <QWidget>
#include <QList>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void playVideo();

    QTcpServer *tcp =  nullptr;
    QTcpSocket *socket = nullptr;

#ifdef Q_WS_X11
    QX11EmbedContainer *_videoWidget;
#else
    //    QFrame *_videoWidget;
#endif
    // [20101215 JG] If KDE is used like unique desktop environment, only use QFrame *_videoWidget;
    bool _isPlaying;
    //libvlc_exception_t _vlcexcep; // [20101215 JG] Used for versions prior to VLC 1.2.0.
    libvlc_instance_t *_vlcinstance;
    // For Camview1 - 1st Frame
    libvlc_media_player_t *_mp;
    libvlc_media_t *_m;

    // For Camview1 - 2nd Frame
    libvlc_media_player_t *_mp2;
    libvlc_media_t *_m2;

    // For Camview1 - 3rd Frame
    libvlc_media_player_t *_mp3;
    libvlc_media_t *_m3;

    // For Camview1 - 4th Frame
    libvlc_media_player_t *_mp4;
    libvlc_media_t *_m4;

    // For Camview1 - 5th Frame
    libvlc_media_player_t *_mp5;
    libvlc_media_t *_m5;

    // For CamMosaicView - 1st Frame
    libvlc_media_player_t *_mp6;
    libvlc_media_t *_m6;

    // For CamMosaicView - 2nd Frame
    libvlc_media_player_t *_mp7;
    libvlc_media_t *_m7;

    // For CamMosaicView - 3rd Frame
    libvlc_media_player_t *_mp8;
    libvlc_media_t *_m8;

    // For CamMosaicView - 4th Frame
    libvlc_media_player_t *_mp9;
    libvlc_media_t *_m9;

    // For CamFullView - 1st Frame
    libvlc_media_player_t *_mp10;
    libvlc_media_t *_m10;

    // For CCTV view on incoming Call from ETB
    libvlc_media_player_t *_mp11;
    libvlc_media_t *_m11;

    // For IPCAM view on pushing frame's button
    libvlc_media_player_t *_mp12;
    libvlc_media_t *_m12;

    // When train stops REAR CAM
    libvlc_media_player_t *_mp13;
    libvlc_media_t *_m13;

    // When train stops REAR LEFT
    libvlc_media_player_t *_mp14;
    libvlc_media_t *_m14;

    // When train stops REAR RIGHT
    libvlc_media_player_t *_mp15;
    libvlc_media_t *_m15;

    // When train stops REAR CAM 2
    libvlc_media_player_t *_mp16;
    libvlc_media_t *_m16;

signals:
    // signal emitted when left button of Full Cam View window is pressed
    void left_full_clicked();

    void closeetbcallwindow();

    void etbdialogopen();


public slots:
    void statusDateTime();

    //opens 5 frame Cam View
    //    void openDefaultCam();

    //opens the menu page present in stackWidget_Dynamic via login window
    void openMenuPage();

    //opens the menu page present in stackWidget_Dynamic via return button
    void openMenuPagereturn();

    //opens the 5 camview page (index 2) present in stackWidget_Dynamic
    void openHomePage();

    //open the login dialog
    void openlogindialog();

    //For getting rtsp streams
    void playStream();

    //For opening etbcallwindow
    void openetbcallwindow();

    //For etb call recording continue dialog
    void openetbcalldialog();

    //open driver login dialog
    //    void opendriverlogindialog();

    //open the default screen
    void opendefaultScreen();

    //Iterate Full Cam view
    void iterateFullCamView();

    //player frame 4 (_mp4)
    void playerFrame4();

    //player frame 5 (_mp5)
    void playerFrame5();

    //player frame 6 (_mp6)
    void playerMosiac1();

    //player frame 7 (_mp7)
    void playerMosiac2();

    //player frame 8 (_mp8)
    void playerMosiac3();

    //player frame 9 (_mp9)
    void playerMosiac4();

    //player frame 10 (_mp10)
    void playerFullCam();

    //player frame 12 (_mp12)
    void recPlayer();

    //Iterate Mosiac Cam view
    void iterateMosiacCamView();

    //Iterate the top row of cameras on the first view
    void iterateExternalCams();

    //Iterate the saloon cameras on the first view
    void iterateSaloonCams();


    //open screenshot window
    void openscreenshotdialog();

    //for downloading logs from NVR
    void replyNVR (QNetworkReply *replyStream);

    //Function to download logs from NVR
    void downloadLogs();

    //for renaming audio file that is received from ddc after every call
    void renameEtbAudio();

    //Function for open recording page for IPCAM if yes is selected on etbcallprompt dialog
    void contRecording();

    //Getting list of relevant devices of network
    void slave_ping_update(/*int pc,*/QList <slave*> slaves);

    //Getting list of IPCAMS
    void camera_ping_update(/*int pc,*/QList <camera*> cameras);

    //function to check LAN connectivity of HMI
    void self_ping_lan_icon();

    //function to check LAN connectivity of SIPSERVER
    void ping_voip_icon();

    //function to check wifi connectivity of HMI
    void wifi_status();

    //function to check 4G connectivity of HMI
    void ppp_status();

    //stop view 1 cams
    void stop_view_1_cam();

    //stop Mosiac view cams
    void stop_view_Mosiac_cam();

    //stop all cameras
    void stop_all_cam();

    //getting logs got IPCAMS
    void iterateCamLogs();

private slots:
    void on_pushButton_Menu_clicked();

    void on_pushButton_camView_1_clicked();

    void on_pushButton_camView_2_clicked();

    void on_pushButton_camView_full_clicked();

    void on_pushButton_info_clicked();

    void on_pushButton_deviceStatus_clicked();

    void on_pushButton_vidArchive_clicked();

    void on_pushButton_etbArchive_clicked();

    void on_pushButton_nvr_clicked();

    void on_pushButton_settings_clicked();

    void on_pushButton_logs_clicked();

    void on_pushButton_return_clicked();

    void on_pushButton_test_clicked();

    void on_pushButton_left_up_clicked();

    void on_pushButton_left_down_clicked();

    void on_pushButton_right_up_clicked();

    void on_pushButton_right_down_clicked();

    void on_pushButton_left_clicked();

    void on_pushButton_right_clicked();

    void on_pushButton_left_full_clicked();

    void on_pushButton_right_full_clicked();

    void etb_connected();

    void etb_ready_read();

    void on_pushButton_frame_1_clicked();

    void on_pushButton_frame_2_clicked();

    void on_pushButton_frame_3_clicked();

    void on_pushButton_frame_4_clicked();

    void on_pushButton_frame_5_clicked();

    void on_pushButton_frame_6_clicked();

    void on_pushButton_frame_7_clicked();

    void on_pushButton_frame_8_clicked();

    void on_pushButton_frame_9_clicked();

    void on_pushButton_record_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_frame_10_clicked();

    void train_stops_connected();

    void train_stops_readyRead();

    void on_pushButton_car1_clicked();

    void on_pushButton_car2_clicked();

    void on_pushButton_car3_clicked();

    void on_pushButton_car12_clicked();

    void on_pushButton_car4_clicked();

    void on_pushButton_car5_clicked();

    void on_pushButton_car6_clicked();

    void on_pushButton_car7_clicked();

    void on_pushButton_car8_clicked();

    void on_pushButton_car9_clicked();

    void on_pushButton_car10_clicked();

    void on_pushButton_car11_clicked();

    void on_pushButton_screenshot_clicked();

    void on_pushButton_Lock_clicked();

    void on_pushButton_DriverAccess_clicked();

    void on_pushButton_MainAccess_clicked();

    void on_pushButton_carM1_clicked();

    void on_pushButton_carM2_clicked();

    void on_pushButton_carM3_clicked();

    void on_pushButton_carM4_clicked();

    void on_pushButton_carM5_clicked();

    void on_pushButton_carM6_clicked();

    void on_pushButton_carM7_clicked();

    void on_pushButton_carM8_clicked();

    void on_pushButton_carM9_clicked();

    void on_pushButton_carM10_clicked();

    void on_pushButton_carM11_clicked();

    void on_pushButton_carM12_clicked();

    void on_pushButton_carF1_clicked();

    void on_pushButton_carF2_clicked();

    void on_pushButton_carF3_clicked();

    void on_pushButton_carF4_clicked();

    void on_pushButton_carF5_clicked();

    void on_pushButton_carF6_clicked();

    void on_pushButton_carF7_clicked();

    void on_pushButton_carF8_clicked();

    void on_pushButton_carF9_clicked();

    void on_pushButton_carF10_clicked();

    void on_pushButton_carF11_clicked();

    void on_pushButton_carF12_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QTimer *lockScreen;
    QTimer *slaveTimer;

    LoginDialog *loginDialog;
    QMediaPlayer *player;
    QVideoWidget *video;
    QTcpServer *tcpServer =  nullptr;
    QTcpSocket *clientConnnection =  nullptr;

    //For CCTV feed when incoming ETB call
    QTcpServer *etbListner =  nullptr;
    QTcpSocket *etbLocalConnection =  nullptr;
    QTcpSocket *etbSocket =  nullptr;

    //When train train stops
    QTcpServer *trainStopLister =  nullptr;
    QTcpSocket *trainStopConnection =  nullptr;
    QTcpSocket *trainStopSocket =  nullptr;

    QList<int> integerList;

    QTimer* recordTimer;
    int recordTimeCtr;

    // for logs from NVR
    QNetworkAccessManager *managerLogs;

    EtbCallWindow *etbWindow = new EtbCallWindow(this);

    int statusCnt=0;


    //    QTimer *timeractive;
    ;

};
#endif // MAINWINDOW_H
