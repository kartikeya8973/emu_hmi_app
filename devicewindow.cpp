#include "devicewindow.h"
#include "ui_devicewindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pingthread.h"
#include "screenshot.h"
#include "common.h"
#include "slavesyncthread.h"
#include "ipcamsyncthread.h"

#include <QDateTime>
#include <QVector>

extern QElapsedTimer timeractive;

extern QList <camera*> hmi_slaves;

extern QList <QList <camera*>> rake;

extern QList<camera*> coach_1;
extern QList<camera*> coach_2;
extern QList<camera*> coach_3;
extern QList<camera*> coach_4;
extern QList<camera*> coach_5;
extern QList<camera*> coach_6;
extern QList<camera*> coach_7;
extern QList<camera*> coach_8;
extern QList<camera*> coach_9;
extern QList<camera*> coach_10;
extern QList<camera*> coach_11;
extern QList<camera*> coach_12;

extern QFile file;

//counter for return button for NVR window
int returncounter_deviceStatus;
int iterate_button_pressed = 0;

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

//URL used by video player
const char* url;

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

    ui->label_StatusCam1->setWordWrap(true);
    ui->label_StatusCam2->setWordWrap(true);
    ui->label_StatusCam3->setWordWrap(true);
    ui->label_StatusCam4->setWordWrap(true);
    ui->label_StatusCam5->setWordWrap(true);
    ui->label_StatusCam6->setWordWrap(true);
    ui->label_StatusCam6->setWordWrap(true);
    ui->label_StatusCam7->setWordWrap(true);
    ui->label_StatusCam8->setWordWrap(true);
    ui->label_StatusCam9->setWordWrap(true);
    ui->label_StatusCam10->setWordWrap(true);
    ui->label_StatusCam11->setWordWrap(true);
    ui->label_StatusCam12->setWordWrap(true);
    ui->label_StatusCam13->setWordWrap(true);
    ui->label_StatusCam14->setWordWrap(true);
    ui->label_StatusCam15->setWordWrap(true);
    ui->label_StatusCam16->setWordWrap(true);

    //Initialising thread for pinging devices
    PingThread *pingthread;
    pingthread = new PingThread();
    pingthread->setObjectName("first thread");
    pingthread->start();

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

void DeviceWindow::videoPlayerD(){
    _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
    libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
    libvlc_media_player_play (_mp_deviceplayer);
    _isPlaying=true;
}


