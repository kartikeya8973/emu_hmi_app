#include "devicewindow.h"
#include "ui_devicewindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include "pingthread.h"
#include "screenshot.h"


extern QElapsedTimer timeractive;


//counter for return button for NVR window
int returncounter_deviceStatus;

extern int ping_for_cam1;
extern int ping_for_cam2;
extern int ping_for_cam3;
extern int ping_for_cam4;
extern int ping_for_cam5;
extern int ping_for_cam6;
extern int ping_for_cam7;
extern int ping_for_cam8;
extern int ping_for_cam9;
extern int ping_for_cam10;
extern int ping_for_cam11;
extern int ping_for_cam12;
extern int ping_for_cam13;
extern int ping_for_cam14;

extern int ping_for_nvr1;
extern int ping_for_nvr2;

//Name by which stream recorded through Device Window is saved in VideoArhives
QString recordedFileNameDevice = "";

//String for recording stream through Device Window
QString recordStringDevice = "";

//Cam no in Device Window
QString camNoFileNameDevice = "";

QString time_text_device_recording = "";
QString date_text_device_recording = "";

DeviceWindow::DeviceWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DeviceWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(statusDateTimeDevice()));
    timer->start(100); //timer to start the data and time on the status bar
    //Adding data and time widgets to the status bar
    ui->statusbar->addPermanentWidget(ui->label_Date);
    ui->statusbar->addPermanentWidget(ui->label_Time);

    //preparation of the vlc command
    const char * const vlc_args[] = {
        "--verbose=2", //be much more verbose then normal for debugging purpose
    };

#ifdef Q_WS_X11
    _videoWidget=new QX11EmbedContainer(this);
#else
    //    _videoWidget=new QFrame(this);
#endif

    _isPlaying=false;

    //create a new libvlc instance
    _vlcinstance_deviceplayer=libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);  //tricky calculation of the char space used

    // Create a media player playing environement
    _mp_deviceplayer = libvlc_media_player_new (_vlcinstance_deviceplayer);

    PingThread *pingthread;
    pingthread = new PingThread();
    pingthread->setObjectName("first thread");
    pingthread->start(QThread::HighestPriority);
}

DeviceWindow::~DeviceWindow()
{
    delete ui;
}

void DeviceWindow::statusDateTimeDevice()
{
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString time_text = time.toString("hh : mm : ss");
    QString date_text = date.toString("dd-MM-yyyy");
    ui->label_Time->setText(time_text);
    ui->label_Date->setText(date_text);

    time_text_device_recording = time.toString("hh.mm.ss");
    date_text_device_recording = date.toString("yyyy.MM.dd");
}

void DeviceWindow::on_pushButton_home_button_clicked()
{
    emit homebuttonPressedDevice();
    //Close the window
    close();

    //timer to keep the window active
    timeractive.start();

}
void DeviceWindow::on_pushButton_return_clicked()
{
    //timer to keep the window active
    timeractive.start();

    //Going to the main page on NVR window

    if(returncounter_deviceStatus == 2)
    {
        libvlc_media_player_play (_mp_deviceplayer);
        ui->stackedWidget->setCurrentIndex(2);
        ui->label_heading->setText(" CAMERA STATUS");
        returncounter_deviceStatus --;
    }

    else if (returncounter_deviceStatus == 1){
        ui->stackedWidget->setCurrentIndex(0);
        ui->label_heading->setText(" DEVICE STATUS");
        returncounter_deviceStatus --;
    }
    //returns to menu page of the Mainwindow
    else if(returncounter_deviceStatus == 0){
        emit returnbuttonPressedDevice();
        close();
    }

}

void DeviceWindow::on_pushButton_nvrStatus_clicked()
{
    //Switch to the second page (Camera Status)
    ui->stackedWidget->setCurrentIndex(1);

    ui->label_heading->setText(" NVR STATUS");

    returncounter_deviceStatus = 1; //signifies we are going to page 2 of stackwidget

    nvrStatus();
}

void DeviceWindow::on_pushButton_camStatus_clicked()
{
    //Switch to the second page (Camera Status)
    ui->stackedWidget->setCurrentIndex(2);

    ui->label_heading->setText(" CAMERA STATUS");

    returncounter_deviceStatus = 1; //signifies we are going to page 3 of stackwidget

    camStatus();
}

void DeviceWindow::nvrStatus()
{
    //NVR1
    if ( ping_for_nvr1 == 0) //Pinging NVR to check if it active
    {
        ui->label_StatusNvr1->setText("IP : 192.168.1.2");
        ui->label_StatusNvr1->setStyleSheet("QLabel { color : green; }");
        ui->label_StatusIconNvr1->setStyleSheet("image: url(:/new/icons/nvr_green.png);");
    }
    else
    {
        ui->label_StatusNvr1->setText("NVR 1 is OFFLINE");
        ui->label_StatusNvr1->setStyleSheet("QLabel { color : red; }");
        ui->label_StatusIconNvr1->setStyleSheet("image: url(:/new/icons/nvr_red.png);");
    }

    //NVR2
    if ( ping_for_nvr2 == 0) //Pinging NVR to check if it active
    {
        ui->label_StatusNvr2->setText("IP : 192.168.1.232");
        ui->label_StatusNvr2->setStyleSheet("QLabel { color : green; }");
        ui->label_StatusIconNvr2->setStyleSheet("image: url(:/new/icons/nvr_green.png);");
    }
    else
    {
        ui->label_StatusNvr2->setText("NVR 2 is OFFLINE");
        ui->label_StatusNvr2->setStyleSheet("QLabel { color : red; }");
        ui->label_StatusIconNvr2->setStyleSheet("image: url(:/new/icons/nvr_red.png);");
    }
}