void DeviceWindow::iterateCams()
{
    //COACH 1
    if(iterate_button_pressed == 0){

        //changing labels of device status by iterating the rake list

        frontCoach();

        for (int j = 0; j < coach_1.size(); j++)
        {
            ui->label_coach_name->setText("CAR TYPE - "+coach_1.at(0)->car_type_desc + "  COACH NO - "+QString::number(coach_1.at(0)->csno));
            //Coach 1 IPCAM 1 - FRONT
            if(coach_1.at(0)->activeStatus == ACTIVE){

                ui->label_cam1->setText(coach_1.at(0)->camera_type_desc);ui->label_StatusCam1->setText("IP : " + coach_1.at(0)->ip_addr);
                ui->label_StatusCam1->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam1->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
            }
            else
            {
                ui->label_cam1->setText(coach_1.at(0)->camera_type_desc);ui->label_StatusCam1->setText(coach_1.at(0)->camera_type_desc + " is OFFLINE");
                ui->label_StatusCam1->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam1->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
            }
            //Coach 1 IPCAM 2 - FRONT RIGHT
            if(coach_1.at(1)->activeStatus == ACTIVE){

                ui->label_cam2->setText(coach_1.at(1)->camera_type_desc);ui->label_StatusCam2->setText("IP : " + coach_1.at(1)->ip_addr);
                ui->label_StatusCam2->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam2->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
            }
            else
            {
                ui->label_cam2->setText(coach_1.at(1)->camera_type_desc);ui->label_StatusCam2->setText(coach_1.at(1)->camera_type_desc + " is OFFLINE");
                ui->label_StatusCam2->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam2->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
            }
            //Coach 1 IPCAM 3 - FRONT LEFT
            if(coach_1.at(2)->activeStatus == ACTIVE){

                ui->label_cam3->setText(coach_1.at(2)->camera_type_desc);ui->label_StatusCam3->setText("IP : " + coach_1.at(2)->ip_addr);
                ui->label_StatusCam3->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam3->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
            }
            else
            {
                ui->label_cam3->setText(coach_1.at(2)->camera_type_desc);ui->label_StatusCam3->setText(coach_1.at(2)->camera_type_desc + " is OFFLINE");
                ui->label_StatusCam3->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam3->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
            }
            //Coach 1 IPCAM 4 - FRONT CAB
            if(coach_1.at(3)->activeStatus == ACTIVE){

                ui->label_cam4->setText(coach_1.at(3)->camera_type_desc);ui->label_StatusCam4->setText("IP : " + coach_1.at(3)->ip_addr);
                ui->label_StatusCam4->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam4->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
            }
            else
            {
                ui->label_cam4->setText(coach_1.at(3)->camera_type_desc);ui->label_StatusCam4->setText(coach_1.at(3)->camera_type_desc + " is OFFLINE");
                ui->label_StatusCam4->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam4->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
            }
            //Coach 1 IPCAM 5 - SALOON 1
            if(coach_1.at(4)->activeStatus == ACTIVE){

                ui->label_cam5->setText(coach_1.at(4)->camera_type_desc+" "+QString::number(coach_1.at(4)->sno));ui->label_StatusCam5->setText("IP : " + coach_1.at(4)->ip_addr);
                ui->label_StatusCam5->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam5->setText(coach_1.at(4)->camera_type_desc+" "+QString::number(coach_1.at(4)->sno));ui->label_StatusCam5->setText(coach_1.at(4)->camera_type_desc +" "+QString::number(coach_1.at(4)->sno)+ " is OFFLINE");
                ui->label_StatusCam5->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //Coach 1 IPCAM 6 - SALOON 2
            if(coach_1.at(5)->activeStatus == ACTIVE){

                ui->label_cam6->setText(coach_1.at(5)->camera_type_desc+" "+QString::number(coach_1.at(5)->sno));ui->label_StatusCam6->setText("IP : " + coach_1.at(5)->ip_addr);
                ui->label_StatusCam6->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam6->setText(coach_1.at(5)->camera_type_desc+" "+QString::number(coach_1.at(5)->sno));ui->label_StatusCam6->setText(coach_1.at(5)->camera_type_desc +" "+QString::number(coach_1.at(5)->sno)+ " is OFFLINE");
                ui->label_StatusCam6->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //Coach 1 IPCAM 7 - SALOON 3
            if(coach_1.at(6)->activeStatus == ACTIVE){

                ui->label_cam7->setText(coach_1.at(6)->camera_type_desc+" "+QString::number(coach_1.at(6)->sno));ui->label_StatusCam7->setText("IP : " + coach_1.at(6)->ip_addr);
                ui->label_StatusCam7->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam7->setText(coach_1.at(6)->camera_type_desc+" "+QString::number(coach_1.at(6)->sno));ui->label_StatusCam7->setText(coach_1.at(6)->camera_type_desc + " "+QString::number(coach_1.at(6)->sno)+" is OFFLINE");
                ui->label_StatusCam7->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //Coach 1 IPCAM 8 - SALOON 4
            if(coach_1.at(7)->activeStatus == ACTIVE){

                ui->label_cam8->setText(coach_1.at(7)->camera_type_desc+" "+QString::number(coach_1.at(7)->sno));ui->label_StatusCam8->setText("IP : " + coach_1.at(7)->ip_addr);
                ui->label_StatusCam8->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam8->setText(coach_1.at(7)->camera_type_desc+" "+QString::number(coach_1.at(7)->sno));ui->label_StatusCam8->setText(coach_1.at(7)->camera_type_desc +" "+QString::number(coach_1.at(7)->sno)+ " is OFFLINE");
                ui->label_StatusCam8->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //Coach 1 IPCAM 9 - SALOON 5
            if(coach_1.at(8)->activeStatus == ACTIVE){

                ui->label_cam9->setText(coach_1.at(8)->camera_type_desc+" "+QString::number(coach_1.at(8)->sno));ui->label_StatusCam9->setText("IP : " + coach_1.at(8)->ip_addr);
                ui->label_StatusCam9->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam9->setText(coach_1.at(8)->camera_type_desc+" "+QString::number(coach_1.at(8)->sno));ui->label_StatusCam9->setText(coach_1.at(8)->camera_type_desc+" "+QString::number(coach_1.at(8)->sno) + " is OFFLINE");
                ui->label_StatusCam9->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //Coach 1 IPCAM 10 - SALOON 6
            if(coach_1.at(9)->activeStatus == ACTIVE){

                ui->label_cam10->setText(coach_1.at(9)->camera_type_desc+" "+QString::number(coach_1.at(9)->sno));ui->label_StatusCam10->setText("IP : " + coach_1.at(9)->ip_addr);
                ui->label_StatusCam10->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam10->setText(coach_1.at(9)->camera_type_desc+" "+QString::number(coach_1.at(9)->sno));ui->label_StatusCam10->setText(coach_1.at(9)->camera_type_desc +" "+QString::number(coach_1.at(9)->sno) + " is OFFLINE");
                ui->label_StatusCam10->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //Coach 1 IPCAM 11 - SALOON 7
            if(coach_1.at(10)->activeStatus == ACTIVE){

                ui->label_cam11->setText(coach_1.at(10)->camera_type_desc+" "+QString::number(coach_1.at(10)->sno));ui->label_StatusCam11->setText("IP : " + coach_1.at(10)->ip_addr);
                ui->label_StatusCam11->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam11->setText(coach_1.at(10)->camera_type_desc+" "+QString::number(coach_1.at(10)->sno));ui->label_StatusCam11->setText(coach_1.at(10)->camera_type_desc +" "+QString::number(coach_1.at(10)->sno)+ " is OFFLINE");
                ui->label_StatusCam11->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //Coach 1 IPCAM 12 - SALOON 8
            if(coach_1.at(11)->activeStatus == ACTIVE){

                ui->label_cam12->setText(coach_1.at(11)->camera_type_desc+" "+QString::number(coach_1.at(11)->sno));ui->label_StatusCam12->setText("IP : " + coach_1.at(11)->ip_addr);
                ui->label_StatusCam12->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");

            }
            else
            {
                ui->label_cam12->setText(coach_1.at(11)->camera_type_desc+" "+QString::number(coach_1.at(11)->sno));ui->label_StatusCam12->setText(coach_1.at(11)->camera_type_desc +" "+QString::number(coach_1.at(11)->sno) + " is OFFLINE");
                ui->label_StatusCam12->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
        }
    }
    //    }

    //COACH 2
    else if(iterate_button_pressed == 1){

        coachWithSaloonCam();

        //changing labels of device status by iterating the rake list
        //for (int i = 0; i < rake.size(); i++) {
        //            coach_2 = rake[1];
        for (int j = 0; j < coach_2.size(); j++)
        {
            ui->label_coach_name->setText("CAR TYPE - "+coach_2.at(0)->car_type_desc + "  COACH NO - "+QString::number(coach_2.at(0)->csno));
            //COACH 2 IPCAM 1 - SALOON 1
            if(coach_2.at(0)->activeStatus == ACTIVE){

                ui->label_cam5->setText(coach_2.at(0)->camera_type_desc+" "+QString::number(coach_2.at(0)->sno));ui->label_StatusCam5->setText("IP : " + coach_2.at(0)->ip_addr);
                ui->label_StatusCam5->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam5->setText(coach_2.at(0)->camera_type_desc+" "+QString::number(coach_2.at(0)->sno));ui->label_StatusCam5->setText(coach_2.at(0)->camera_type_desc+" "+QString::number(coach_2.at(0)->sno)+" is OFFLINE");
                ui->label_StatusCam5->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //COACH 2 IPCAM 2 - SALOON 2
            if(coach_2.at(1)->activeStatus == ACTIVE){

                ui->label_cam6->setText(coach_2.at(1)->camera_type_desc+" "+QString::number(coach_2.at(1)->sno));ui->label_StatusCam6->setText("IP : " + coach_2.at(1)->ip_addr);
                ui->label_StatusCam6->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam6->setText(coach_2.at(1)->camera_type_desc+" "+QString::number(coach_2.at(1)->sno));ui->label_StatusCam6->setText(coach_2.at(1)->camera_type_desc+" "+QString::number(coach_2.at(1)->sno)+" is OFFLINE");
                ui->label_StatusCam6->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 2 IPCAM 3 - SALOON 3
            if(coach_2.at(2)->activeStatus == ACTIVE){

                ui->label_cam7->setText(coach_2.at(2)->camera_type_desc+" "+QString::number(coach_2.at(2)->sno));ui->label_StatusCam7->setText("IP : " + coach_2.at(2)->ip_addr);
                ui->label_StatusCam7->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam7->setText(coach_2.at(2)->camera_type_desc+" "+QString::number(coach_2.at(2)->sno));ui->label_StatusCam7->setText(coach_2.at(2)->camera_type_desc+" "+QString::number(coach_2.at(2)->sno)+" is OFFLINE");
                ui->label_StatusCam7->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 2 IPCAM 4 - SALOON 4
            if(coach_2.at(3)->activeStatus == ACTIVE){

                ui->label_cam8->setText(coach_2.at(3)->camera_type_desc+" "+QString::number(coach_2.at(3)->sno));ui->label_StatusCam8->setText("IP : " + coach_2.at(3)->ip_addr);
                ui->label_StatusCam8->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam8->setText(coach_2.at(3)->camera_type_desc+" "+QString::number(coach_2.at(3)->sno));ui->label_StatusCam8->setText(coach_2.at(3)->camera_type_desc+" "+QString::number(coach_2.at(3)->sno)+" is OFFLINE");
                ui->label_StatusCam8->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 2 IPCAM 5 - SALOON 5
            if(coach_2.at(4)->activeStatus == ACTIVE){

                ui->label_cam9->setText(coach_2.at(4)->camera_type_desc+" "+QString::number(coach_2.at(4)->sno));ui->label_StatusCam9->setText("IP : " + coach_2.at(4)->ip_addr);
                ui->label_StatusCam9->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam9->setText(coach_2.at(4)->camera_type_desc+" "+QString::number(coach_2.at(4)->sno));ui->label_StatusCam9->setText(coach_2.at(4)->camera_type_desc+" "+QString::number(coach_2.at(4)->sno)+" is OFFLINE");
                ui->label_StatusCam9->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 2 IPCAM 6 - SALOON 6
            if(coach_2.at(5)->activeStatus == ACTIVE){

                ui->label_cam10->setText(coach_2.at(5)->camera_type_desc+" "+QString::number(coach_2.at(5)->sno));ui->label_StatusCam10->setText("IP : " + coach_2.at(5)->ip_addr);
                ui->label_StatusCam10->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam10->setText(coach_2.at(5)->camera_type_desc+" "+QString::number(coach_2.at(5)->sno));ui->label_StatusCam10->setText(coach_2.at(5)->camera_type_desc+" "+QString::number(coach_2.at(5)->sno)+" is OFFLINE");
                ui->label_StatusCam10->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 2 IPCAM 7 - SALOON 7
            if(coach_2.at(6)->activeStatus == ACTIVE){

                ui->label_cam11->setText(coach_2.at(6)->camera_type_desc+" "+QString::number(coach_2.at(6)->sno));ui->label_StatusCam11->setText("IP : " + coach_2.at(6)->ip_addr);
                ui->label_StatusCam11->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam11->setText(coach_2.at(6)->camera_type_desc+" "+QString::number(coach_2.at(6)->sno));ui->label_StatusCam11->setText(coach_2.at(6)->camera_type_desc+" "+QString::number(coach_2.at(6)->sno)+" is OFFLINE");
                ui->label_StatusCam11->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 2 IPCAM 8 - SALOON 8
            if(coach_2.at(7)->activeStatus == ACTIVE){

                ui->label_cam12->setText(coach_2.at(7)->camera_type_desc+" "+QString::number(coach_2.at(7)->sno));ui->label_StatusCam12->setText("IP : " + coach_2.at(7)->ip_addr);
                ui->label_StatusCam12->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam12->setText(coach_2.at(7)->camera_type_desc+" "+QString::number(coach_2.at(7)->sno));ui->label_StatusCam12->setText(coach_2.at(7)->camera_type_desc+" "+QString::number(coach_2.at(7)->sno)+" is OFFLINE");
                ui->label_StatusCam12->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
        }
    }
    //    }

    //COACH 3
    else if(iterate_button_pressed == 2){

        coachWithSaloonCam();

        //changing labels of device status by iterating the rake list
        //for (int i = 0; i < rake.size(); i++) {
        //            coach_3 = rake[2];
        for (int j = 0; j < coach_3.size(); j++)
        {
            ui->label_coach_name->setText("CAR TYPE - "+coach_3.at(0)->car_type_desc + "  COACH NO - "+QString::number(coach_3.at(0)->csno));
            //COACH 3 IPCAM 1 - SALOON 1
            if(coach_3.at(0)->activeStatus == ACTIVE){

                ui->label_cam5->setText(coach_3.at(0)->camera_type_desc+" "+QString::number(coach_3.at(0)->sno));ui->label_StatusCam5->setText("IP : " + coach_3.at(0)->ip_addr);
                ui->label_StatusCam5->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam5->setText(coach_3.at(0)->camera_type_desc+" "+QString::number(coach_3.at(0)->sno));ui->label_StatusCam5->setText(coach_3.at(0)->camera_type_desc+" "+QString::number(coach_3.at(0)->sno)+" is OFFLINE");
                ui->label_StatusCam5->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //COACH 3 IPCAM 2 - SALOON 2
            if(coach_3.at(1)->activeStatus == ACTIVE){

                ui->label_cam6->setText(coach_3.at(1)->camera_type_desc+" "+QString::number(coach_3.at(1)->sno));ui->label_StatusCam6->setText("IP : " + coach_3.at(1)->ip_addr);
                ui->label_StatusCam6->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam6->setText(coach_3.at(1)->camera_type_desc+" "+QString::number(coach_3.at(1)->sno));ui->label_StatusCam6->setText(coach_3.at(1)->camera_type_desc+" "+QString::number(coach_3.at(1)->sno)+" is OFFLINE");
                ui->label_StatusCam6->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 3 IPCAM 3 - SALOON 3
            if(coach_3.at(2)->activeStatus == ACTIVE){

                ui->label_cam7->setText(coach_3.at(2)->camera_type_desc+" "+QString::number(coach_3.at(2)->sno));ui->label_StatusCam7->setText("IP : " + coach_3.at(2)->ip_addr);
                ui->label_StatusCam7->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam7->setText(coach_3.at(2)->camera_type_desc+" "+QString::number(coach_3.at(2)->sno));ui->label_StatusCam7->setText(coach_3.at(2)->camera_type_desc+" "+QString::number(coach_3.at(2)->sno)+" is OFFLINE");
                ui->label_StatusCam7->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 3 IPCAM 4 - SALOON 4
            if(coach_3.at(3)->activeStatus == ACTIVE){

                ui->label_cam8->setText(coach_3.at(3)->camera_type_desc+" "+QString::number(coach_3.at(3)->sno));ui->label_StatusCam8->setText("IP : " + coach_3.at(3)->ip_addr);
                ui->label_StatusCam8->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam8->setText(coach_3.at(3)->camera_type_desc+" "+QString::number(coach_3.at(3)->sno));ui->label_StatusCam8->setText(coach_3.at(3)->camera_type_desc+" "+QString::number(coach_3.at(3)->sno)+" is OFFLINE");
                ui->label_StatusCam8->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 3 IPCAM 5 - SALOON 5
            if(coach_3.at(4)->activeStatus == ACTIVE){

                ui->label_cam9->setText(coach_3.at(4)->camera_type_desc+" "+QString::number(coach_3.at(4)->sno));ui->label_StatusCam9->setText("IP : " + coach_3.at(4)->ip_addr);
                ui->label_StatusCam9->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam9->setText(coach_3.at(4)->camera_type_desc+" "+QString::number(coach_3.at(4)->sno));ui->label_StatusCam9->setText(coach_3.at(4)->camera_type_desc+" "+QString::number(coach_3.at(4)->sno)+" is OFFLINE");
                ui->label_StatusCam9->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 3 IPCAM 6 - SALOON 6
            if(coach_3.at(5)->activeStatus == ACTIVE){

                ui->label_cam10->setText(coach_3.at(5)->camera_type_desc+" "+QString::number(coach_3.at(5)->sno));ui->label_StatusCam10->setText("IP : " + coach_3.at(5)->ip_addr);
                ui->label_StatusCam10->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam10->setText(coach_3.at(5)->camera_type_desc+" "+QString::number(coach_3.at(5)->sno));ui->label_StatusCam10->setText(coach_3.at(5)->camera_type_desc+" "+QString::number(coach_3.at(5)->sno)+" is OFFLINE");
                ui->label_StatusCam10->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 3 IPCAM 7 - SALOON 7
            if(coach_3.at(6)->activeStatus == ACTIVE){

                ui->label_cam11->setText(coach_3.at(6)->camera_type_desc+" "+QString::number(coach_3.at(6)->sno));ui->label_StatusCam11->setText("IP : " + coach_3.at(6)->ip_addr);
                ui->label_StatusCam11->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam11->setText(coach_3.at(6)->camera_type_desc+" "+QString::number(coach_3.at(6)->sno));ui->label_StatusCam11->setText(coach_3.at(6)->camera_type_desc+" "+QString::number(coach_3.at(6)->sno)+" is OFFLINE");
                ui->label_StatusCam11->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 3 IPCAM 8 - SALOON 8
            if(coach_3.at(7)->activeStatus == ACTIVE){

                ui->label_cam12->setText(coach_3.at(7)->camera_type_desc+" "+QString::number(coach_3.at(7)->sno));ui->label_StatusCam12->setText("IP : " + coach_3.at(7)->ip_addr);
                ui->label_StatusCam12->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam12->setText(coach_3.at(7)->camera_type_desc+" "+QString::number(coach_3.at(7)->sno));ui->label_StatusCam12->setText(coach_3.at(7)->camera_type_desc+" "+QString::number(coach_3.at(7)->sno)+" is OFFLINE");
                ui->label_StatusCam12->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
        }
    }
    //    }
    //COACH 4
    else if(iterate_button_pressed == 3){

        coachWithSaloonCam();

        //changing labels of device status by iterating the rake list
        //for (int i = 0; i < rake.size(); i++) {
        //            coach_4 = rake[3];
        for (int j = 0; j < coach_4.size(); j++)
        {
            ui->label_coach_name->setText("CAR TYPE - "+coach_4.at(0)->car_type_desc + "  COACH NO - "+QString::number(coach_4.at(0)->csno));
            //COACH 4 IPCAM 1 - SALOON 1
            if(coach_4.at(0)->activeStatus == ACTIVE){

                ui->label_cam5->setText(coach_4.at(0)->camera_type_desc+" "+QString::number(coach_4.at(0)->sno));ui->label_StatusCam5->setText("IP : " + coach_4.at(0)->ip_addr);
                ui->label_StatusCam5->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam5->setText(coach_4.at(0)->camera_type_desc+" "+QString::number(coach_4.at(0)->sno));ui->label_StatusCam5->setText(coach_4.at(0)->camera_type_desc+" "+QString::number(coach_4.at(0)->sno)+" is OFFLINE");
                ui->label_StatusCam5->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //COACH 4 IPCAM 2 - SALOON 2
            if(coach_4.at(1)->activeStatus == ACTIVE){

                ui->label_cam6->setText(coach_4.at(1)->camera_type_desc+" "+QString::number(coach_4.at(1)->sno));ui->label_StatusCam6->setText("IP : " + coach_4.at(1)->ip_addr);
                ui->label_StatusCam6->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam6->setText(coach_4.at(1)->camera_type_desc+" "+QString::number(coach_4.at(1)->sno));ui->label_StatusCam6->setText(coach_4.at(1)->camera_type_desc+" "+QString::number(coach_4.at(1)->sno)+" is OFFLINE");
                ui->label_StatusCam6->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 4 IPCAM 3 - SALOON 3
            if(coach_4.at(2)->activeStatus == ACTIVE){

                ui->label_cam7->setText(coach_4.at(2)->camera_type_desc+" "+QString::number(coach_4.at(2)->sno));ui->label_StatusCam7->setText("IP : " + coach_4.at(2)->ip_addr);
                ui->label_StatusCam7->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam7->setText(coach_4.at(2)->camera_type_desc+" "+QString::number(coach_4.at(2)->sno));ui->label_StatusCam7->setText(coach_4.at(2)->camera_type_desc+" "+QString::number(coach_4.at(2)->sno)+" is OFFLINE");
                ui->label_StatusCam7->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 4 IPCAM 4 - SALOON 4
            if(coach_4.at(3)->activeStatus == ACTIVE){

                ui->label_cam8->setText(coach_4.at(3)->camera_type_desc+" "+QString::number(coach_4.at(3)->sno));ui->label_StatusCam8->setText("IP : " + coach_4.at(3)->ip_addr);
                ui->label_StatusCam8->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam8->setText(coach_4.at(3)->camera_type_desc+" "+QString::number(coach_4.at(3)->sno));ui->label_StatusCam8->setText(coach_4.at(3)->camera_type_desc+" "+QString::number(coach_4.at(3)->sno)+" is OFFLINE");
                ui->label_StatusCam8->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 4 IPCAM 5 - SALOON 5
            if(coach_4.at(4)->activeStatus == ACTIVE){

                ui->label_cam9->setText(coach_4.at(4)->camera_type_desc+" "+QString::number(coach_4.at(4)->sno));ui->label_StatusCam9->setText("IP : " + coach_4.at(4)->ip_addr);
                ui->label_StatusCam9->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam9->setText(coach_4.at(4)->camera_type_desc+" "+QString::number(coach_4.at(4)->sno));ui->label_StatusCam9->setText(coach_4.at(4)->camera_type_desc+" "+QString::number(coach_4.at(4)->sno)+" is OFFLINE");
                ui->label_StatusCam9->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 4 IPCAM 6 - SALOON 6
            if(coach_4.at(5)->activeStatus == ACTIVE){

                ui->label_cam10->setText(coach_4.at(5)->camera_type_desc+" "+QString::number(coach_4.at(5)->sno));ui->label_StatusCam10->setText("IP : " + coach_4.at(5)->ip_addr);
                ui->label_StatusCam10->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam10->setText(coach_4.at(5)->camera_type_desc+" "+QString::number(coach_4.at(5)->sno));ui->label_StatusCam10->setText(coach_4.at(5)->camera_type_desc+" "+QString::number(coach_4.at(5)->sno)+" is OFFLINE");
                ui->label_StatusCam10->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 4 IPCAM 7 - SALOON 7
            if(coach_4.at(6)->activeStatus == ACTIVE){

                ui->label_cam11->setText(coach_4.at(6)->camera_type_desc+" "+QString::number(coach_4.at(6)->sno));ui->label_StatusCam11->setText("IP : " + coach_4.at(6)->ip_addr);
                ui->label_StatusCam11->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam11->setText(coach_4.at(6)->camera_type_desc+" "+QString::number(coach_4.at(6)->sno));ui->label_StatusCam11->setText(coach_4.at(6)->camera_type_desc+" "+QString::number(coach_4.at(6)->sno)+" is OFFLINE");
                ui->label_StatusCam11->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 4 IPCAM 8 - SALOON 8
            if(coach_4.at(7)->activeStatus == ACTIVE){

                ui->label_cam12->setText(coach_4.at(7)->camera_type_desc+" "+QString::number(coach_4.at(7)->sno));ui->label_StatusCam12->setText("IP : " + coach_4.at(7)->ip_addr);
                ui->label_StatusCam12->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam12->setText(coach_4.at(7)->camera_type_desc+" "+QString::number(coach_4.at(7)->sno));ui->label_StatusCam12->setText(coach_4.at(7)->camera_type_desc+" "+QString::number(coach_4.at(7)->sno)+" is OFFLINE");
                ui->label_StatusCam12->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
        }
    }
    //    }

    //COACH 5
    else if(iterate_button_pressed == 4){

        coachWithSaloonCam();

        //changing labels of device status by iterating the rake list
        //for (int i = 0; i < rake.size(); i++) {
        //            coach_5 = rake[4];
        for (int j = 0; j < coach_5.size(); j++)
        {
            ui->label_coach_name->setText("CAR TYPE - "+coach_5.at(0)->car_type_desc + "  COACH NO - "+QString::number(coach_5.at(0)->csno));
            //COACH 5 IPCAM 1 - SALOON 1
            if(coach_5.at(0)->activeStatus == ACTIVE){

                ui->label_cam5->setText(coach_5.at(0)->camera_type_desc+" "+QString::number(coach_5.at(0)->sno));ui->label_StatusCam5->setText("IP : " + coach_5.at(0)->ip_addr);
                ui->label_StatusCam5->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam5->setText(coach_5.at(0)->camera_type_desc+" "+QString::number(coach_5.at(0)->sno));ui->label_StatusCam5->setText(coach_5.at(0)->camera_type_desc+" "+QString::number(coach_5.at(0)->sno)+" is OFFLINE");
                ui->label_StatusCam5->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //COACH 5 IPCAM 2 - SALOON 2
            if(coach_5.at(1)->activeStatus == ACTIVE){

                ui->label_cam6->setText(coach_5.at(1)->camera_type_desc+" "+QString::number(coach_5.at(1)->sno));ui->label_StatusCam6->setText("IP : " + coach_5.at(1)->ip_addr);
                ui->label_StatusCam6->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam6->setText(coach_5.at(1)->camera_type_desc+" "+QString::number(coach_5.at(1)->sno));ui->label_StatusCam6->setText(coach_5.at(1)->camera_type_desc+" "+QString::number(coach_5.at(1)->sno)+" is OFFLINE");
                ui->label_StatusCam6->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 5 IPCAM 3 - SALOON 3
            if(coach_5.at(2)->activeStatus == ACTIVE){

                ui->label_cam7->setText(coach_5.at(2)->camera_type_desc+" "+QString::number(coach_5.at(2)->sno));ui->label_StatusCam7->setText("IP : " + coach_5.at(2)->ip_addr);
                ui->label_StatusCam7->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam7->setText(coach_5.at(2)->camera_type_desc+" "+QString::number(coach_5.at(2)->sno));ui->label_StatusCam7->setText(coach_5.at(2)->camera_type_desc+" "+QString::number(coach_5.at(2)->sno)+" is OFFLINE");
                ui->label_StatusCam7->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 5 IPCAM 4 - SALOON 4
            if(coach_5.at(3)->activeStatus == ACTIVE){

                ui->label_cam8->setText(coach_5.at(3)->camera_type_desc+" "+QString::number(coach_5.at(3)->sno));ui->label_StatusCam8->setText("IP : " + coach_5.at(3)->ip_addr);
                ui->label_StatusCam8->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam8->setText(coach_5.at(3)->camera_type_desc+" "+QString::number(coach_5.at(3)->sno));ui->label_StatusCam8->setText(coach_5.at(3)->camera_type_desc+" "+QString::number(coach_5.at(3)->sno)+" is OFFLINE");
                ui->label_StatusCam8->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 5 IPCAM 5 - SALOON 5
            if(coach_5.at(4)->activeStatus == ACTIVE){

                ui->label_cam9->setText(coach_5.at(4)->camera_type_desc+" "+QString::number(coach_5.at(4)->sno));ui->label_StatusCam9->setText("IP : " + coach_5.at(4)->ip_addr);
                ui->label_StatusCam9->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam9->setText(coach_5.at(4)->camera_type_desc+" "+QString::number(coach_5.at(4)->sno));ui->label_StatusCam9->setText(coach_5.at(4)->camera_type_desc+" "+QString::number(coach_5.at(4)->sno)+" is OFFLINE");
                ui->label_StatusCam9->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 5 IPCAM 6 - SALOON 6
            if(coach_5.at(5)->activeStatus == ACTIVE){

                ui->label_cam10->setText(coach_5.at(5)->camera_type_desc+" "+QString::number(coach_5.at(5)->sno));ui->label_StatusCam10->setText("IP : " + coach_5.at(5)->ip_addr);
                ui->label_StatusCam10->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam10->setText(coach_5.at(5)->camera_type_desc+" "+QString::number(coach_5.at(5)->sno));ui->label_StatusCam10->setText(coach_5.at(5)->camera_type_desc+" "+QString::number(coach_5.at(5)->sno)+" is OFFLINE");
                ui->label_StatusCam10->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 5 IPCAM 7 - SALOON 7
            if(coach_5.at(6)->activeStatus == ACTIVE){

                ui->label_cam11->setText(coach_5.at(6)->camera_type_desc+" "+QString::number(coach_5.at(6)->sno));ui->label_StatusCam11->setText("IP : " + coach_5.at(6)->ip_addr);
                ui->label_StatusCam11->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam11->setText(coach_5.at(6)->camera_type_desc+" "+QString::number(coach_5.at(6)->sno));ui->label_StatusCam11->setText(coach_5.at(6)->camera_type_desc+" "+QString::number(coach_5.at(6)->sno)+" is OFFLINE");
                ui->label_StatusCam11->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 5 IPCAM 8 - SALOON 8
            if(coach_5.at(7)->activeStatus == ACTIVE){

                ui->label_cam12->setText(coach_5.at(7)->camera_type_desc+" "+QString::number(coach_5.at(7)->sno));ui->label_StatusCam12->setText("IP : " + coach_5.at(7)->ip_addr);
                ui->label_StatusCam12->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam12->setText(coach_5.at(7)->camera_type_desc+" "+QString::number(coach_5.at(7)->sno));ui->label_StatusCam12->setText(coach_5.at(7)->camera_type_desc+" "+QString::number(coach_5.at(7)->sno)+" is OFFLINE");
                ui->label_StatusCam12->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
        }
    }
    //    }

    //COACH 6
    else if(iterate_button_pressed == 5){

        coachWithSaloonCam();

        //changing labels of device status by iterating the rake list
        //for (int i = 0; i < rake.size(); i++) {
        //            coach_6 = rake[5];
        for (int j = 0; j < coach_6.size(); j++)
        {
            ui->label_coach_name->setText("CAR TYPE - "+coach_6.at(0)->car_type_desc + "  COACH NO - "+QString::number(coach_6.at(0)->csno));
            //COACH 6 IPCAM 1 - SALOON 1
            if(coach_6.at(0)->activeStatus == ACTIVE){

                ui->label_cam5->setText(coach_6.at(0)->camera_type_desc+" "+QString::number(coach_6.at(0)->sno));ui->label_StatusCam5->setText("IP : " + coach_6.at(0)->ip_addr);
                ui->label_StatusCam5->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam5->setText(coach_6.at(0)->camera_type_desc+" "+QString::number(coach_6.at(0)->sno));ui->label_StatusCam5->setText(coach_6.at(0)->camera_type_desc+" "+QString::number(coach_6.at(0)->sno)+" is OFFLINE");
                ui->label_StatusCam5->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);"); //log
            }
            //COACH 6 IPCAM 2 - SALOON 2
            if(coach_6.at(1)->activeStatus == ACTIVE){

                ui->label_cam6->setText(coach_6.at(1)->camera_type_desc+" "+QString::number(coach_6.at(1)->sno));ui->label_StatusCam6->setText("IP : " + coach_6.at(1)->ip_addr);
                ui->label_StatusCam6->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam6->setText(coach_6.at(1)->camera_type_desc+" "+QString::number(coach_6.at(1)->sno));ui->label_StatusCam6->setText(coach_6.at(1)->camera_type_desc+" "+QString::number(coach_6.at(1)->sno)+" is OFFLINE");
                ui->label_StatusCam6->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 6 IPCAM 3 - SALOON 3
            if(coach_6.at(2)->activeStatus == ACTIVE){

                ui->label_cam7->setText(coach_6.at(2)->camera_type_desc+" "+QString::number(coach_6.at(2)->sno));ui->label_StatusCam7->setText("IP : " + coach_6.at(2)->ip_addr);
                ui->label_StatusCam7->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam7->setText(coach_6.at(2)->camera_type_desc+" "+QString::number(coach_6.at(2)->sno));ui->label_StatusCam7->setText(coach_6.at(2)->camera_type_desc+" "+QString::number(coach_6.at(2)->sno)+" is OFFLINE");
                ui->label_StatusCam7->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");//log
            }

            //COACH 6 IPCAM 4 - SALOON 4
            if(coach_6.at(3)->activeStatus == ACTIVE){

                ui->label_cam8->setText(coach_6.at(3)->camera_type_desc+" "+QString::number(coach_6.at(3)->sno));ui->label_StatusCam8->setText("IP : " + coach_6.at(3)->ip_addr);
                ui->label_StatusCam8->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam8->setText(coach_6.at(3)->camera_type_desc+" "+QString::number(coach_6.at(3)->sno));ui->label_StatusCam8->setText(coach_6.at(3)->camera_type_desc+" "+QString::number(coach_6.at(3)->sno)+" is OFFLINE");
                ui->label_StatusCam8->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 6 IPCAM 5 - SALOON 5
            if(coach_6.at(4)->activeStatus == ACTIVE){

                ui->label_cam9->setText(coach_6.at(4)->camera_type_desc+" "+QString::number(coach_6.at(4)->sno));ui->label_StatusCam9->setText("IP : " + coach_6.at(4)->ip_addr);
                ui->label_StatusCam9->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam9->setText(coach_6.at(4)->camera_type_desc+" "+QString::number(coach_6.at(4)->sno));ui->label_StatusCam9->setText(coach_6.at(4)->camera_type_desc+" "+QString::number(coach_6.at(4)->sno)+" is OFFLINE");
                ui->label_StatusCam9->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 6 IPCAM 6 - SALOON 6
            if(coach_6.at(5)->activeStatus == ACTIVE){

                ui->label_cam10->setText(coach_6.at(5)->camera_type_desc+" "+QString::number(coach_6.at(5)->sno));ui->label_StatusCam10->setText("IP : " + coach_6.at(5)->ip_addr);
                ui->label_StatusCam10->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam10->setText(coach_6.at(5)->camera_type_desc+" "+QString::number(coach_6.at(5)->sno));ui->label_StatusCam10->setText(coach_6.at(5)->camera_type_desc+" "+QString::number(coach_6.at(5)->sno)+" is OFFLINE");
                ui->label_StatusCam10->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 6 IPCAM 7 - SALOON 7
            if(coach_6.at(6)->activeStatus == ACTIVE){

                ui->label_cam11->setText(coach_6.at(6)->camera_type_desc+" "+QString::number(coach_6.at(6)->sno));ui->label_StatusCam11->setText("IP : " + coach_6.at(6)->ip_addr);
                ui->label_StatusCam11->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam11->setText(coach_6.at(6)->camera_type_desc+" "+QString::number(coach_6.at(6)->sno));ui->label_StatusCam11->setText(coach_6.at(6)->camera_type_desc+" "+QString::number(coach_6.at(6)->sno)+" is OFFLINE");
                ui->label_StatusCam11->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 6 IPCAM 8 - SALOON 8
            if(coach_6.at(7)->activeStatus == ACTIVE){

                ui->label_cam12->setText(coach_6.at(7)->camera_type_desc+" "+QString::number(coach_6.at(7)->sno));ui->label_StatusCam12->setText("IP : " + coach_6.at(7)->ip_addr);
                ui->label_StatusCam12->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam12->setText(coach_6.at(7)->camera_type_desc+" "+QString::number(coach_6.at(7)->sno));ui->label_StatusCam12->setText(coach_6.at(7)->camera_type_desc+" "+QString::number(coach_6.at(7)->sno)+" is OFFLINE");
                ui->label_StatusCam12->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
        }
    }
    //    }

    //COACH 7
    else if(iterate_button_pressed == 6){

        coachWithSaloonCam();

        //changing labels of device status by iterating the rake list
        //for (int i = 0; i < rake.size(); i++) {
        //            coach_7 = rake[6];
        for (int j = 0; j < coach_7.size(); j++)
        {
            ui->label_coach_name->setText("CAR TYPE - "+coach_7.at(0)->car_type_desc + "  COACH NO - "+QString::number(coach_7.at(0)->csno));
            //COACH 7 IPCAM 1 - SALOON 1
            if(coach_7.at(0)->activeStatus == ACTIVE){
                ui->label_cam5->setText(coach_7.at(0)->camera_type_desc+" "+QString::number(coach_7.at(0)->sno));ui->label_StatusCam5->setText("IP : " + coach_7.at(0)->ip_addr);
                ui->label_StatusCam5->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam5->setText(coach_7.at(0)->camera_type_desc+" "+QString::number(coach_7.at(0)->sno));ui->label_StatusCam5->setText(coach_7.at(0)->camera_type_desc+" "+QString::number(coach_7.at(0)->sno)+" is OFFLINE");
                ui->label_StatusCam5->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //COACH 7 IPCAM 2 - SALOON 2
            if(coach_7.at(1)->activeStatus == ACTIVE){

                ui->label_cam6->setText(coach_7.at(1)->camera_type_desc+" "+QString::number(coach_7.at(1)->sno));ui->label_StatusCam6->setText("IP : " + coach_7.at(1)->ip_addr);
                ui->label_StatusCam6->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam6->setText(coach_7.at(1)->camera_type_desc+" "+QString::number(coach_7.at(1)->sno));ui->label_StatusCam6->setText(coach_7.at(1)->camera_type_desc+" "+QString::number(coach_7.at(1)->sno)+" is OFFLINE");
                ui->label_StatusCam6->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 7 IPCAM 3 - SALOON 3
            if(coach_7.at(2)->activeStatus == ACTIVE){

                ui->label_cam7->setText(coach_7.at(2)->camera_type_desc+" "+QString::number(coach_7.at(2)->sno));ui->label_StatusCam7->setText("IP : " + coach_7.at(2)->ip_addr);
                ui->label_StatusCam7->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam7->setText(coach_7.at(2)->camera_type_desc+" "+QString::number(coach_7.at(2)->sno));ui->label_StatusCam7->setText(coach_7.at(2)->camera_type_desc+" "+QString::number(coach_7.at(2)->sno)+" is OFFLINE");
                ui->label_StatusCam7->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 7 IPCAM 4 - SALOON 4
            if(coach_7.at(3)->activeStatus == ACTIVE){

                ui->label_cam8->setText(coach_7.at(3)->camera_type_desc+" "+QString::number(coach_7.at(3)->sno));ui->label_StatusCam8->setText("IP : " + coach_7.at(3)->ip_addr);
                ui->label_StatusCam8->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam8->setText(coach_7.at(3)->camera_type_desc+" "+QString::number(coach_7.at(3)->sno));ui->label_StatusCam8->setText(coach_7.at(3)->camera_type_desc+" "+QString::number(coach_7.at(3)->sno)+" is OFFLINE");
                ui->label_StatusCam8->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 7 IPCAM 5 - SALOON 5
            if(coach_7.at(4)->activeStatus == ACTIVE){

                ui->label_cam9->setText(coach_7.at(4)->camera_type_desc+" "+QString::number(coach_7.at(4)->sno));ui->label_StatusCam9->setText("IP : " + coach_7.at(4)->ip_addr);
                ui->label_StatusCam9->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam9->setText(coach_7.at(4)->camera_type_desc+" "+QString::number(coach_7.at(4)->sno));ui->label_StatusCam9->setText(coach_7.at(4)->camera_type_desc+" "+QString::number(coach_7.at(4)->sno)+" is OFFLINE");
                ui->label_StatusCam9->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 7 IPCAM 6 - SALOON 6
            if(coach_7.at(5)->activeStatus == ACTIVE){

                ui->label_cam10->setText(coach_7.at(5)->camera_type_desc+" "+QString::number(coach_7.at(5)->sno));ui->label_StatusCam10->setText("IP : " + coach_7.at(5)->ip_addr);
                ui->label_StatusCam10->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam10->setText(coach_7.at(5)->camera_type_desc+" "+QString::number(coach_7.at(5)->sno));ui->label_StatusCam10->setText(coach_7.at(5)->camera_type_desc+" "+QString::number(coach_7.at(5)->sno)+" is OFFLINE");
                ui->label_StatusCam10->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 7 IPCAM 7 - SALOON 7
            if(coach_7.at(6)->activeStatus == ACTIVE){

                ui->label_cam11->setText(coach_7.at(6)->camera_type_desc+" "+QString::number(coach_7.at(6)->sno));ui->label_StatusCam11->setText("IP : " + coach_7.at(6)->ip_addr);
                ui->label_StatusCam11->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam11->setText(coach_7.at(6)->camera_type_desc+" "+QString::number(coach_7.at(6)->sno));ui->label_StatusCam11->setText(coach_7.at(6)->camera_type_desc+" "+QString::number(coach_7.at(6)->sno)+" is OFFLINE");
                ui->label_StatusCam11->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 7 IPCAM 8 - SALOON 8
            if(coach_7.at(7)->activeStatus == ACTIVE){

                ui->label_cam12->setText(coach_7.at(7)->camera_type_desc+" "+QString::number(coach_7.at(7)->sno));ui->label_StatusCam12->setText("IP : " + coach_7.at(7)->ip_addr);
                ui->label_StatusCam12->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam12->setText(coach_7.at(7)->camera_type_desc+" "+QString::number(coach_7.at(7)->sno));ui->label_StatusCam12->setText(coach_7.at(7)->camera_type_desc+" "+QString::number(coach_7.at(7)->sno)+" is OFFLINE");
                ui->label_StatusCam12->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
        }
    }
    //    }

    //COACH 8
    else if(iterate_button_pressed == 7){

        coachWithSaloonCam();

        //changing labels of device status by iterating the rake list
        //for (int i = 0; i < rake.size(); i++) {
        //            coach_8 = rake[7];
        for (int j = 0; j < coach_8.size(); j++)
        {
            ui->label_coach_name->setText("CAR TYPE - "+coach_8.at(0)->car_type_desc + "  COACH NO - "+QString::number(coach_8.at(0)->csno));
            //COACH 8 IPCAM 1 - SALOON 1
            if(coach_8.at(0)->activeStatus == ACTIVE){

                ui->label_cam5->setText(coach_8.at(0)->camera_type_desc+" "+QString::number(coach_8.at(0)->sno));ui->label_StatusCam5->setText("IP : " + coach_8.at(0)->ip_addr);
                ui->label_StatusCam5->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam5->setText(coach_8.at(0)->camera_type_desc+" "+QString::number(coach_8.at(0)->sno));ui->label_StatusCam5->setText(coach_8.at(0)->camera_type_desc+" "+QString::number(coach_8.at(0)->sno)+" is OFFLINE");
                ui->label_StatusCam5->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //COACH 8 IPCAM 2 - SALOON 2
            if(coach_8.at(1)->activeStatus == ACTIVE){

                ui->label_cam6->setText(coach_8.at(1)->camera_type_desc+" "+QString::number(coach_8.at(1)->sno));ui->label_StatusCam6->setText("IP : " + coach_8.at(1)->ip_addr);
                ui->label_StatusCam6->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam6->setText(coach_8.at(1)->camera_type_desc+" "+QString::number(coach_8.at(1)->sno));ui->label_StatusCam6->setText(coach_8.at(1)->camera_type_desc+" "+QString::number(coach_8.at(1)->sno)+" is OFFLINE");
                ui->label_StatusCam6->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 8 IPCAM 3 - SALOON 3
            if(coach_8.at(2)->activeStatus == ACTIVE){

                ui->label_cam7->setText(coach_8.at(2)->camera_type_desc+" "+QString::number(coach_8.at(2)->sno));ui->label_StatusCam7->setText("IP : " + coach_8.at(2)->ip_addr);
                ui->label_StatusCam7->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam7->setText(coach_8.at(2)->camera_type_desc+" "+QString::number(coach_8.at(2)->sno));ui->label_StatusCam7->setText(coach_8.at(2)->camera_type_desc+" "+QString::number(coach_8.at(2)->sno)+" is OFFLINE");
                ui->label_StatusCam7->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 8 IPCAM 4 - SALOON 4
            if(coach_8.at(3)->activeStatus == ACTIVE){

                ui->label_cam8->setText(coach_8.at(3)->camera_type_desc+" "+QString::number(coach_8.at(3)->sno));ui->label_StatusCam8->setText("IP : " + coach_8.at(3)->ip_addr);
                ui->label_StatusCam8->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam8->setText(coach_8.at(3)->camera_type_desc+" "+QString::number(coach_8.at(3)->sno));ui->label_StatusCam8->setText(coach_8.at(3)->camera_type_desc+" "+QString::number(coach_8.at(3)->sno)+" is OFFLINE");
                ui->label_StatusCam8->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 8 IPCAM 5 - SALOON 5
            if(coach_8.at(4)->activeStatus == ACTIVE){

                ui->label_cam9->setText(coach_8.at(4)->camera_type_desc+" "+QString::number(coach_8.at(4)->sno));ui->label_StatusCam9->setText("IP : " + coach_8.at(4)->ip_addr);
                ui->label_StatusCam9->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam9->setText(coach_8.at(4)->camera_type_desc+" "+QString::number(coach_8.at(4)->sno));ui->label_StatusCam9->setText(coach_8.at(4)->camera_type_desc+" "+QString::number(coach_8.at(4)->sno)+" is OFFLINE");
                ui->label_StatusCam9->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 8 IPCAM 6 - SALOON 6
            if(coach_8.at(5)->activeStatus == ACTIVE){

                ui->label_cam10->setText(coach_8.at(5)->camera_type_desc+" "+QString::number(coach_8.at(5)->sno));ui->label_StatusCam10->setText("IP : " + coach_8.at(5)->ip_addr);
                ui->label_StatusCam10->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam10->setText(coach_8.at(5)->camera_type_desc+" "+QString::number(coach_8.at(5)->sno));ui->label_StatusCam10->setText(coach_8.at(5)->camera_type_desc+" "+QString::number(coach_8.at(5)->sno)+" is OFFLINE");
                ui->label_StatusCam10->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 8 IPCAM 7 - SALOON 7
            if(coach_8.at(6)->activeStatus == ACTIVE){

                ui->label_cam11->setText(coach_8.at(6)->camera_type_desc+" "+QString::number(coach_8.at(6)->sno));ui->label_StatusCam11->setText("IP : " + coach_8.at(6)->ip_addr);
                ui->label_StatusCam11->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam11->setText(coach_8.at(6)->camera_type_desc+" "+QString::number(coach_8.at(6)->sno));ui->label_StatusCam11->setText(coach_8.at(6)->camera_type_desc+" "+QString::number(coach_8.at(6)->sno)+" is OFFLINE");
                ui->label_StatusCam11->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 8 IPCAM 8 - SALOON 8
            if(coach_8.at(7)->activeStatus == ACTIVE){

                ui->label_cam12->setText(coach_8.at(7)->camera_type_desc+" "+QString::number(coach_8.at(7)->sno));ui->label_StatusCam12->setText("IP : " + coach_8.at(7)->ip_addr);
                ui->label_StatusCam12->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam12->setText(coach_8.at(7)->camera_type_desc+" "+QString::number(coach_8.at(7)->sno));ui->label_StatusCam12->setText(coach_8.at(7)->camera_type_desc+" "+QString::number(coach_8.at(7)->sno)+" is OFFLINE");
                ui->label_StatusCam12->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
        }
    }
    //    }
    //COACH 9
    else if(iterate_button_pressed == 8){

        coachWithSaloonCam();

        //changing labels of device status by iterating the rake list
        //for (int i = 0; i < rake.size(); i++) {
        //            coach_9 = rake[8];
        for (int j = 0; j < coach_9.size(); j++)
        {
            ui->label_coach_name->setText("CAR TYPE - "+coach_9.at(0)->car_type_desc + "  COACH NO - "+QString::number(coach_9.at(0)->csno));
            //COACH 9 IPCAM 1 - SALOON 1
            if(coach_9.at(0)->activeStatus == ACTIVE){

                ui->label_cam5->setText(coach_9.at(0)->camera_type_desc+" "+QString::number(coach_9.at(0)->sno));ui->label_StatusCam5->setText("IP : " + coach_9.at(0)->ip_addr);
                ui->label_StatusCam5->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam5->setText(coach_9.at(0)->camera_type_desc+" "+QString::number(coach_9.at(0)->sno));ui->label_StatusCam5->setText(coach_9.at(0)->camera_type_desc+" "+QString::number(coach_9.at(0)->sno)+" is OFFLINE");
                ui->label_StatusCam5->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //COACH 9 IPCAM 2 - SALOON 2
            if(coach_9.at(1)->activeStatus == ACTIVE){

                ui->label_cam6->setText(coach_9.at(1)->camera_type_desc+" "+QString::number(coach_9.at(1)->sno));ui->label_StatusCam6->setText("IP : " + coach_9.at(1)->ip_addr);
                ui->label_StatusCam6->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam6->setText(coach_9.at(1)->camera_type_desc+" "+QString::number(coach_9.at(1)->sno));ui->label_StatusCam6->setText(coach_9.at(1)->camera_type_desc+" "+QString::number(coach_9.at(1)->sno)+" is OFFLINE");
                ui->label_StatusCam6->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 9 IPCAM 3 - SALOON 3
            if(coach_9.at(2)->activeStatus == ACTIVE){

                ui->label_cam7->setText(coach_9.at(2)->camera_type_desc+" "+QString::number(coach_9.at(2)->sno));ui->label_StatusCam7->setText("IP : " + coach_9.at(2)->ip_addr);
                ui->label_StatusCam7->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam7->setText(coach_9.at(2)->camera_type_desc+" "+QString::number(coach_9.at(2)->sno));ui->label_StatusCam7->setText(coach_9.at(2)->camera_type_desc+" "+QString::number(coach_9.at(2)->sno)+" is OFFLINE");
                ui->label_StatusCam7->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 9 IPCAM 4 - SALOON 4
            if(coach_9.at(3)->activeStatus == ACTIVE){

                ui->label_cam8->setText(coach_9.at(3)->camera_type_desc+" "+QString::number(coach_9.at(3)->sno));ui->label_StatusCam8->setText("IP : " + coach_9.at(3)->ip_addr);
                ui->label_StatusCam8->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam8->setText(coach_9.at(3)->camera_type_desc+" "+QString::number(coach_9.at(3)->sno));ui->label_StatusCam8->setText(coach_9.at(3)->camera_type_desc+" "+QString::number(coach_9.at(3)->sno)+" is OFFLINE");
                ui->label_StatusCam8->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 9 IPCAM 5 - SALOON 5
            if(coach_9.at(4)->activeStatus == ACTIVE){

                ui->label_cam9->setText(coach_9.at(4)->camera_type_desc+" "+QString::number(coach_9.at(4)->sno));ui->label_StatusCam9->setText("IP : " + coach_9.at(4)->ip_addr);
                ui->label_StatusCam9->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam9->setText(coach_9.at(4)->camera_type_desc+" "+QString::number(coach_9.at(4)->sno));ui->label_StatusCam9->setText(coach_9.at(4)->camera_type_desc+" "+QString::number(coach_9.at(4)->sno)+" is OFFLINE");
                ui->label_StatusCam9->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 9 IPCAM 6 - SALOON 6
            if(coach_9.at(5)->activeStatus == ACTIVE){

                ui->label_cam10->setText(coach_9.at(5)->camera_type_desc+" "+QString::number(coach_9.at(5)->sno));ui->label_StatusCam10->setText("IP : " + coach_9.at(5)->ip_addr);
                ui->label_StatusCam10->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam10->setText(coach_9.at(5)->camera_type_desc+" "+QString::number(coach_9.at(5)->sno));ui->label_StatusCam10->setText(coach_9.at(5)->camera_type_desc+" "+QString::number(coach_9.at(5)->sno)+" is OFFLINE");
                ui->label_StatusCam10->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 9 IPCAM 7 - SALOON 7
            if(coach_9.at(6)->activeStatus == ACTIVE){

                ui->label_cam11->setText(coach_9.at(6)->camera_type_desc+" "+QString::number(coach_9.at(6)->sno));ui->label_StatusCam11->setText("IP : " + coach_9.at(6)->ip_addr);
                ui->label_StatusCam11->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam11->setText(coach_9.at(6)->camera_type_desc+" "+QString::number(coach_9.at(6)->sno));ui->label_StatusCam11->setText(coach_9.at(6)->camera_type_desc+" "+QString::number(coach_9.at(6)->sno)+" is OFFLINE");
                ui->label_StatusCam11->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 9 IPCAM 8 - SALOON 8
            if(coach_9.at(7)->activeStatus == ACTIVE){

                ui->label_cam12->setText(coach_9.at(7)->camera_type_desc+" "+QString::number(coach_9.at(7)->sno));ui->label_StatusCam12->setText("IP : " + coach_9.at(7)->ip_addr);
                ui->label_StatusCam12->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam12->setText(coach_9.at(7)->camera_type_desc+" "+QString::number(coach_9.at(7)->sno));ui->label_StatusCam12->setText(coach_9.at(7)->camera_type_desc+" "+QString::number(coach_9.at(7)->sno)+" is OFFLINE");
                ui->label_StatusCam12->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
        }
    }
    //    }

    //COACH 10
    else if(iterate_button_pressed == 9){

        coachWithSaloonCam();

        //changing labels of device status by iterating the rake list
        //for (int i = 0; i < rake.size(); i++) {
        //            coach_10 = rake[9];
        for (int j = 0; j < coach_10.size(); j++)
        {
            ui->label_coach_name->setText("CAR TYPE - "+coach_10.at(0)->car_type_desc + "  COACH NO - "+QString::number(coach_10.at(0)->csno));
            //COACH 10 IPCAM 1 - SALOON 1
            if(coach_10.at(0)->activeStatus == ACTIVE){

                ui->label_cam5->setText(coach_10.at(0)->camera_type_desc+" "+QString::number(coach_10.at(0)->sno));ui->label_StatusCam5->setText("IP : " + coach_10.at(0)->ip_addr);
                ui->label_StatusCam5->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam5->setText(coach_10.at(0)->camera_type_desc+" "+QString::number(coach_10.at(0)->sno));ui->label_StatusCam5->setText(coach_10.at(0)->camera_type_desc+" "+QString::number(coach_10.at(0)->sno)+" is OFFLINE");
                ui->label_StatusCam5->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //COACH 10 IPCAM 2 - SALOON 2
            if(coach_10.at(1)->activeStatus == ACTIVE){

                ui->label_cam6->setText(coach_10.at(1)->camera_type_desc+" "+QString::number(coach_10.at(1)->sno));ui->label_StatusCam6->setText("IP : " + coach_10.at(1)->ip_addr);
                ui->label_StatusCam6->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam6->setText(coach_10.at(1)->camera_type_desc+" "+QString::number(coach_10.at(1)->sno));ui->label_StatusCam6->setText(coach_10.at(1)->camera_type_desc+" "+QString::number(coach_10.at(1)->sno)+" is OFFLINE");
                ui->label_StatusCam6->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 10 IPCAM 3 - SALOON 3
            if(coach_10.at(2)->activeStatus == ACTIVE){

                ui->label_cam7->setText(coach_10.at(2)->camera_type_desc+" "+QString::number(coach_10.at(2)->sno));ui->label_StatusCam7->setText("IP : " + coach_10.at(2)->ip_addr);
                ui->label_StatusCam7->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam7->setText(coach_10.at(2)->camera_type_desc+" "+QString::number(coach_10.at(2)->sno));ui->label_StatusCam7->setText(coach_10.at(2)->camera_type_desc+" "+QString::number(coach_10.at(2)->sno)+" is OFFLINE");
                ui->label_StatusCam7->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 10 IPCAM 4 - SALOON 4
            if(coach_10.at(3)->activeStatus == ACTIVE){

                ui->label_cam8->setText(coach_10.at(3)->camera_type_desc+" "+QString::number(coach_10.at(3)->sno));ui->label_StatusCam8->setText("IP : " + coach_10.at(3)->ip_addr);
                ui->label_StatusCam8->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam8->setText(coach_10.at(3)->camera_type_desc+" "+QString::number(coach_10.at(3)->sno));ui->label_StatusCam8->setText(coach_10.at(3)->camera_type_desc+" "+QString::number(coach_10.at(3)->sno)+" is OFFLINE");
                ui->label_StatusCam8->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 10 IPCAM 5 - SALOON 5
            if(coach_10.at(4)->activeStatus == ACTIVE){

                ui->label_cam9->setText(coach_10.at(4)->camera_type_desc+" "+QString::number(coach_10.at(4)->sno));ui->label_StatusCam9->setText("IP : " + coach_10.at(4)->ip_addr);
                ui->label_StatusCam9->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam9->setText(coach_10.at(4)->camera_type_desc+" "+QString::number(coach_10.at(4)->sno));ui->label_StatusCam9->setText(coach_10.at(4)->camera_type_desc+" "+QString::number(coach_10.at(4)->sno)+" is OFFLINE");
                ui->label_StatusCam9->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 10 IPCAM 6 - SALOON 6
            if(coach_10.at(5)->activeStatus == ACTIVE){

                ui->label_cam10->setText(coach_10.at(5)->camera_type_desc+" "+QString::number(coach_10.at(5)->sno));ui->label_StatusCam10->setText("IP : " + coach_10.at(5)->ip_addr);
                ui->label_StatusCam10->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam10->setText(coach_10.at(5)->camera_type_desc+" "+QString::number(coach_10.at(5)->sno));ui->label_StatusCam10->setText(coach_10.at(5)->camera_type_desc+" "+QString::number(coach_10.at(5)->sno)+" is OFFLINE");
                ui->label_StatusCam10->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 10 IPCAM 7 - SALOON 7
            if(coach_10.at(6)->activeStatus == ACTIVE){

                ui->label_cam11->setText(coach_10.at(6)->camera_type_desc+" "+QString::number(coach_10.at(6)->sno));ui->label_StatusCam11->setText("IP : " + coach_10.at(6)->ip_addr);
                ui->label_StatusCam11->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam11->setText(coach_10.at(6)->camera_type_desc+" "+QString::number(coach_10.at(6)->sno));ui->label_StatusCam11->setText(coach_10.at(6)->camera_type_desc+" "+QString::number(coach_10.at(6)->sno)+" is OFFLINE");
                ui->label_StatusCam11->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 10 IPCAM 8 - SALOON 8
            if(coach_10.at(7)->activeStatus == ACTIVE){

                ui->label_cam12->setText(coach_10.at(7)->camera_type_desc+" "+QString::number(coach_10.at(7)->sno));ui->label_StatusCam12->setText("IP : " + coach_10.at(7)->ip_addr);
                ui->label_StatusCam12->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam12->setText(coach_10.at(7)->camera_type_desc+" "+QString::number(coach_10.at(7)->sno));ui->label_StatusCam12->setText(coach_10.at(7)->camera_type_desc+" "+QString::number(coach_10.at(7)->sno)+" is OFFLINE");
                ui->label_StatusCam12->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
        }
    }
    //    }

    //COACH 11
    else if(iterate_button_pressed == 10){

        coachWithSaloonCam();

        //changing labels of device status by iterating the rake list
        //for (int i = 0; i < rake.size(); i++) {
        //            coach_11 = rake[10];
        for (int j = 0; j < coach_11.size(); j++)
        {
            ui->label_coach_name->setText("CAR TYPE - "+coach_11.at(0)->car_type_desc + "  COACH NO - "+QString::number(coach_11.at(0)->csno));
            //COACH 11 IPCAM 1 - SALOON 1
            if(coach_11.at(0)->activeStatus == ACTIVE){

                ui->label_cam5->setText(coach_11.at(0)->camera_type_desc+" "+QString::number(coach_11.at(0)->sno));ui->label_StatusCam5->setText("IP : " + coach_11.at(0)->ip_addr);
                ui->label_StatusCam5->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam5->setText(coach_11.at(0)->camera_type_desc+" "+QString::number(coach_11.at(0)->sno));ui->label_StatusCam5->setText(coach_11.at(0)->camera_type_desc+" "+QString::number(coach_11.at(0)->sno)+" is OFFLINE");
                ui->label_StatusCam5->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //COACH 11 IPCAM 2 - SALOON 2
            if(coach_11.at(1)->activeStatus == ACTIVE){

                ui->label_cam6->setText(coach_11.at(1)->camera_type_desc+" "+QString::number(coach_11.at(1)->sno));ui->label_StatusCam6->setText("IP : " + coach_11.at(1)->ip_addr);
                ui->label_StatusCam6->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam6->setText(coach_11.at(1)->camera_type_desc+" "+QString::number(coach_11.at(1)->sno));ui->label_StatusCam6->setText(coach_11.at(1)->camera_type_desc+" "+QString::number(coach_11.at(1)->sno)+" is OFFLINE");
                ui->label_StatusCam6->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 11 IPCAM 3 - SALOON 3
            if(coach_11.at(2)->activeStatus == ACTIVE){

                ui->label_cam7->setText(coach_11.at(2)->camera_type_desc+" "+QString::number(coach_11.at(2)->sno));ui->label_StatusCam7->setText("IP : " + coach_11.at(2)->ip_addr);
                ui->label_StatusCam7->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam7->setText(coach_11.at(2)->camera_type_desc+" "+QString::number(coach_11.at(2)->sno));ui->label_StatusCam7->setText(coach_11.at(2)->camera_type_desc+" "+QString::number(coach_11.at(2)->sno)+" is OFFLINE");
                ui->label_StatusCam7->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 11 IPCAM 4 - SALOON 4
            if(coach_11.at(3)->activeStatus == ACTIVE){

                ui->label_cam8->setText(coach_11.at(3)->camera_type_desc+" "+QString::number(coach_11.at(3)->sno));ui->label_StatusCam8->setText("IP : " + coach_11.at(3)->ip_addr);
                ui->label_StatusCam8->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam8->setText(coach_11.at(3)->camera_type_desc+" "+QString::number(coach_11.at(3)->sno));ui->label_StatusCam8->setText(coach_11.at(3)->camera_type_desc+" "+QString::number(coach_11.at(3)->sno)+" is OFFLINE");
                ui->label_StatusCam8->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 11 IPCAM 5 - SALOON 5
            if(coach_11.at(4)->activeStatus == ACTIVE){

                ui->label_cam9->setText(coach_11.at(4)->camera_type_desc+" "+QString::number(coach_11.at(4)->sno));ui->label_StatusCam9->setText("IP : " + coach_11.at(4)->ip_addr);
                ui->label_StatusCam9->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam9->setText(coach_11.at(4)->camera_type_desc+" "+QString::number(coach_11.at(4)->sno));ui->label_StatusCam9->setText(coach_11.at(4)->camera_type_desc+" "+QString::number(coach_11.at(4)->sno)+" is OFFLINE");
                ui->label_StatusCam9->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 11 IPCAM 6 - SALOON 6
            if(coach_11.at(5)->activeStatus == ACTIVE){

                ui->label_cam10->setText(coach_11.at(5)->camera_type_desc+" "+QString::number(coach_11.at(5)->sno));ui->label_StatusCam10->setText("IP : " + coach_11.at(5)->ip_addr);
                ui->label_StatusCam10->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam10->setText(coach_11.at(5)->camera_type_desc+" "+QString::number(coach_11.at(5)->sno));ui->label_StatusCam10->setText(coach_11.at(5)->camera_type_desc+" "+QString::number(coach_11.at(5)->sno)+" is OFFLINE");
                ui->label_StatusCam10->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 11 IPCAM 7 - SALOON 7
            if(coach_11.at(6)->activeStatus == ACTIVE){

                ui->label_cam11->setText(coach_11.at(6)->camera_type_desc+" "+QString::number(coach_11.at(6)->sno));ui->label_StatusCam11->setText("IP : " + coach_11.at(6)->ip_addr);
                ui->label_StatusCam11->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam11->setText(coach_11.at(6)->camera_type_desc+" "+QString::number(coach_11.at(6)->sno));ui->label_StatusCam11->setText(coach_11.at(6)->camera_type_desc+" "+QString::number(coach_11.at(6)->sno)+" is OFFLINE");
                ui->label_StatusCam11->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }

            //COACH 11 IPCAM 8 - SALOON 8
            if(coach_11.at(7)->activeStatus == ACTIVE){

                ui->label_cam12->setText(coach_11.at(7)->camera_type_desc+" "+QString::number(coach_11.at(7)->sno));ui->label_StatusCam12->setText("IP : " + coach_11.at(7)->ip_addr);
                ui->label_StatusCam12->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam12->setText(coach_11.at(7)->camera_type_desc+" "+QString::number(coach_11.at(7)->sno));ui->label_StatusCam12->setText(coach_11.at(7)->camera_type_desc+" "+QString::number(coach_11.at(7)->sno)+" is OFFLINE");
                ui->label_StatusCam12->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
        }
    }
    //    }

    //COACH 12
    else if(iterate_button_pressed == 11){

        //changing labels of device status by iterating the rake list

        rearCoach();

        //for (int i = 0; i < rake.size(); i++) {
        //            coach_12 = rake[11];
        for (int j = 0; j < coach_12.size(); j++)
        {
            ui->label_coach_name->setText("CAR TYPE - "+coach_12.at(0)->car_type_desc + "  COACH NO - "+QString::number(coach_12.at(0)->csno));

            //Coach 12 IPCAM 1 - REAR
            if(coach_12.at(0)->activeStatus == ACTIVE){

                ui->label_cam13->setText(coach_12.at(0)->camera_type_desc);ui->label_StatusCam13->setText("IP : " + coach_12.at(0)->ip_addr);
                ui->label_StatusCam13->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam13->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
            }
            else
            {
                ui->label_cam13->setText(coach_12.at(0)->camera_type_desc);ui->label_StatusCam13->setText(coach_12.at(0)->camera_type_desc + " is OFFLINE");
                ui->label_StatusCam13->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam13->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
            }
            //Coach 12 IPCAM 2 - REAR RIGHT
            if(coach_12.at(1)->activeStatus == ACTIVE){

                ui->label_cam14->setText(coach_12.at(1)->camera_type_desc);ui->label_StatusCam14->setText("IP : " + coach_12.at(1)->ip_addr);
                ui->label_StatusCam14->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam14->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
            }
            else
            {
                ui->label_cam14->setText(coach_12.at(1)->camera_type_desc);ui->label_StatusCam14->setText(coach_12.at(1)->camera_type_desc + " is OFFLINE");
                ui->label_StatusCam14->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam14->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
            }
            //Coach 12 IPCAM 3 - REAR LEFT
            if(coach_12.at(2)->activeStatus == ACTIVE){

                ui->label_cam15->setText(coach_12.at(2)->camera_type_desc);ui->label_StatusCam15->setText("IP : " + coach_12.at(2)->ip_addr);
                ui->label_StatusCam15->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam15->setStyleSheet("image: url(:/new/icons/cctv_green.png);");

            }
            else
            {
                ui->label_cam15->setText(coach_12.at(2)->camera_type_desc);ui->label_StatusCam15->setText(coach_12.at(2)->camera_type_desc + " is OFFLINE");
                ui->label_StatusCam15->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam15->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
            }
            //Coach 12 IPCAM 4 - REAR CAB
            if(coach_12.at(3)->activeStatus == ACTIVE){

                ui->label_cam16->setText(coach_12.at(3)->camera_type_desc);ui->label_StatusCam16->setText("IP : " + coach_12.at(3)->ip_addr);
                ui->label_StatusCam16->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam16->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
            }
            else
            {
                ui->label_cam16->setText(coach_12.at(3)->camera_type_desc);ui->label_StatusCam16->setText(coach_12.at(3)->camera_type_desc + " is OFFLINE");
                ui->label_StatusCam16->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam16->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
            }
            //Coach 12 IPCAM 5 - SALOON 1
            if(coach_12.at(4)->activeStatus == ACTIVE){

                ui->label_cam5->setText(coach_12.at(4)->camera_type_desc+" "+QString::number(coach_12.at(4)->sno));ui->label_StatusCam5->setText("IP : " + coach_12.at(4)->ip_addr);
                ui->label_StatusCam5->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam5->setText(coach_12.at(4)->camera_type_desc+" "+QString::number(coach_12.at(4)->sno));ui->label_StatusCam5->setText(coach_12.at(4)->camera_type_desc +" "+QString::number(coach_12.at(4)->sno)+ " is OFFLINE");
                ui->label_StatusCam5->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam5->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //Coach 12 IPCAM 6 - SALOON 2
            if(coach_12.at(5)->activeStatus == ACTIVE){

                ui->label_cam6->setText(coach_12.at(5)->camera_type_desc+" "+QString::number(coach_12.at(5)->sno));ui->label_StatusCam6->setText("IP : " + coach_12.at(5)->ip_addr);
                ui->label_StatusCam6->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam6->setText(coach_12.at(5)->camera_type_desc+" "+QString::number(coach_12.at(5)->sno));ui->label_StatusCam6->setText(coach_12.at(5)->camera_type_desc +" "+QString::number(coach_12.at(5)->sno)+ " is OFFLINE");
                ui->label_StatusCam6->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam6->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //Coach 12 IPCAM 7 - SALOON 3
            if(coach_12.at(6)->activeStatus == ACTIVE){

                ui->label_cam7->setText(coach_12.at(6)->camera_type_desc+" "+QString::number(coach_12.at(6)->sno));ui->label_StatusCam7->setText("IP : " + coach_12.at(6)->ip_addr);
                ui->label_StatusCam7->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam7->setText(coach_12.at(6)->camera_type_desc+" "+QString::number(coach_12.at(6)->sno));ui->label_StatusCam7->setText(coach_12.at(6)->camera_type_desc + " "+QString::number(coach_12.at(6)->sno)+" is OFFLINE");
                ui->label_StatusCam7->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam7->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //Coach 12 IPCAM 8 - SALOON 4
            if(coach_12.at(7)->activeStatus == ACTIVE){

                ui->label_cam8->setText(coach_12.at(7)->camera_type_desc+" "+QString::number(coach_12.at(7)->sno));ui->label_StatusCam8->setText("IP : " + coach_12.at(7)->ip_addr);
                ui->label_StatusCam8->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam8->setText(coach_12.at(7)->camera_type_desc+" "+QString::number(coach_12.at(7)->sno));ui->label_StatusCam8->setText(coach_12.at(7)->camera_type_desc +" "+QString::number(coach_12.at(7)->sno)+ " is OFFLINE");
                ui->label_StatusCam8->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam8->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //Coach 1 IPCAM 9 - SALOON 5
            if(coach_12.at(8)->activeStatus == ACTIVE){

                ui->label_cam9->setText(coach_12.at(8)->camera_type_desc+" "+QString::number(coach_12.at(8)->sno));ui->label_StatusCam9->setText("IP : " + coach_12.at(8)->ip_addr);
                ui->label_StatusCam9->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam9->setText(coach_12.at(8)->camera_type_desc+" "+QString::number(coach_12.at(8)->sno));ui->label_StatusCam9->setText(coach_12.at(8)->camera_type_desc+" "+QString::number(coach_12.at(8)->sno) + " is OFFLINE");
                ui->label_StatusCam9->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam9->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //Coach 1 IPCAM 12 - SALOON 6
            if(coach_12.at(9)->activeStatus == ACTIVE){

                ui->label_cam10->setText(coach_12.at(9)->camera_type_desc+" "+QString::number(coach_12.at(9)->sno));ui->label_StatusCam10->setText("IP : " + coach_12.at(9)->ip_addr);
                ui->label_StatusCam10->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam10->setText(coach_12.at(9)->camera_type_desc+" "+QString::number(coach_12.at(9)->sno));ui->label_StatusCam10->setText(coach_12.at(9)->camera_type_desc +" "+QString::number(coach_12.at(9)->sno) + " is OFFLINE");
                ui->label_StatusCam10->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam10->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //Coach 1 IPCAM 12 - SALOON 7
            if(coach_12.at(10)->activeStatus == ACTIVE){

                ui->label_cam11->setText(coach_12.at(10)->camera_type_desc+" "+QString::number(coach_12.at(10)->sno));ui->label_StatusCam11->setText("IP : " + coach_12.at(10)->ip_addr);
                ui->label_StatusCam11->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam11->setText(coach_12.at(10)->camera_type_desc+" "+QString::number(coach_12.at(10)->sno));ui->label_StatusCam11->setText(coach_12.at(10)->camera_type_desc +" "+QString::number(coach_12.at(10)->sno)+ " is OFFLINE");
                ui->label_StatusCam11->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam11->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
            //Coach 1 IPCAM 12 - SALOON 8
            if(coach_12.at(11)->activeStatus == ACTIVE){

                ui->label_cam12->setText(coach_12.at(11)->camera_type_desc+" "+QString::number(coach_12.at(11)->sno));ui->label_StatusCam12->setText("IP : " + coach_12.at(11)->ip_addr);
                ui->label_StatusCam12->setStyleSheet("QLabel { color : green; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_green.png);");
            }
            else
            {
                ui->label_cam12->setText(coach_12.at(11)->camera_type_desc+" "+QString::number(coach_12.at(11)->sno));ui->label_StatusCam12->setText(coach_12.at(11)->camera_type_desc +" "+QString::number(coach_12.at(11)->sno) + " is OFFLINE");
                ui->label_StatusCam12->setStyleSheet("QLabel { color : red; }");ui->pushButton_cam12->setStyleSheet("image: url(:/new/icons/cab_cam_red.png);");
            }
        }
    }
}
//}

void DeviceWindow::frontCoach()
{
    ui->pushButton_cam13->setStyleSheet("image: url(:/new/icons/cctv.png);");
    ui->pushButton_cam14->setStyleSheet("image: url(:/new/icons/cctv.png);");
    ui->pushButton_cam15->setStyleSheet("image: url(:/new/icons/cctv.png);");
    ui->pushButton_cam16->setStyleSheet("image: url(:/new/icons/cctv.png);");
    ui->label_cam13->setText("");
    ui->label_StatusCam13->setText("NO CAMERA INSTALLED");
    ui->label_StatusCam13->setStyleSheet("QLabel { color: rgb(150, 150, 150); }");
    ui->label_cam14->setText("");
    ui->label_StatusCam14->setText("NO CAMERA INSTALLED");
    ui->label_StatusCam14->setStyleSheet("QLabel { color: rgb(150, 150, 150); }");
    ui->label_cam15->setText("");
    ui->label_StatusCam15->setText("NO CAMERA INSTALLED");
    ui->label_StatusCam15->setStyleSheet("QLabel { color: rgb(150, 150, 150); }");
    ui->label_cam16->setText("");
    ui->label_StatusCam16->setText("NO CAMERA INSTALLED");
    ui->label_StatusCam16->setStyleSheet("QLabel { color: rgb(150, 150, 150); }");
}

void DeviceWindow::rearCoach()
{
    ui->pushButton_cam1->setStyleSheet("image: url(:/new/icons/cctv.png);");
    ui->pushButton_cam2->setStyleSheet("image: url(:/new/icons/cctv.png);");
    ui->pushButton_cam3->setStyleSheet("image: url(:/new/icons/cctv.png);");
    ui->pushButton_cam4->setStyleSheet("image: url(:/new/icons/cctv.png);");
    ui->label_cam1->setText("");
    ui->label_StatusCam1->setText("NO CAMERA INSTALLED");
    ui->label_StatusCam1->setStyleSheet("QLabel { color: rgb(150, 150, 150); }");
    ui->label_cam2->setText("");
    ui->label_StatusCam2->setText("NO CAMERA INSTALLED");
    ui->label_StatusCam2->setStyleSheet("QLabel { color: rgb(150, 150, 150); }");
    ui->label_cam3->setText("");
    ui->label_StatusCam3->setText("NO CAMERA INSTALLED");
    ui->label_StatusCam3->setStyleSheet("QLabel { color: rgb(150, 150, 150); }");
    ui->label_cam4->setText("");
    ui->label_StatusCam4->setText("NO CAMERA INSTALLED");
    ui->label_StatusCam4->setStyleSheet("QLabel { color: rgb(150, 150, 150); }");
}

void DeviceWindow::coachWithSaloonCam()
{
    ui->pushButton_cam1->setStyleSheet("image: url(:/new/icons/cctv.png);");
    ui->pushButton_cam2->setStyleSheet("image: url(:/new/icons/cctv.png);");
    ui->pushButton_cam3->setStyleSheet("image: url(:/new/icons/cctv.png);");
    ui->pushButton_cam4->setStyleSheet("image: url(:/new/icons/cctv.png);");
    ui->pushButton_cam13->setStyleSheet("image: url(:/new/icons/cctv.png);");
    ui->pushButton_cam14->setStyleSheet("image: url(:/new/icons/cctv.png);");
    ui->pushButton_cam15->setStyleSheet("image: url(:/new/icons/cctv.png);");
    ui->pushButton_cam16->setStyleSheet("image: url(:/new/icons/cctv.png);");
    ui->label_cam1->setText("");
    ui->label_StatusCam1->setText("NO CAMERA INSTALLED");
    ui->label_StatusCam1->setStyleSheet("QLabel { color: rgb(150, 150, 150); }");
    ui->label_cam2->setText("");
    ui->label_StatusCam2->setText("NO CAMERA INSTALLED");
    ui->label_StatusCam2->setStyleSheet("QLabel { color: rgb(150, 150, 150); }");
    ui->label_cam3->setText("");
    ui->label_StatusCam3->setText("NO CAMERA INSTALLED");
    ui->label_StatusCam3->setStyleSheet("QLabel { color: rgb(150, 150, 150); }");
    ui->label_cam4->setText("");
    ui->label_StatusCam4->setText("NO CAMERA INSTALLED");
    ui->label_StatusCam4->setStyleSheet("QLabel { color: rgb(150, 150, 150); }");
    ui->label_cam13->setText("");
    ui->label_StatusCam13->setText("NO CAMERA INSTALLED");
    ui->label_StatusCam13->setStyleSheet("QLabel { color: rgb(150, 150, 150); }");
    ui->label_cam14->setText("");
    ui->label_StatusCam14->setText("NO CAMERA INSTALLED");
    ui->label_StatusCam14->setStyleSheet("QLabel { color: rgb(150, 150, 150); }");
    ui->label_cam15->setText("");
    ui->label_StatusCam15->setText("NO CAMERA INSTALLED");
    ui->label_StatusCam15->setStyleSheet("QLabel { color: rgb(150, 150, 150); }");
    ui->label_cam16->setText("");
    ui->label_StatusCam16->setText("NO CAMERA INSTALLED");
    ui->label_StatusCam16->setStyleSheet("QLabel { color: rgb(150, 150, 150); }");
}

void DeviceWindow::on_pushButton_iterate_cams_left_clicked()
{

    libvlc_media_player_stop (_mp_deviceplayer);

    iterate_button_pressed--;

    if(iterate_button_pressed == -1)
    {
        iterate_button_pressed = 11;
        iterateCams();
    }

    else
    {
        iterateCams();
    }
}


void DeviceWindow::on_pushButton_iterate_cams_right_clicked()
{

    libvlc_media_player_stop (_mp_deviceplayer);

    iterate_button_pressed++;

    if(iterate_button_pressed == 12)
    {
        iterate_button_pressed = 0;
        iterateCams();
    }

    else
    {
        iterateCams();
    }
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

void DeviceWindow::nvrStatus(){

    if(ping_for_nvr1 == 0){
        ui->label_StatusNvr1->setText("IP : 192.168.1.2");
        ui->label_StatusNvr1->setStyleSheet("QLabel { color : green; }");ui->label_StatusIconNvr1->setStyleSheet("image: url(:/new/icons/nvr_green.png);");
    }
    else{
        ui->label_StatusNvr1->setText("NVR OFFLINE");
        ui->label_StatusNvr1->setStyleSheet("QLabel { color : red; }");ui->label_StatusIconNvr1->setStyleSheet("image: url(:/new/icons/nvr_red.png);");
    }
    if(ping_for_nvr2 == 0){
        ui->label_StatusNvr2->setText("IP : 192.168.1.234");
        ui->label_StatusNvr2->setStyleSheet("QLabel { color : green; }");ui->label_StatusIconNvr2->setStyleSheet("image: url(:/new/icons/nvr_green.png);");
    }
    else{
        ui->label_StatusNvr2->setText("NVR OFFLINE");
        ui->label_StatusNvr2->setStyleSheet("QLabel { color : red; }");ui->label_StatusIconNvr2->setStyleSheet("image: url(:/new/icons/nvr_red.png);");
    }

}

void DeviceWindow::on_pushButton_camStatus_clicked()
{
    //Switch to the second page (Camera Status)
    ui->stackedWidget->setCurrentIndex(2);

    ui->label_heading->setText(" CAMERA STATUS");

    returncounter_deviceStatus = 1; //signifies we are going to page 3 of stackwidget

    //    camStatus();

    iterateCams();

}


//void DeviceWindow::on_pushButton_sendDeviceStatus_clicked()
//{
//    // create JSON object
//    QJsonObject jsonObj;
//    jsonObj["StatusNVR1"] = ping_for_nvr1;
//    jsonObj["StatusNVR2"] = ping_for_nvr2;
//    jsonObj["StatusIPCAM1"] = ping_for_cam1;
//    jsonObj["StatusIPCAM2"] = ping_for_cam2;
//    jsonObj["StatusIPCAM3"] = ping_for_cam3;
//    jsonObj["StatusIPCAM4"] = ping_for_cam4;
//    jsonObj["StatusIPCAM5"] = ping_for_cam5;
//    jsonObj["StatusIPCAM6"] = ping_for_cam6;
//    jsonObj["StatusIPCAM7"] = ping_for_cam7;
//    jsonObj["StatusIPCAM8"] = ping_for_cam8;
//    jsonObj["StatusIPCAM9"] = ping_for_cam9;
//    jsonObj["StatusIPCAM10"] = ping_for_cam10;
//    jsonObj["StatusIPCAM11"] = ping_for_cam11;
//    jsonObj["StatusIPCAM12"] = ping_for_cam12;
//    jsonObj["StatusIPCAM13"] = ping_for_cam13;
//    jsonObj["StatusIPCAM14"] = ping_for_cam14;


//    // convert JSON object to JSON document
//    QJsonDocument jsonDoc(jsonObj);

//    // convert JSON document to a QByteArray
//    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Indented);

//    QFile file(deviceStatusJsonPath);
//    file.open(QIODevice::WriteOnly);
//    file.write(jsonData);
//    file.close();

//    // Create a QTcpSocket instance
//    QTcpSocket sendDeviceStatus;
//    sendDeviceStatus.connectToHost("192.168.1.50", 1234); //IP address of DDC
//    if (sendDeviceStatus.waitForConnected()) {

//        // Send the JSON data over the socket
//        sendDeviceStatus.write(jsonData);
//        sendDeviceStatus.flush(); // Important: Make sure to flush the socket to ensure all data is sent

//        // Wait for the server's response
//        if (sendDeviceStatus.waitForReadyRead()) {
//            QByteArray response = sendDeviceStatus.readAll();
//            qDebug() << "Server response:" << response;
//            ui->label_sendDeviceStatus->setText(response);
//            ui->label_sendDeviceStatus->setStyleSheet("QLabel {color : green;}");
//            QTimer::singleShot(5000, this, [this] () { ui->label_sendDeviceStatus->setText(""); });
//        }
//    } else {
//        qDebug() << "Connection error:" << sendDeviceStatus.errorString();
//        ui->label_sendDeviceStatus->setText(sendDeviceStatus.errorString());
//        ui->label_sendDeviceStatus->setStyleSheet("QLabel {color : red;}");
//        QTimer::singleShot(5000, this, [this] () { ui->label_sendDeviceStatus->setText(""); });
//    }

//    // Close the socket when done
//    sendDeviceStatus.close();
//}


void DeviceWindow::on_pushButton_cam1_clicked()
{
    libvlc_media_player_stop (_mp_deviceplayer);
    //FRONT CAM (only valid for COACH 1)
    if(iterate_button_pressed == 0 && coach_1.at(0)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_1.at(0)->camera_type_desc);
        QString urlstr = coach_1.at(0)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_1.at(0)->car_type_desc+"_"+coach_1.at(0)->camera_type_desc;
    }
}


void DeviceWindow::on_pushButton_cam2_clicked()
{
    libvlc_media_player_stop (_mp_deviceplayer);
    //FRONT RIGHT (only valid for COACH 1)
    if(iterate_button_pressed == 0 && coach_1.at(1)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_1.at(1)->camera_type_desc);
        QString urlstr = coach_1.at(1)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_1.at(1)->car_type_desc+"_"+coach_1.at(1)->camera_type_desc;
    }
}


void DeviceWindow::on_pushButton_cam3_clicked()
{
    libvlc_media_player_stop (_mp_deviceplayer);
    //FRONT LEFT (only valid for COACH 1)
    if(iterate_button_pressed == 0 && coach_1.at(2)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_1.at(2)->camera_type_desc);
        QString urlstr = coach_1.at(2)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_1.at(2)->car_type_desc+"_"+coach_1.at(2)->camera_type_desc;
    }
}


void DeviceWindow::on_pushButton_cam4_clicked()
{
    libvlc_media_player_stop (_mp_deviceplayer);
    //FRONT CAB (only valid for COACH 1)
    if(iterate_button_pressed == 0 && coach_1.at(3)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_1.at(3)->camera_type_desc);
        QString urlstr = coach_1.at(3)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_1.at(3)->car_type_desc+"_"+coach_1.at(3)->camera_type_desc;
    }
}


void DeviceWindow::on_pushButton_cam5_clicked()
{
    libvlc_media_player_stop (_mp_deviceplayer);
    //SALOON CAM 1 for COACH 1
    if(iterate_button_pressed == 0 && coach_1.at(4)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_1.at(4)->camera_type_desc+"_"+QString::number(coach_1.at(4)->sno));
        QString urlstr = coach_1.at(4)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_1.at(4)->car_type_desc+"_"+coach_1.at(4)->camera_type_desc+"_"+QString::number(coach_1.at(4)->sno);
    }
    //SALOON CAM 1 for COACH 2
    else if(iterate_button_pressed == 1 && coach_2.at(0)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_2.at(0)->camera_type_desc+"_"+QString::number(coach_2.at(0)->sno));
        QString urlstr = coach_2.at(0)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_2.at(0)->car_type_desc+"_"+coach_2.at(0)->camera_type_desc+"_"+QString::number(coach_2.at(0)->sno);
    }
    //SALOON CAM 1 for COACH 3
    else if(iterate_button_pressed == 2 && coach_3.at(0)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_3.at(0)->camera_type_desc+"_"+QString::number(coach_3.at(0)->sno));
        QString urlstr = coach_3.at(0)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_3.at(0)->car_type_desc+"_"+coach_3.at(0)->camera_type_desc+"_"+QString::number(coach_3.at(0)->sno);
    }
    //SALOON CAM 1 for COACH 4
    else if(iterate_button_pressed == 3 && coach_4.at(0)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_4.at(0)->camera_type_desc+"_"+QString::number(coach_4.at(0)->sno));
        QString urlstr = coach_4.at(0)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_4.at(0)->car_type_desc+"_"+coach_4.at(0)->camera_type_desc+"_"+QString::number(coach_4.at(0)->sno);
    }
    //SALOON CAM 1 for COACH 5
    else if(iterate_button_pressed == 4 && coach_5.at(0)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_5.at(0)->camera_type_desc+"_"+QString::number(coach_5.at(0)->sno));
        QString urlstr = coach_5.at(0)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_5.at(0)->car_type_desc+"_"+coach_5.at(0)->camera_type_desc+"_"+QString::number(coach_5.at(0)->sno);
    }
    //SALOON CAM 1 for COACH 6
    else if(iterate_button_pressed == 5 && coach_6.at(0)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_6.at(0)->camera_type_desc+"_"+QString::number(coach_6.at(0)->sno));
        QString urlstr = coach_6.at(0)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_6.at(0)->car_type_desc+"_"+coach_6.at(0)->camera_type_desc+"_"+QString::number(coach_6.at(0)->sno);
    }
    //SALOON CAM 1 for COACH 7
    else if(iterate_button_pressed == 6 && coach_7.at(0)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_7.at(0)->camera_type_desc+"_"+QString::number(coach_7.at(0)->sno));
        QString urlstr = coach_7.at(0)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_7.at(0)->car_type_desc+"_"+coach_7.at(0)->camera_type_desc+"_"+QString::number(coach_7.at(0)->sno);
    }
    //SALOON CAM 1 for COACH 8
    else if(iterate_button_pressed == 7 && coach_8.at(0)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_8.at(0)->camera_type_desc+"_"+QString::number(coach_8.at(0)->sno));
        QString urlstr = coach_8.at(0)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_8.at(0)->car_type_desc+"_"+coach_8.at(0)->camera_type_desc+"_"+QString::number(coach_8.at(0)->sno);
    }
    //SALOON CAM 1 for COACH 9
    else if(iterate_button_pressed == 8 && coach_9.at(0)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_9.at(0)->camera_type_desc+"_"+QString::number(coach_9.at(0)->sno));
        QString urlstr = coach_9.at(0)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_9.at(0)->car_type_desc+"_"+coach_9.at(0)->camera_type_desc+"_"+QString::number(coach_9.at(0)->sno);
    }
    //SALOON CAM 1 for COACH 10
    else if(iterate_button_pressed == 9 && coach_10.at(0)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_10.at(0)->camera_type_desc+"_"+QString::number(coach_10.at(0)->sno));
        QString urlstr = coach_10.at(0)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_10.at(0)->car_type_desc+"_"+coach_10.at(0)->camera_type_desc+"_"+QString::number(coach_10.at(0)->sno);
    }
    //SALOON CAM 1 for COACH 11
    else if(iterate_button_pressed == 10 && coach_11.at(0)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_11.at(0)->camera_type_desc+"_"+QString::number(coach_11.at(0)->sno));
        QString urlstr = coach_11.at(0)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_11.at(0)->car_type_desc+"_"+coach_11.at(0)->camera_type_desc+"_"+QString::number(coach_11.at(0)->sno);
    }
    //SALOON CAM 1 for COACH 12
    else if(iterate_button_pressed == 11 && coach_12.at(4)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_12.at(4)->camera_type_desc+"_"+QString::number(coach_12.at(4)->sno));
        QString urlstr = coach_12.at(4)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_12.at(4)->car_type_desc+"_"+coach_12.at(4)->camera_type_desc+"_"+QString::number(coach_12.at(4)->sno);
    }

}


void DeviceWindow::on_pushButton_cam6_clicked()
{
    libvlc_media_player_stop (_mp_deviceplayer);
    //SALOON CAM 2 for COACH 1
    if(iterate_button_pressed == 0 && coach_1.at(5)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_1.at(5)->camera_type_desc+"_"+QString::number(coach_1.at(5)->sno));
        QString urlstr = coach_1.at(5)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_1.at(5)->car_type_desc+"_"+coach_1.at(5)->camera_type_desc+"_"+QString::number(coach_1.at(5)->sno);
    }
    //SALOON CAM 2 for COACH 2
    else if(iterate_button_pressed == 1 && coach_2.at(1)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_2.at(1)->camera_type_desc+"_"+QString::number(coach_2.at(1)->sno));
        QString urlstr = coach_2.at(1)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_2.at(1)->car_type_desc+"_"+coach_2.at(1)->camera_type_desc+"_"+QString::number(coach_2.at(1)->sno);
    }
    //SALOON CAM 2 for COACH 3
    else if(iterate_button_pressed == 2 && coach_3.at(1)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_3.at(1)->camera_type_desc+"_"+QString::number(coach_3.at(1)->sno));
        QString urlstr = coach_3.at(1)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_3.at(1)->car_type_desc+"_"+coach_3.at(1)->camera_type_desc+"_"+QString::number(coach_3.at(1)->sno);
    }
    //SALOON CAM 2 for COACH 4
    else if(iterate_button_pressed == 3 && coach_4.at(1)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_4.at(1)->camera_type_desc+"_"+QString::number(coach_4.at(1)->sno));
        QString urlstr = coach_4.at(1)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_4.at(1)->car_type_desc+"_"+coach_4.at(1)->camera_type_desc+"_"+QString::number(coach_4.at(1)->sno);
    }
    //SALOON CAM 2 for COACH 5
    else if(iterate_button_pressed == 4 && coach_5.at(1)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_5.at(1)->camera_type_desc+"_"+QString::number(coach_5.at(1)->sno));
        QString urlstr = coach_5.at(1)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_5.at(1)->car_type_desc+"_"+coach_5.at(1)->camera_type_desc+"_"+QString::number(coach_5.at(1)->sno);
    }
    //SALOON CAM 2 for COACH 6
    else if(iterate_button_pressed == 5 && coach_6.at(1)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_6.at(1)->camera_type_desc+"_"+QString::number(coach_6.at(1)->sno));
        QString urlstr = coach_6.at(1)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_6.at(1)->car_type_desc+"_"+coach_6.at(1)->camera_type_desc+"_"+QString::number(coach_6.at(1)->sno);
    }
    //SALOON CAM 2 for COACH 7
    else if(iterate_button_pressed == 6 && coach_7.at(1)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_7.at(1)->camera_type_desc+"_"+QString::number(coach_7.at(1)->sno));
        QString urlstr = coach_7.at(1)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_7.at(1)->car_type_desc+"_"+coach_7.at(1)->camera_type_desc+"_"+QString::number(coach_7.at(1)->sno);
    }
    //SALOON CAM 2 for COACH 8
    else if(iterate_button_pressed == 7 && coach_8.at(1)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_8.at(1)->camera_type_desc+"_"+QString::number(coach_8.at(1)->sno));
        QString urlstr = coach_8.at(1)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_8.at(1)->car_type_desc+"_"+coach_8.at(1)->camera_type_desc+"_"+QString::number(coach_8.at(1)->sno);
    }
    //SALOON CAM 2 for COACH 9
    else if(iterate_button_pressed == 8 && coach_9.at(1)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_9.at(1)->camera_type_desc+"_"+QString::number(coach_9.at(1)->sno));
        QString urlstr = coach_9.at(1)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_9.at(1)->car_type_desc+"_"+coach_9.at(1)->camera_type_desc+"_"+QString::number(coach_9.at(1)->sno);
    }
    //SALOON CAM 2 for COACH 10
    else if(iterate_button_pressed == 9 && coach_10.at(1)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_10.at(1)->camera_type_desc+"_"+QString::number(coach_10.at(1)->sno));
        QString urlstr = coach_10.at(1)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_10.at(1)->car_type_desc+"_"+coach_10.at(1)->camera_type_desc+"_"+QString::number(coach_10.at(1)->sno);
    }
    //SALOON CAM 2 for COACH 11
    else if(iterate_button_pressed == 10 && coach_11.at(1)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_11.at(1)->camera_type_desc+"_"+QString::number(coach_11.at(1)->sno));
        QString urlstr = coach_11.at(1)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_11.at(1)->car_type_desc+"_"+coach_11.at(1)->camera_type_desc+"_"+QString::number(coach_11.at(1)->sno);
    }
    //SALOON CAM 2 for COACH 12
    else if(iterate_button_pressed == 11 && coach_12.at(5)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_12.at(5)->camera_type_desc+"_"+QString::number(coach_12.at(5)->sno));
        QString urlstr = coach_12.at(5)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        const char* url = "rtsp://"+url_ba+"/video1.sdp";
        _m_deviceplayer = libvlc_media_new_location(_vlcinstance_deviceplayer, url);
        libvlc_media_player_set_media (_mp_deviceplayer, _m_deviceplayer);
        int windid = ui->frame->winId();
        libvlc_media_player_set_xwindow (_mp_deviceplayer, windid );
        libvlc_media_player_play (_mp_deviceplayer);
        _isPlaying=true;
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_12.at(5)->car_type_desc+"_"+coach_12.at(5)->camera_type_desc+"_"+QString::number(coach_12.at(5)->sno);
    }

}


void DeviceWindow::on_pushButton_cam7_clicked()
{
    libvlc_media_player_stop (_mp_deviceplayer);
    //SALOON CAM 3 for COACH 1
    if(iterate_button_pressed == 0 && coach_1.at(6)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_1.at(6)->camera_type_desc+"_"+QString::number(coach_1.at(6)->sno));
        QString urlstr = coach_1.at(6)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_1.at(6)->car_type_desc+"_"+coach_1.at(6)->camera_type_desc+"_"+QString::number(coach_1.at(6)->sno);
    }
    //SALOON CAM 3 for COACH 2
    else if(iterate_button_pressed == 1 && coach_2.at(2)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_2.at(2)->camera_type_desc+"_"+QString::number(coach_2.at(2)->sno));
        QString urlstr = coach_2.at(2)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_2.at(2)->car_type_desc+"_"+coach_2.at(2)->camera_type_desc+"_"+QString::number(coach_2.at(2)->sno);
    }
    //SALOON CAM 3 for COACH 3
    else if(iterate_button_pressed == 2 && coach_3.at(2)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_3.at(2)->camera_type_desc+"_"+QString::number(coach_3.at(2)->sno));
        QString urlstr = coach_3.at(2)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_3.at(2)->car_type_desc+"_"+coach_3.at(2)->camera_type_desc+"_"+QString::number(coach_3.at(2)->sno);
    }
    //SALOON CAM 3 for COACH 4
    else if(iterate_button_pressed == 3 && coach_4.at(2)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_4.at(2)->camera_type_desc+"_"+QString::number(coach_4.at(2)->sno));
        QString urlstr = coach_4.at(2)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_4.at(2)->car_type_desc+"_"+coach_4.at(2)->camera_type_desc+"_"+QString::number(coach_4.at(2)->sno);
    }
    //SALOON CAM 3 for COACH 5
    else if(iterate_button_pressed == 4 && coach_5.at(2)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_5.at(2)->camera_type_desc+"_"+QString::number(coach_5.at(2)->sno));
        QString urlstr = coach_5.at(2)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_5.at(2)->car_type_desc+"_"+coach_5.at(2)->camera_type_desc+"_"+QString::number(coach_5.at(2)->sno);
    }
    //SALOON CAM 3 for COACH 6
    else if(iterate_button_pressed == 5 && coach_6.at(2)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_6.at(2)->camera_type_desc+"_"+QString::number(coach_6.at(2)->sno));
        QString urlstr = coach_6.at(2)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_6.at(2)->car_type_desc+"_"+coach_6.at(2)->camera_type_desc+"_"+QString::number(coach_6.at(2)->sno);
    }
    //SALOON CAM 3 for COACH 7
    else if(iterate_button_pressed == 6 && coach_7.at(2)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_7.at(2)->camera_type_desc+"_"+QString::number(coach_7.at(2)->sno));
        QString urlstr = coach_7.at(2)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_7.at(2)->car_type_desc+"_"+coach_7.at(2)->camera_type_desc+"_"+QString::number(coach_7.at(2)->sno);
    }
    //SALOON CAM 3 for COACH 8
    else if(iterate_button_pressed == 7 && coach_8.at(2)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_8.at(2)->camera_type_desc+"_"+QString::number(coach_8.at(2)->sno));
        QString urlstr = coach_8.at(2)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_8.at(2)->car_type_desc+"_"+coach_8.at(2)->camera_type_desc+"_"+QString::number(coach_8.at(2)->sno);
    }
    //SALOON CAM 3 for COACH 9
    else if(iterate_button_pressed == 8 && coach_9.at(2)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_9.at(2)->camera_type_desc+"_"+QString::number(coach_9.at(2)->sno));
        QString urlstr = coach_9.at(2)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_9.at(2)->car_type_desc+"_"+coach_9.at(2)->camera_type_desc+"_"+QString::number(coach_9.at(2)->sno);
    }
    //SALOON CAM 3 for COACH 10
    else if(iterate_button_pressed == 9 && coach_10.at(2)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_10.at(2)->camera_type_desc+"_"+QString::number(coach_10.at(2)->sno));
        QString urlstr = coach_10.at(2)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_10.at(2)->car_type_desc+"_"+coach_10.at(2)->camera_type_desc+"_"+QString::number(coach_10.at(2)->sno);
    }
    //SALOON CAM 3 for COACH 11
    else if(iterate_button_pressed == 10 && coach_11.at(2)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_11.at(2)->camera_type_desc+"_"+QString::number(coach_11.at(2)->sno));
        QString urlstr = coach_11.at(2)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_11.at(2)->car_type_desc+"_"+coach_11.at(2)->camera_type_desc+"_"+QString::number(coach_11.at(2)->sno);
    }
    //SALOON CAM 3 for COACH 12
    else if(iterate_button_pressed == 11 && coach_12.at(6)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_12.at(6)->camera_type_desc+"_"+QString::number(coach_12.at(6)->sno));
        QString urlstr = coach_12.at(6)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_12.at(6)->car_type_desc+"_"+coach_12.at(6)->camera_type_desc+"_"+QString::number(coach_12.at(6)->sno);
    }
}


void DeviceWindow::on_pushButton_cam8_clicked()
{
    libvlc_media_player_stop (_mp_deviceplayer);
    //SALOON CAM 4 for COACH 1
    if(iterate_button_pressed == 0 && coach_1.at(7)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_1.at(7)->camera_type_desc+"_"+QString::number(coach_1.at(7)->sno));
        QString urlstr = coach_1.at(7)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_1.at(7)->car_type_desc+"_"+coach_1.at(7)->camera_type_desc+"_"+QString::number(coach_1.at(7)->sno);
    }
    //SALOON CAM 4 for COACH 2
    else if(iterate_button_pressed == 1 && coach_2.at(3)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_2.at(3)->camera_type_desc+"_"+QString::number(coach_2.at(3)->sno));
        QString urlstr = coach_2.at(3)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_2.at(3)->car_type_desc+"_"+coach_2.at(3)->camera_type_desc+"_"+QString::number(coach_2.at(3)->sno);
    }
    //SALOON CAM 4 for COACH 3
    else if(iterate_button_pressed == 2 && coach_3.at(3)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_3.at(3)->camera_type_desc+"_"+QString::number(coach_3.at(3)->sno));
        QString urlstr = coach_3.at(3)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_3.at(3)->car_type_desc+"_"+coach_3.at(3)->camera_type_desc+"_"+QString::number(coach_3.at(3)->sno);
    }
    //SALOON CAM 4 for COACH 4
    else if(iterate_button_pressed == 3 && coach_4.at(3)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_4.at(3)->camera_type_desc+"_"+QString::number(coach_4.at(3)->sno));
        QString urlstr = coach_4.at(3)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_4.at(3)->car_type_desc+"_"+coach_4.at(3)->camera_type_desc+"_"+QString::number(coach_4.at(3)->sno);
    }
    //SALOON CAM 4 for COACH 5
    else if(iterate_button_pressed == 4 && coach_5.at(3)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_5.at(3)->camera_type_desc+"_"+QString::number(coach_5.at(3)->sno));
        QString urlstr = coach_5.at(3)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_5.at(3)->car_type_desc+"_"+coach_5.at(3)->camera_type_desc+"_"+QString::number(coach_5.at(3)->sno);
    }
    //SALOON CAM 4 for COACH 6
    else if(iterate_button_pressed == 5 && coach_6.at(3)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_6.at(3)->camera_type_desc+"_"+QString::number(coach_6.at(3)->sno));
        QString urlstr = coach_6.at(3)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_6.at(3)->car_type_desc+"_"+coach_6.at(3)->camera_type_desc+"_"+QString::number(coach_6.at(3)->sno);
    }
    //SALOON CAM 4 for COACH 7
    else if(iterate_button_pressed == 6 && coach_7.at(3)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_7.at(3)->camera_type_desc+"_"+QString::number(coach_7.at(3)->sno));
        QString urlstr = coach_7.at(3)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_7.at(3)->car_type_desc+"_"+coach_7.at(3)->camera_type_desc+"_"+QString::number(coach_7.at(3)->sno);
    }
    //SALOON CAM 4 for COACH 8
    else if(iterate_button_pressed == 7 && coach_8.at(3)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_8.at(3)->camera_type_desc+"_"+QString::number(coach_8.at(3)->sno));
        QString urlstr = coach_8.at(3)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_8.at(3)->car_type_desc+"_"+coach_8.at(3)->camera_type_desc+"_"+QString::number(coach_8.at(3)->sno);
    }
    //SALOON CAM 4 for COACH 9
    else if(iterate_button_pressed == 8 && coach_9.at(3)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_9.at(3)->camera_type_desc+"_"+QString::number(coach_9.at(3)->sno));
        QString urlstr = coach_9.at(3)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_9.at(3)->car_type_desc+"_"+coach_9.at(3)->camera_type_desc+"_"+QString::number(coach_9.at(3)->sno);
    }
    //SALOON CAM 4 for COACH 10
    else if(iterate_button_pressed == 9 && coach_10.at(3)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_10.at(3)->camera_type_desc+"_"+QString::number(coach_10.at(3)->sno));
        QString urlstr = coach_10.at(3)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_10.at(3)->car_type_desc+"_"+coach_10.at(3)->camera_type_desc+"_"+QString::number(coach_10.at(3)->sno);
    }
    //SALOON CAM 4 for COACH 11
    else if(iterate_button_pressed == 10 && coach_11.at(3)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_11.at(3)->camera_type_desc+"_"+QString::number(coach_11.at(3)->sno));
        QString urlstr = coach_11.at(3)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_11.at(3)->car_type_desc+"_"+coach_11.at(3)->camera_type_desc+"_"+QString::number(coach_11.at(3)->sno);
    }
    //SALOON CAM 4 for COACH 12
    else if(iterate_button_pressed == 11 && coach_12.at(7)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_12.at(7)->camera_type_desc+"_"+QString::number(coach_12.at(7)->sno));
        QString urlstr = coach_12.at(7)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_12.at(7)->car_type_desc+"_"+coach_12.at(7)->camera_type_desc+"_"+QString::number(coach_12.at(7)->sno);
    }
}


void DeviceWindow::on_pushButton_cam9_clicked()
{
    libvlc_media_player_stop (_mp_deviceplayer);
    //SALOON CAM 5 for COACH 1
    if(iterate_button_pressed == 0 && coach_1.at(8)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_1.at(8)->camera_type_desc+"_"+QString::number(coach_1.at(8)->sno));
        QString urlstr = coach_1.at(8)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_1.at(8)->car_type_desc+"_"+coach_1.at(8)->camera_type_desc+"_"+QString::number(coach_1.at(8)->sno);
    }
    //SALOON CAM 5 for COACH 2
    else if(iterate_button_pressed == 1 && coach_2.at(4)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_2.at(4)->camera_type_desc+"_"+QString::number(coach_2.at(4)->sno));
        QString urlstr = coach_2.at(4)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_2.at(4)->car_type_desc+"_"+coach_2.at(4)->camera_type_desc+"_"+QString::number(coach_2.at(4)->sno);
    }
    //SALOON CAM 5 for COACH 3
    else if(iterate_button_pressed == 2 && coach_3.at(4)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_3.at(4)->camera_type_desc+"_"+QString::number(coach_3.at(4)->sno));
        QString urlstr = coach_3.at(4)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_3.at(4)->car_type_desc+"_"+coach_3.at(4)->camera_type_desc+"_"+QString::number(coach_3.at(4)->sno);
    }
    //SALOON CAM 5 for COACH 4
    else if(iterate_button_pressed == 3 && coach_4.at(4)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_4.at(4)->camera_type_desc+"_"+QString::number(coach_4.at(4)->sno));
        QString urlstr = coach_4.at(4)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_4.at(4)->car_type_desc+"_"+coach_4.at(4)->camera_type_desc+"_"+QString::number(coach_4.at(4)->sno);
    }
    //SALOON CAM 5 for COACH 5
    else if(iterate_button_pressed == 4 && coach_5.at(4)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_5.at(4)->camera_type_desc+"_"+QString::number(coach_5.at(4)->sno));
        QString urlstr = coach_5.at(4)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_5.at(4)->car_type_desc+"_"+coach_5.at(4)->camera_type_desc+"_"+QString::number(coach_5.at(4)->sno);
    }
    //SALOON CAM 5 for COACH 6
    else if(iterate_button_pressed == 5 && coach_6.at(4)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_6.at(4)->camera_type_desc+"_"+QString::number(coach_6.at(4)->sno));
        QString urlstr = coach_6.at(4)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_6.at(4)->car_type_desc+"_"+coach_6.at(4)->camera_type_desc+"_"+QString::number(coach_6.at(4)->sno);
    }
    //SALOON CAM 5 for COACH 7
    else if(iterate_button_pressed == 6 && coach_7.at(4)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_7.at(4)->camera_type_desc+"_"+QString::number(coach_7.at(4)->sno));
        QString urlstr = coach_7.at(4)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_7.at(4)->car_type_desc+"_"+coach_7.at(4)->camera_type_desc+"_"+QString::number(coach_7.at(4)->sno);
    }
    //SALOON CAM 5 for COACH 8
    else if(iterate_button_pressed == 7 && coach_8.at(4)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_8.at(4)->camera_type_desc+"_"+QString::number(coach_8.at(4)->sno));
        QString urlstr = coach_8.at(4)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_8.at(4)->car_type_desc+"_"+coach_8.at(4)->camera_type_desc+"_"+QString::number(coach_8.at(4)->sno);
    }
    //SALOON CAM 5 for COACH 9
    else if(iterate_button_pressed == 8 && coach_9.at(4)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_9.at(4)->camera_type_desc+"_"+QString::number(coach_9.at(4)->sno));
        QString urlstr = coach_9.at(4)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_9.at(4)->car_type_desc+"_"+coach_9.at(4)->camera_type_desc+"_"+QString::number(coach_9.at(4)->sno);
    }
    //SALOON CAM 5 for COACH 10
    else if(iterate_button_pressed == 9 && coach_10.at(4)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_10.at(4)->camera_type_desc+"_"+QString::number(coach_10.at(4)->sno));
        QString urlstr = coach_10.at(4)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_10.at(4)->car_type_desc+"_"+coach_10.at(4)->camera_type_desc+"_"+QString::number(coach_10.at(4)->sno);
    }
    //SALOON CAM 5 for COACH 11
    else if(iterate_button_pressed == 10 && coach_11.at(4)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_11.at(4)->camera_type_desc+"_"+QString::number(coach_11.at(4)->sno));
        QString urlstr = coach_11.at(4)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_11.at(4)->car_type_desc+"_"+coach_11.at(4)->camera_type_desc+"_"+QString::number(coach_11.at(4)->sno);
    }
    //SALOON CAM 5 for COACH 12
    else if(iterate_button_pressed == 11 && coach_12.at(8)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_12.at(8)->camera_type_desc+"_"+QString::number(coach_12.at(8)->sno));
        QString urlstr = coach_12.at(8)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_12.at(8)->car_type_desc+"_"+coach_12.at(8)->camera_type_desc+"_"+QString::number(coach_12.at(8)->sno);
    }
}


void DeviceWindow::on_pushButton_cam10_clicked()
{
    libvlc_media_player_stop (_mp_deviceplayer);
    //SALOON CAM 6 for COACH 1
    if(iterate_button_pressed == 0 && coach_1.at(9)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_1.at(9)->camera_type_desc+"_"+QString::number(coach_1.at(9)->sno));
        QString urlstr = coach_1.at(9)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_1.at(9)->car_type_desc+"_"+coach_1.at(9)->camera_type_desc+"_"+QString::number(coach_1.at(9)->sno);
    }

    //SALOON CAM 6 for COACH 2
    else if(iterate_button_pressed == 1 && coach_2.at(5)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_2.at(5)->camera_type_desc+"_"+QString::number(coach_2.at(5)->sno));
        QString urlstr = coach_2.at(5)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_2.at(5)->car_type_desc+"_"+coach_2.at(5)->camera_type_desc+"_"+QString::number(coach_2.at(5)->sno);
    }
    //SALOON CAM 6 for COACH 3
    else if(iterate_button_pressed == 2 && coach_3.at(5)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_3.at(5)->camera_type_desc+"_"+QString::number(coach_3.at(5)->sno));
        QString urlstr = coach_3.at(5)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_3.at(5)->car_type_desc+"_"+coach_3.at(5)->camera_type_desc+"_"+QString::number(coach_3.at(5)->sno);
    }
    //SALOON CAM 6 for COACH 4
    else if(iterate_button_pressed == 3 && coach_4.at(5)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_4.at(5)->camera_type_desc+"_"+QString::number(coach_4.at(5)->sno));
        QString urlstr = coach_4.at(5)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_4.at(5)->car_type_desc+"_"+coach_4.at(5)->camera_type_desc+"_"+QString::number(coach_4.at(5)->sno);
    }
    //SALOON CAM 6 for COACH 5
    else if(iterate_button_pressed == 4 && coach_5.at(5)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_5.at(5)->camera_type_desc+"_"+QString::number(coach_5.at(5)->sno));
        QString urlstr = coach_5.at(5)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_5.at(5)->car_type_desc+"_"+coach_5.at(5)->camera_type_desc+"_"+QString::number(coach_5.at(5)->sno);
    }
    //SALOON CAM 6 for COACH 6
    else if(iterate_button_pressed == 5 && coach_6.at(5)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_6.at(5)->camera_type_desc+"_"+QString::number(coach_6.at(5)->sno));
        QString urlstr = coach_6.at(5)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_6.at(5)->car_type_desc+"_"+coach_6.at(5)->camera_type_desc+"_"+QString::number(coach_6.at(5)->sno);
    }
    //SALOON CAM 6 for COACH 7
    else if(iterate_button_pressed == 6 && coach_7.at(5)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_7.at(5)->camera_type_desc+"_"+QString::number(coach_7.at(5)->sno));
        QString urlstr = coach_7.at(5)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_7.at(5)->car_type_desc+"_"+coach_7.at(5)->camera_type_desc+"_"+QString::number(coach_7.at(5)->sno);
    }
    //SALOON CAM 6 for COACH 8
    else if(iterate_button_pressed == 7 && coach_8.at(5)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_8.at(5)->camera_type_desc+"_"+QString::number(coach_8.at(5)->sno));
        QString urlstr = coach_8.at(5)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_8.at(5)->car_type_desc+"_"+coach_8.at(5)->camera_type_desc+"_"+QString::number(coach_8.at(5)->sno);
    }
    //SALOON CAM 6 for COACH 9
    else if(iterate_button_pressed == 8 && coach_9.at(5)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_9.at(5)->camera_type_desc+"_"+QString::number(coach_9.at(5)->sno));
        QString urlstr = coach_9.at(5)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_9.at(5)->car_type_desc+"_"+coach_9.at(5)->camera_type_desc+"_"+QString::number(coach_9.at(5)->sno);
    }
    //SALOON CAM 6 for COACH 10
    else if(iterate_button_pressed == 9 && coach_10.at(5)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_10.at(5)->camera_type_desc+"_"+QString::number(coach_10.at(5)->sno));
        QString urlstr = coach_10.at(5)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_10.at(5)->car_type_desc+"_"+coach_10.at(5)->camera_type_desc+"_"+QString::number(coach_10.at(5)->sno);
    }
    //SALOON CAM 6 for COACH 11
    else if(iterate_button_pressed == 10 && coach_11.at(5)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_11.at(5)->camera_type_desc+"_"+QString::number(coach_11.at(5)->sno));
        QString urlstr = coach_11.at(5)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_11.at(5)->car_type_desc+"_"+coach_11.at(5)->camera_type_desc+"_"+QString::number(coach_11.at(5)->sno);
    }
    //SALOON CAM 6 for COACH 12
    else if(iterate_button_pressed == 11 && coach_12.at(9)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_12.at(9)->camera_type_desc+"_"+QString::number(coach_12.at(9)->sno));
        QString urlstr = coach_12.at(9)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_12.at(9)->car_type_desc+"_"+coach_12.at(9)->camera_type_desc+"_"+QString::number(coach_12.at(9)->sno);
    }

}


void DeviceWindow::on_pushButton_cam11_clicked()
{
    libvlc_media_player_stop (_mp_deviceplayer);
    //SALOON CAM 7 for COACH 1
    if(iterate_button_pressed == 0 && coach_1.at(10)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_1.at(10)->camera_type_desc+"_"+QString::number(coach_1.at(10)->sno));
        QString urlstr = coach_1.at(10)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_1.at(10)->car_type_desc+"_"+coach_1.at(10)->camera_type_desc+"_"+QString::number(coach_1.at(10)->sno);
    }
    //SALOON CAM 7 for COACH 2
    else if(iterate_button_pressed == 1 && coach_2.at(6)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_2.at(6)->camera_type_desc+"_"+QString::number(coach_2.at(6)->sno));
        QString urlstr = coach_2.at(6)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_2.at(6)->car_type_desc+"_"+coach_2.at(6)->camera_type_desc+"_"+QString::number(coach_2.at(6)->sno);
    }
    //SALOON CAM 7 for COACH 3
    else if(iterate_button_pressed == 2 && coach_3.at(6)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_3.at(6)->camera_type_desc+"_"+QString::number(coach_3.at(6)->sno));
        QString urlstr = coach_3.at(6)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_3.at(6)->car_type_desc+"_"+coach_3.at(6)->camera_type_desc+"_"+QString::number(coach_3.at(6)->sno);
    }
    //SALOON CAM 7 for COACH 4
    else if(iterate_button_pressed == 3 && coach_4.at(6)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_4.at(6)->camera_type_desc+"_"+QString::number(coach_4.at(6)->sno));
        QString urlstr = coach_4.at(6)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_4.at(6)->car_type_desc+"_"+coach_4.at(6)->camera_type_desc+"_"+QString::number(coach_4.at(6)->sno);
    }
    //SALOON CAM 7 for COACH 5
    else if(iterate_button_pressed == 4 && coach_5.at(6)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_5.at(6)->camera_type_desc+"_"+QString::number(coach_5.at(6)->sno));
        QString urlstr = coach_5.at(6)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_5.at(6)->car_type_desc+"_"+coach_5.at(6)->camera_type_desc+"_"+QString::number(coach_5.at(6)->sno);
    }
    //SALOON CAM 7 for COACH 6
    else if(iterate_button_pressed == 5 && coach_6.at(6)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_6.at(6)->camera_type_desc+"_"+QString::number(coach_6.at(6)->sno));
        QString urlstr = coach_6.at(6)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_6.at(6)->car_type_desc+"_"+coach_6.at(6)->camera_type_desc+"_"+QString::number(coach_6.at(6)->sno);
    }
    //SALOON CAM 7 for COACH 7
    else if(iterate_button_pressed == 6 && coach_7.at(6)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_7.at(6)->camera_type_desc+"_"+QString::number(coach_7.at(6)->sno));
        QString urlstr = coach_7.at(6)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_7.at(6)->car_type_desc+"_"+coach_7.at(6)->camera_type_desc+"_"+QString::number(coach_7.at(6)->sno);
    }
    //SALOON CAM 7 for COACH 8
    else if(iterate_button_pressed == 7 && coach_8.at(6)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_8.at(6)->camera_type_desc+"_"+QString::number(coach_8.at(6)->sno));
        QString urlstr = coach_8.at(6)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_8.at(6)->car_type_desc+"_"+coach_8.at(6)->camera_type_desc+"_"+QString::number(coach_8.at(6)->sno);
    }
    //SALOON CAM 7 for COACH 9
    else if(iterate_button_pressed == 8 && coach_9.at(6)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_9.at(6)->camera_type_desc+"_"+QString::number(coach_9.at(6)->sno));
        QString urlstr = coach_9.at(6)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_9.at(6)->car_type_desc+"_"+coach_9.at(6)->camera_type_desc+"_"+QString::number(coach_9.at(6)->sno);
    }
    //SALOON CAM 7 for COACH 10
    else if(iterate_button_pressed == 9 && coach_10.at(6)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_10.at(6)->camera_type_desc+"_"+QString::number(coach_10.at(6)->sno));
        QString urlstr = coach_10.at(6)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_10.at(6)->car_type_desc+"_"+coach_10.at(6)->camera_type_desc+"_"+QString::number(coach_10.at(6)->sno);
    }
    //SALOON CAM 7 for COACH 11
    else if(iterate_button_pressed == 10 && coach_11.at(6)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_11.at(6)->camera_type_desc+"_"+QString::number(coach_11.at(6)->sno));
        QString urlstr = coach_11.at(6)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_11.at(6)->car_type_desc+"_"+coach_11.at(6)->camera_type_desc+"_"+QString::number(coach_11.at(6)->sno);
    }
    //SALOON CAM 7 for COACH 12
    else if(iterate_button_pressed == 11 && coach_12.at(10)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_12.at(10)->camera_type_desc+"_"+QString::number(coach_12.at(10)->sno));
        QString urlstr = coach_12.at(10)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_12.at(10)->car_type_desc+"_"+coach_12.at(10)->camera_type_desc+"_"+QString::number(coach_12.at(10)->sno);
    }

}


void DeviceWindow::on_pushButton_cam12_clicked()
{
    libvlc_media_player_stop (_mp_deviceplayer);
    //SALOON CAM 8 for COACH 1
    if(iterate_button_pressed == 0 && coach_1.at(11)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_1.at(11)->camera_type_desc+"_"+QString::number(coach_1.at(11)->sno));
        QString urlstr = coach_1.at(11)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_1.at(11)->car_type_desc+"_"+coach_1.at(11)->camera_type_desc+"_"+QString::number(coach_1.at(11)->sno);
    }
    //SALOON CAM 8 for COACH 2
    else if(iterate_button_pressed == 1 && coach_2.at(7)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_2.at(7)->camera_type_desc+"_"+QString::number(coach_2.at(7)->sno));
        QString urlstr = coach_2.at(7)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_2.at(7)->car_type_desc+"_"+coach_2.at(7)->camera_type_desc+"_"+QString::number(coach_2.at(7)->sno);
    }
    //SALOON CAM 8 for COACH 3
    else if(iterate_button_pressed == 2 && coach_3.at(7)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_3.at(7)->camera_type_desc+"_"+QString::number(coach_3.at(7)->sno));
        QString urlstr = coach_3.at(7)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_3.at(7)->car_type_desc+"_"+coach_3.at(7)->camera_type_desc+"_"+QString::number(coach_3.at(7)->sno);
    }
    //SALOON CAM 8 for COACH 4
    else if(iterate_button_pressed == 3 && coach_4.at(7)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_4.at(7)->camera_type_desc+"_"+QString::number(coach_4.at(7)->sno));
        QString urlstr = coach_4.at(7)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_4.at(7)->car_type_desc+"_"+coach_4.at(7)->camera_type_desc+"_"+QString::number(coach_4.at(7)->sno);
    }
    //SALOON CAM 8 for COACH 5
    else if(iterate_button_pressed == 4 && coach_5.at(7)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_5.at(7)->camera_type_desc+"_"+QString::number(coach_5.at(7)->sno));
        QString urlstr = coach_5.at(7)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_5.at(7)->car_type_desc+"_"+coach_5.at(7)->camera_type_desc+"_"+QString::number(coach_5.at(7)->sno);
    }
    //SALOON CAM 8 for COACH 6
    else if(iterate_button_pressed == 5 && coach_6.at(7)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_6.at(7)->camera_type_desc+"_"+QString::number(coach_6.at(7)->sno));
        QString urlstr = coach_6.at(7)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_6.at(7)->car_type_desc+"_"+coach_6.at(7)->camera_type_desc+"_"+QString::number(coach_6.at(7)->sno);
    }
    //SALOON CAM 8 for COACH 7
    else if(iterate_button_pressed == 6 && coach_7.at(7)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_7.at(7)->camera_type_desc+"_"+QString::number(coach_7.at(7)->sno));
        QString urlstr = coach_7.at(7)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_7.at(7)->car_type_desc+"_"+coach_7.at(7)->camera_type_desc+"_"+QString::number(coach_7.at(7)->sno);
    }
    //SALOON CAM 8 for COACH 8
    else if(iterate_button_pressed == 7 && coach_8.at(7)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_8.at(7)->camera_type_desc+"_"+QString::number(coach_8.at(7)->sno));
        QString urlstr = coach_8.at(7)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_8.at(7)->car_type_desc+"_"+coach_8.at(7)->camera_type_desc+"_"+QString::number(coach_8.at(7)->sno);
    }
    //SALOON CAM 8 for COACH 9
    else if(iterate_button_pressed == 8 && coach_9.at(7)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_9.at(7)->camera_type_desc+"_"+QString::number(coach_9.at(7)->sno));
        QString urlstr = coach_9.at(7)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_9.at(7)->car_type_desc+"_"+coach_9.at(7)->camera_type_desc+"_"+QString::number(coach_9.at(7)->sno);
    }
    //SALOON CAM 8 for COACH 10
    else if(iterate_button_pressed == 9 && coach_10.at(7)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_10.at(7)->camera_type_desc+"_"+QString::number(coach_10.at(7)->sno));
        QString urlstr = coach_10.at(7)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_10.at(7)->car_type_desc+"_"+coach_10.at(7)->camera_type_desc+"_"+QString::number(coach_10.at(7)->sno);
    }
    //SALOON CAM 8 for COACH 11
    else if(iterate_button_pressed == 10 && coach_11.at(7)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_11.at(7)->camera_type_desc+"_"+QString::number(coach_11.at(7)->sno));
        QString urlstr = coach_11.at(7)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_11.at(7)->car_type_desc+"_"+coach_11.at(7)->camera_type_desc+"_"+QString::number(coach_11.at(7)->sno);
    }
    //SALOON CAM 8 for COACH 12
    else if(iterate_button_pressed == 11 && coach_12.at(11)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_12.at(11)->camera_type_desc+"_"+QString::number(coach_12.at(11)->sno));
        QString urlstr = coach_12.at(11)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_12.at(11)->car_type_desc+"_"+coach_12.at(11)->camera_type_desc+"_"+QString::number(coach_12.at(11)->sno);
    }
}