void DeviceWindow::camStatus()
{
    //CAM1
    //    int ping_for_cam1 = system("ping -c 1 192.168.1.221 > /dev/null 2>&1");
    if ( ping_for_cam1 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam1->setText("IP : 192.168.1.221");
        ui->label_StatusCam1->setStyleSheet("QLabel { color : green; }");
        ui->pushButton_cam1->setStyleSheet("image: url(:/new/icons/cctv_green.png);");

    }
    else
    {
        ui->label_StatusCam1->setText("CAM 1 is OFFLINE");
        ui->label_StatusCam1->setStyleSheet("QLabel { color : red; }");
        ui->pushButton_cam1->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

    //CAM2
    //    int ping_for_cam2 = system("ping -c 1 192.168.1.222 > /dev/null 2>&1");
    if ( ping_for_cam2 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam2->setText("IP : 192.168.1.222");
        ui->label_StatusCam2->setStyleSheet("QLabel { color : green; }");
        ui->pushButton_cam2->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam2->setText("CAM 2 is OFFLINE");
        ui->label_StatusCam2->setStyleSheet("QLabel { color : red; }");
        ui->pushButton_cam2->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

    //CAM3
    //    int ping_for_cam3 = system("ping -c 1 192.168.1.223 > /dev/null 2>&1");
    if ( ping_for_cam3 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam3->setText("IP : 192.168.1.223");
        ui->label_StatusCam3->setStyleSheet("QLabel { color : green; }");
        ui->pushButton_cam3->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam3->setText("CAM 3 is OFFLINE");
        ui->label_StatusCam3->setStyleSheet("QLabel { color : red; }");
        ui->pushButton_cam3->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

    //    int ping_for_cam4 = system("ping -c 1 192.168.1.224 > /dev/null 2>&1");
    if ( ping_for_cam4 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam4->setText("IP : 192.168.1.224");
        ui->label_StatusCam4->setStyleSheet("QLabel { color : green; }");
        ui->pushButton_cam4->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam4->setText("CAM 4 is OFFLINE");
        ui->label_StatusCam4->setStyleSheet("QLabel { color : red; }");
        ui->pushButton_cam4->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

    //    int ping_for_cam5 = system("ping -c 1 192.168.1.225 > /dev/null 2>&1");
    if ( ping_for_cam5 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam5->setText("IP : 192.168.1.225");
        ui->label_StatusCam5->setStyleSheet("QLabel { color : green; }");
        ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam5->setText("CAM 5 is OFFLINE");
        ui->label_StatusCam5->setStyleSheet("QLabel { color : red; }");
        ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

    //    int ping_for_cam6 = system("ping -c 1 192.168.1.226 > /dev/null 2>&1");
    if ( ping_for_cam6 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam6->setText("IP : 192.168.1.226");
        ui->label_StatusCam6->setStyleSheet("QLabel { color : green; }");
        ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam6->setText("CAM 6 is OFFLINE");
        ui->label_StatusCam6->setStyleSheet("QLabel { color : red; }");
        ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

    //    int ping_for_cam7 = system("ping -c 1 192.168.1.227 > /dev/null 2>&1");
    if ( ping_for_cam7 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam7->setText("IP : 192.168.1.227");
        ui->label_StatusCam7->setStyleSheet("QLabel { color : green; }");
        ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam7->setText("CAM 7 is OFFLINE");
        ui->label_StatusCam7->setStyleSheet("QLabel { color : red; }");
        ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

    //    int ping_for_cam8 = system("ping -c 1 192.168.1.228 > /dev/null 2>&1");
    if ( ping_for_cam8 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam8->setText("IP : 192.168.1.228");
        ui->label_StatusCam8->setStyleSheet("QLabel { color : green; }");
        ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam8->setText("CAM 8 is OFFLINE");
        ui->label_StatusCam8->setStyleSheet("QLabel { color : red; }");
        ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

    //    int ping_for_cam9 = system("ping -c 1 192.168.1.229 > /dev/null 2>&1");
    if ( ping_for_cam9 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam9->setText("IP : 192.168.1.229");
        ui->label_StatusCam9->setStyleSheet("QLabel { color : green; }");
        ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam9->setText("CAM 9 is OFFLINE");
        ui->label_StatusCam9->setStyleSheet("QLabel { color : red; }");
        ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

    //    int ping_for_cam10 = system("ping -c 1 192.168.1.230 > /dev/null 2>&1");
    if ( ping_for_cam10 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam10->setText("IP : 192.168.1.230");
        ui->label_StatusCam10->setStyleSheet("QLabel { color : green; }");
        ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam10->setText("CAM 10 is OFFLINE");
        ui->label_StatusCam10->setStyleSheet("QLabel { color : red; }");
        ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

    //    int ping_for_cam11 = system("ping -c 1 192.168.1.231 > /dev/null 2>&1");
    if ( ping_for_cam11 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam11->setText("IP : 192.168.1.231");
        ui->label_StatusCam11->setStyleSheet("QLabel { color : green; }");
        ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam11->setText("CAM 11 is OFFLINE");
        ui->label_StatusCam11->setStyleSheet("QLabel { color : red; }");
        ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

    //    int ping_for_cam12 = system("ping -c 1 192.168.1.232 > /dev/null 2>&1");
    if ( ping_for_cam12 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam12->setText("IP : 192.168.1.232");
        ui->label_StatusCam12->setStyleSheet("QLabel { color : green; }");
        ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam12->setText("CAM 12 is OFFLINE");
        ui->label_StatusCam12->setStyleSheet("QLabel { color : red; }");
        ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

    if ( ping_for_cam13 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam13->setText("IP : 192.168.1.233");
        ui->label_StatusCam13->setStyleSheet("QLabel { color : green; }");
        ui->pushButton_cam13->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam13->setText("CAM 13 is OFFLINE");
        ui->label_StatusCam13->setStyleSheet("QLabel { color : red; }");
        ui->pushButton_cam13->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

    if ( ping_for_cam14 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam14->setText("IP : 192.168.1.234");
        ui->label_StatusCam14->setStyleSheet("QLabel { color : green; }");
        ui->pushButton_cam14->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam14->setText("CAM 14 is OFFLINE");
        ui->label_StatusCam14->setStyleSheet("QLabel { color : red; }");
        ui->pushButton_cam14->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }
}





void DeviceWindow::on_pushButton_sendDeviceStatus_clicked()
{
    // create JSON object
    QJsonObject jsonObj;
    jsonObj["StatusNVR1"] = ping_for_nvr1;
    jsonObj["StatusNVR2"] = ping_for_nvr2;
    jsonObj["StatusIPCAM1"] = ping_for_cam1;
    jsonObj["StatusIPCAM2"] = ping_for_cam2;
    jsonObj["StatusIPCAM3"] = ping_for_cam3;
    jsonObj["StatusIPCAM4"] = ping_for_cam4;
    jsonObj["StatusIPCAM5"] = ping_for_cam5;
    jsonObj["StatusIPCAM6"] = ping_for_cam6;
    jsonObj["StatusIPCAM7"] = ping_for_cam7;
    jsonObj["StatusIPCAM8"] = ping_for_cam8;
    jsonObj["StatusIPCAM9"] = ping_for_cam9;
    jsonObj["StatusIPCAM10"] = ping_for_cam10;
    jsonObj["StatusIPCAM11"] = ping_for_cam11;
    jsonObj["StatusIPCAM12"] = ping_for_cam12;
    jsonObj["StatusIPCAM13"] = ping_for_cam13;
    jsonObj["StatusIPCAM14"] = ping_for_cam14;


    // convert JSON object to JSON document
    QJsonDocument jsonDoc(jsonObj);

    // convert JSON document to a QByteArray
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Indented);

    QFile file("/home/hmi/Downloads/deviceStatus.json");
    //    QFile file("/home/csemi/Downloads/deviceStatus.json");
    file.open(QIODevice::WriteOnly);
    file.write(jsonData);
    file.close();

    // Create a QTcpSocket instance
    QTcpSocket sendDeviceStatus;
    sendDeviceStatus.connectToHost("192.168.1.50", 1234); //IP address of DDC
    if (sendDeviceStatus.waitForConnected()) {

        // Send the JSON data over the socket
        sendDeviceStatus.write(jsonData);
        sendDeviceStatus.flush(); // Important: Make sure to flush the socket to ensure all data is sent

        // Wait for the server's response
        if (sendDeviceStatus.waitForReadyRead()) {
            QByteArray response = sendDeviceStatus.readAll();
            qDebug() << "Server response:" << response;
            ui->label_sendDeviceStatus->setText(response);
            ui->label_sendDeviceStatus->setStyleSheet("QLabel {color : green;}");
            QTimer::singleShot(5000, this, [this] () { ui->label_sendDeviceStatus->setText(""); });
        }
    } else {
        qDebug() << "Connection error:" << sendDeviceStatus.errorString();
        ui->label_sendDeviceStatus->setText(sendDeviceStatus.errorString());
        ui->label_sendDeviceStatus->setStyleSheet("QLabel {color : red;}");
        QTimer::singleShot(5000, this, [this] () { ui->label_sendDeviceStatus->setText(""); });
    }

    // Close the socket when done
    sendDeviceStatus.close();
}


void DeviceWindow::on_pushButton_cam1_clicked()
{

    ui->stackedWidget->setCurrentIndex(3);

    ui->label_heading->setText( " FRONT CAMERA");

    const char* url = "rtsp://192.168.1.221/video1.sdp";
    _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
    libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );

    libvlc_media_player_play (_mp_deviceplayer);
    _isPlaying=true;

    ui->label_cam_info->setText("IP - 192.168.1.221");

    returncounter_deviceStatus = 2;
    recordStringDevice = "rtsp://192.168.1.221/video1.sdp";
    camNoFileNameDevice = "CAM_1";

}


void DeviceWindow::on_pushButton_cam2_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);

    ui->label_heading->setText( " SALOON CAM 1");

    const char* url = "rtsp://192.168.1.222/video1.sdp";
    _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
    libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );

    libvlc_media_player_play (_mp_deviceplayer);
    _isPlaying=true;

    ui->label_cam_info->setText("IP - 192.168.1.222");

    returncounter_deviceStatus = 2;
    recordStringDevice = "rtsp://192.168.1.222/video1.sdp";
    camNoFileNameDevice = "CAM_2";

}


void DeviceWindow::on_pushButton_cam3_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);

    ui->label_heading->setText( " SALOON CAM 2");

    const char* url = "rtsp://192.168.1.2223/video1.sdp";
    _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
    libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );

    libvlc_media_player_play (_mp_deviceplayer);
    _isPlaying=true;

    ui->label_cam_info->setText("IP - 192.168.1.223");

    returncounter_deviceStatus = 2;
    recordStringDevice = "rtsp://192.168.1.223/video1.sdp";
    camNoFileNameDevice = "CAM_3";

}


void DeviceWindow::on_pushButton_cam4_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);

    ui->label_heading->setText( " REAR CAMERA");

    const char* url = "rtsp://192.168.1.224/video1.sdp";
    _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
    libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );

    libvlc_media_player_play (_mp_deviceplayer);
    _isPlaying=true;

    ui->label_cam_info->setText("IP - 192.168.1.224");

    returncounter_deviceStatus = 2;
    recordStringDevice = "rtsp://192.168.1.224/video1.sdp";
    camNoFileNameDevice = "CAM_4";

}


void DeviceWindow::on_pushButton_cam5_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);

    ui->label_heading->setText( " FRONT LEFT CAM");

    const char* url = "rtsp://192.168.1.225/video1.sdp";
    _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
    libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );

    libvlc_media_player_play (_mp_deviceplayer);
    _isPlaying=true;

    ui->label_cam_info->setText("IP - 192.168.1.225");

    returncounter_deviceStatus = 2;
    recordStringDevice = "rtsp://192.168.1.225/video1.sdp";
    camNoFileNameDevice = "CAM_5";

}


void DeviceWindow::on_pushButton_cam6_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);

    ui->label_heading->setText( " SALOON CAM 5");

    const char* url = "rtsp://192.168.1.226/video1.sdp";
    _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
    libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );

    libvlc_media_player_play (_mp_deviceplayer);
    _isPlaying=true;

    ui->label_cam_info->setText("IP - 192.168.1.226");

    returncounter_deviceStatus = 2;
    recordStringDevice = "rtsp://192.168.1.226/video1.sdp";
    camNoFileNameDevice = "CAM_6";

}


void DeviceWindow::on_pushButton_cam7_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);

    ui->label_heading->setText( " SALOON CAM 6");

    const char* url = "rtsp://192.168.1.227/video1.sdp";
    _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
    libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );

    libvlc_media_player_play (_mp_deviceplayer);
    _isPlaying=true;

    ui->label_cam_info->setText("IP - 192.168.1.227");

    returncounter_deviceStatus = 2;
    recordStringDevice = "rtsp://192.168.1.227/video1.sdp";
    camNoFileNameDevice = "CAM_7";
}


void DeviceWindow::on_pushButton_cam8_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);

    ui->label_heading->setText( " SALOON CAM 8");

    const char* url = "rtsp://192.168.1.228/video1.sdp";
    _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
    libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );

    libvlc_media_player_play (_mp_deviceplayer);
    _isPlaying=true;

    ui->label_cam_info->setText("IP - 192.168.1.228");

    returncounter_deviceStatus = 2;
    recordStringDevice = "rtsp://192.168.1.228/video1.sdp";
    camNoFileNameDevice = "CAM_8";

}