void DeviceWindow::on_pushButton_cam13_clicked()
{
    libvlc_media_player_stop (_mp_deviceplayer);
    //REAR CAM (only valid for COACH 12)
    if(iterate_button_pressed == 11 && coach_12.at(0)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_12.at(0)->camera_type_desc);
        QString urlstr = coach_12.at(0)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_12.at(0)->car_type_desc+"_"+coach_12.at(0)->camera_type_desc;
    }
}

void DeviceWindow::on_pushButton_cam14_clicked()
{
    libvlc_media_player_stop (_mp_deviceplayer);
    //REAR RIGHT (only valid for COACH 12)
    if(iterate_button_pressed == 11 && coach_12.at(1)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_12.at(1)->camera_type_desc);
        QString urlstr = coach_12.at(1)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_12.at(1)->car_type_desc+"_"+coach_12.at(1)->camera_type_desc;
    }
}

void DeviceWindow::on_pushButton_cam15_clicked()
{
    libvlc_media_player_stop (_mp_deviceplayer);
    //REAR LEFT (only valid for COACH 12)
    if(iterate_button_pressed == 0 && coach_12.at(2)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_1.at(2)->camera_type_desc);
        QString urlstr = coach_12.at(2)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_12.at(2)->car_type_desc+"_"+coach_12.at(2)->camera_type_desc;
    }
}