void DeviceWindow::on_pushButton_cam9_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);

    ui->label_heading->setText( " FRONT RIGHT CAM");

    const char* url = "rtsp://192.168.1.229/video1.sdp";
    _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
    libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );

    libvlc_media_player_play (_mp_deviceplayer);
    _isPlaying=true;

    ui->label_cam_info->setText("IP - 192.168.1.229");

    returncounter_deviceStatus = 2;
    recordStringDevice = "rtsp://192.168.1.229/video1.sdp";
    camNoFileNameDevice = "CAM_9";

}


void DeviceWindow::on_pushButton_cam10_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);

    ui->label_heading->setText( " SALOON CAM 7");

    const char* url = "rtsp://192.168.1.230/video1.sdp";
    _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
    libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );

    libvlc_media_player_play (_mp_deviceplayer);
    _isPlaying=true;

    ui->label_cam_info->setText("IP - 192.168.1.230");

    returncounter_deviceStatus = 2;
    recordStringDevice = "rtsp://192.168.1.230/video1.sdp";
    camNoFileNameDevice = "CAM_10";

}


void DeviceWindow::on_pushButton_cam11_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);

    ui->label_heading->setText( " SALOON CAM 4");

    const char* url = "rtsp://192.168.1.231/video1.sdp";
    _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
    libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );

    libvlc_media_player_play (_mp_deviceplayer);
    _isPlaying=true;

    ui->label_cam_info->setText("IP - 192.168.1.231");

    returncounter_deviceStatus = 2;
    recordStringDevice = "rtsp://192.168.1.231/video1.sdp";
    camNoFileNameDevice = "CAM_11";

}


void DeviceWindow::on_pushButton_cam12_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);

    ui->label_heading->setText( " SALOON CAM 8");

    const char* url = "rtsp://192.168.1.232/video1.sdp";
    _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
    libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );

    libvlc_media_player_play (_mp_deviceplayer);
    _isPlaying=true;

    ui->label_cam_info->setText("IP - 192.168.1.232");

    returncounter_deviceStatus = 2;
    recordStringDevice = "rtsp://192.168.1.232/video1.sdp";
    camNoFileNameDevice = "CAM_12";
}


void DeviceWindow::on_pushButton_cam13_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);

    ui->label_heading->setText( " REAR LEFT CAM");

    const char* url = "rtsp://192.168.1.233/video1.sdp";
    _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
    libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );

    libvlc_media_player_play (_mp_deviceplayer);
    _isPlaying=true;

    ui->label_cam_info->setText("IP - 192.168.1.233");

    returncounter_deviceStatus = 2;
    recordStringDevice = "rtsp://192.168.1.233/video1.sdp";
    camNoFileNameDevice = "CAM_13";
}


void DeviceWindow::on_pushButton_cam14_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);

    ui->label_heading->setText( " REAR RIGHT CAM");

    const char* url = "rtsp://192.168.1.234/video1.sdp";
    _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
    libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );

    libvlc_media_player_play (_mp_deviceplayer);
    _isPlaying=true;

    ui->label_cam_info->setText("IP - 192.168.1.234");

    returncounter_deviceStatus = 2;

    recordStringDevice = "rtsp://192.168.1.234/video1.sdp";
    camNoFileNameDevice = "CAM_14";
}

void DeviceWindow::on_pushButton_record_clicked()
{
    recordedFileNameDevice = date_text_device_recording + "_" +time_text_device_recording;

    QString forRecordingStream = "gst-launch-1.0 -ev  rtspsrc location=" + recordStringDevice + " ! application/x-rtp, media=video, encoding-name=H264 ! queue ! rtph264depay "
                                                                                                "! h264parse ! matroskamux ! filesink location=/home/hmi/VidArchives/"+date_text_device_recording+"_recordings/"+camNoFileNameDevice+"_"+recordedFileNameDevice+".mp4 &";
    //    QString forRecordingStream = "cvlc -vvv "+ recordString + " --sout=\"#transcode{vcodec=mp4v,vfilter=canvas{width=800,height=600}}:std{access=file,mux=mp4,dst=/home/hmi/VidArchives/24.02.2023_recordings/123.mp4}\" &";
    system(qPrintable(forRecordingStream));

    ui->label_recordingIcon->setStyleSheet("image: url(:/new/icons/recordIcon.png);");
    ui->label_recording_status->setText("RECORDING STARTED");
    ui->pushButton_record->setStyleSheet("background-color: rgb(138, 226, 52);");
    ui->pushButton_stop->setStyleSheet("background-color: rgb(211, 215, 207);");

}

void DeviceWindow::on_pushButton_stop_clicked()
{
    //Stopping gstreamer recording

    system("ps -A | grep gst | awk '{ printf $1 }' | xargs kill -2 $1");

    recordedFileNameDevice = "";
    camNoFileNameDevice = "";

    ui->label_recordingIcon->setStyleSheet("");
    ui->label_recording_status->setText("RECORDING STOPPED");
    ui->pushButton_record->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->pushButton_stop->setStyleSheet("background-color: rgb(239, 41, 41);");

}

//Screenshot
void DeviceWindow::openscreenshotdialog()
{
    Screenshot *screenshot = new Screenshot();
    screenshot->setModal(true);
    screenshot->setWindowFlag(Qt::FramelessWindowHint);
    screenshot->exec();
    timeractive.start();
}

void DeviceWindow::on_pushButton_screenshot_clicked()
{
    openscreenshotdialog();
    //timer to keep the window active
    timeractive.start();
}