void DeviceWindow::on_pushButton_cam16_clicked()
{
    libvlc_media_player_stop (_mp_deviceplayer);
    //REAR CAB (only valid for COACH 12)
    if(iterate_button_pressed == 0 && coach_12.at(3)->activeStatus == ACTIVE)
    {
        ui->stackedWidget->setCurrentIndex(3);
        ui->label_heading->setText(coach_12.at(3)->camera_type_desc);
        QString urlstr = coach_12.at(3)->ip_addr;
        QByteArray url_ba = urlstr.toLocal8Bit();
        url = "rtsp://"+url_ba+"/video1.sdp";
        videoPlayerD();
        ui->label_cam_info->setText("IP - " + urlstr);
        returncounter_deviceStatus = 2;
        recordStringDevice = "rtsp://"+url_ba+"/video1.sdp";
        camNoFileNameDevice = coach_12.at(3)->car_type_desc+"_"+coach_12.at(3)->camera_type_desc;
    }
}

void DeviceWindow::on_pushButton_record_clicked()
{
    recordedFileNameDevice = date_text_device_recording + "_" +time_text_device_recording;

    QString forRecordingStream = "gst-launch-1.0 -ev  rtspsrc location=" + recordStringDevice + " ! application/x-rtp, media=video, encoding-name=H264 ! queue ! rtph264depay "
                                                                                                "! h264parse ! matroskamux ! filesink location="+pathToVidArchives+date_text_device_recording+"_recordings/"+camNoFileNameDevice+"_"+recordedFileNameDevice+".mp4 &";
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













