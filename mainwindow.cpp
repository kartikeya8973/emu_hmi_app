#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include "logindialog.h"
//#include "driverlogin.h"
#include "infowindow.h"
#include "devicewindow.h"
#include "videoarchivewindow.h"
#include "etbarchivewindow.h"
#include "nvrwindow.h"
#include "settingswindow.h"
#include "logswindow.h"
#include "renamewindow.h"
#include <vlc/libvlc.h>
#include "pingthread.h"
#include "screenshot.h"
#include "defaults.h"
#include "etbcallwindow.h"
#include "common.h"
#include "etbcallprompt.h"
#include "ipcamsyncthread.h"
#include "slavesyncthread.h"


#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QFrame>
#include <QProcess>


extern QString filepath;

extern IpCamSyncThread *ipcamsyncthread;
extern SlaveSyncThread *slaveSyncThread;
extern systemconfig systemconfig;
extern slave_tasks_t    slave_task;
extern QList <slave*> papis_slaves;
extern QList <QList <slave*>> rake_devices;
extern QList <camera*> hmi_slaves;
extern QList <QList <camera*>> rake;

extern int iterate_button_pressed;

//return counter for main window
int returncounter_main;

int buttonpress;

// timer for keeping in check the active and inactive status on the application
QElapsedTimer timeractive;

QSqlDatabase passdb;
QSqlDatabase passdb_driver;

//integer to iterate btw IPCAM for full cam view
int iFullCam = 0;

//integer to iterate btw IPCAM for mosiac cam view
int iMosiacCam = 0;

//integer to iterate btw External IPCAM for 5 cam view (top 3 frames)
int iExtCam = 0;

//integer to iterate btw Saloon IPCAM for 5 cam view ( botton 2 frames)
int iSaloonCam = 0;

//String for recording stream
QString recordString = "";

//Name by which recorded stream is saved in VideoArhives
QString recordedFileName = "";

//Name by which recorded stream is saved when etb call is established
QString recordedFileNameEtb = "";

//Cam no which is  also part of the name by recorded stream is saved in VideoArhives
QString camNoFileName = "";

//String for storing date and time for directory and file creating
QString time_text_recording = "";
QString date_text_recording = "";

//Creating file for HMI logs
QDate date_logs = QDate::currentDate();
QString date_logs_str = date_logs.toString("yyyy.MM.dd");
QFile file(pathToLogs + date_logs_str + "/"+date_logs_str+"_systemlogs");

//Coach Wise list of IPCAM
QList<camera*> coach_1;
QList<camera*> coach_2;
QList<camera*> coach_3;
QList<camera*> coach_4;
QList<camera*> coach_5;
QList<camera*> coach_6;
QList<camera*> coach_7;
QList<camera*> coach_8;
QList<camera*> coach_9;
QList<camera*> coach_10;
QList<camera*> coach_11;
QList<camera*> coach_12;

//List of all Cameras (All Coaches)
QStringList listAllCams;

//List of External Cameras(Front & Rear Coach)
QStringList listExternalCams;

//List of Saloon Cameras (All Coaches)
QStringList listSaloonCams;

QString hmi_ipAdd;
QString hmi_ipAdd_2;
QString server_ipAdd;
QString server_ipAdd_2;

/*
    DDC1                50
    DDC2                52
    HMI1                70
    HMI2                72
    HCD1                111
    HCD2                112
    SS-ICDU1            90
    SS-ICDU2            x
    DS-ICDU1            92
    DS-ICDU2            x
    CC1                 80, 81
    CC2                 82, 83
    ETB1                60
    ETB2                62
    NW-SW1              x
    NW-SW2              x
    NVR-1               2
    NVR-2               3
    VOIP SERVER-1       22
    VOIP SERVER-2       24

    Cameras
        Front           221
        Rear            224
        Front Left      229
        Front Right     x   225
        Rear Left       x   226
        Read Right      x   227
        Saloon1         222
        Saloon2         223
        Saloon3         x   228
        Saloon4         231
        Saloon5         x   232
        Saloon6         x   233
        Saloon7         230
        Saloon8         x   234

 */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->showMaximized();
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(statusDateTime()));
    timer->start(); //timer to start the data and time on the status bar
    //Adding data and time widgets to the status bar
    ui->statusbar->addPermanentWidget(ui->label_Date);
    ui->statusbar->addPermanentWidget(ui->label_Time);

    //    Setting page 2 as default
    ui->stackedWidget_Dynamic->setCurrentIndex(0);

    //For playing udp videos in labels
    //    playVideo();

    //return counter set initially to 0
    returncounter_main = 0;

    //Creating Database
    passdb = QSqlDatabase::addDatabase("QSQLITE", "connection1");

    //Creating Database
    passdb_driver = QSqlDatabase::addDatabase("QSQLITE", "connection2");

    //disable buttons
    ui->pushButton_camView_1->setEnabled(false);
    ui->pushButton_camView_2->setEnabled(false);
    ui->pushButton_camView_full->setEnabled(false);
    ui->pushButton_return->setEnabled(false);
    ui->pushButton_left_up->setEnabled(false);
    ui->pushButton_right_up->setEnabled(false);
    ui->pushButton_left_down->setEnabled(false);
    ui->pushButton_right_down->setEnabled(false);
    ui->pushButton_frame_1->setEnabled(false);
    ui->pushButton_frame_2->setEnabled(false);
    ui->pushButton_frame_3->setEnabled(false);
    ui->pushButton_frame_4->setEnabled(false);
    ui->pushButton_frame_5->setEnabled(false);
    ui->pushButton_car1->setEnabled(false);
    ui->pushButton_Menu->setEnabled(false);

    ui->pushButton_Menu->setStyleSheet("image:url(:/new/icons/menu_disable_2.png);");

    //calling ping thread for cameras
    ipcamsyncthread = new IpCamSyncThread();
    ipcamsyncthread->update_config(systemconfig);
    //    connect(ipcamsyncthread,SIGNAL(slave_scan_progress_eth(/*int,*/QList<camera*>))
    //            ,this,SLOT(&DeviceWindow::slave_ping_update(/*int,*/QList<camera*>)));
    ipcamsyncthread->start(/*QThread::HighestPriority*/);

    //populating list with device status
    camera_ping_update(hmi_slaves);

    //calling ping thread for devices
    //    slaveSyncThread = new SlaveSyncThread();
    //    slaveSyncThread->update_config(systemconfig);
    //    //    connect(ipcamsyncthread,SIGNAL(slave_scan_progress_eth(/*int,*/QList<camera*>))
    //    //            ,this,SLOT(&DeviceWindow::slave_ping_update(/*int,*/QList<camera*>)));
    //    slaveSyncThread->start(/*QThread::HighestPriority*/);

    slave_ping_update(papis_slaves);



    //    timeractive = new QTimer(this);
    //    connect(timer, SIGNAL(timeout()),this,SLOT(openlogindialog()));
    //    timeractive->start();


    timeractive.start();

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
    _vlcinstance=libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);  //tricky calculation of the char space used

    // Create a media player playing environement
    _mp = libvlc_media_player_new (_vlcinstance);
    _mp2 = libvlc_media_player_new (_vlcinstance);
    _mp3 = libvlc_media_player_new (_vlcinstance);
    _mp4 = libvlc_media_player_new (_vlcinstance);
    _mp5 = libvlc_media_player_new (_vlcinstance);
    _mp6 = libvlc_media_player_new (_vlcinstance);
    _mp7 = libvlc_media_player_new (_vlcinstance);
    _mp8 = libvlc_media_player_new (_vlcinstance);
    _mp9 = libvlc_media_player_new (_vlcinstance);
    _mp10 = libvlc_media_player_new (_vlcinstance);
    _mp11 = libvlc_media_player_new (_vlcinstance);
    _mp12 = libvlc_media_player_new (_vlcinstance);
    _mp13 = libvlc_media_player_new (_vlcinstance);
    _mp14 = libvlc_media_player_new (_vlcinstance);
    _mp15 = libvlc_media_player_new (_vlcinstance);
    _mp16 = libvlc_media_player_new (_vlcinstance);

    //Function for getting RTSP streams using libvlc
    playStream();

    etbListner = new QTcpServer(this);
    etbListner->listen(QHostAddress::Any,1999);
    connect(etbListner,SIGNAL(newConnection()),this,SLOT(etb_connected()));

    trainStopLister = new QTcpServer(this);
    trainStopLister->listen(QHostAddress::Any,3333);
    connect(trainStopLister,SIGNAL(newConnection()),this,SLOT(train_stops_connected()));


    //    recordTimer = new QTimer(this);
    //    connect(recordTimer, SIGNAL(timeout()), this, SLOT(push_button_record()));
    //    recordTimeCtr = 0;


    //Creating VidArchives folder for current date
    system(qPrintable(createVidArchivesFolder));

    //Creatting ETBArchives folder for current date
    system(qPrintable(createEtbArchivesFolder));

    //Creatting logs folder for current date
    system(qPrintable(createLogsArchivesFolder));

    //Initialising thread for pinging devices
    //    PingThread *pingthread;
    //    pingthread = new PingThread();
    //    pingthread->setObjectName("first thread");
    //    pingthread->start(QThread::HighestPriority);


    //opens default screen
    opendefaultScreen();

    //download logs from NVR on boot for each day
    downloadLogs();

    //Creating logs for HMI for each day
    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            // File was successfully created
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", LOG FILE CREATED & SYSTEM BOOTED\n";
            file.close();
        }
    }
    else{
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            // File was successfully created
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", SYSTEM BOOTED\n";
            file.close();
        }
    }

    //Chaging Voip icon
    //    ping_voip_icon();

    if(iExtCam==0){
        ui->label_frame_1->setText("COACH - "+QString::number(coach_1.at(iExtCam)->csno)+" "+coach_1.at(iExtCam)->car_type_desc+"_"+coach_1.at(iExtCam)->camera_type_desc);
        ui->label_frame_2->setText("COACH - "+QString::number(coach_1.at(iExtCam+1)->csno)+" "+coach_1.at(iExtCam+1)->car_type_desc+"_"+coach_1.at(iExtCam+1)->camera_type_desc);
        ui->label_frame_3->setText("COACH - "+QString::number(coach_1.at(iExtCam+2)->csno)+" "+coach_1.at(iExtCam+2)->car_type_desc+"_"+coach_1.at(iExtCam+2)->camera_type_desc);
    }

    if(iSaloonCam==0){
        ui->label_frame_4->setText("COACH - "+QString::number(coach_1.at(iSaloonCam+4)->csno)+" "+coach_1.at(iSaloonCam+4)->car_type_desc+"_"+coach_1.at(iSaloonCam+4)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iSaloonCam+4)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_1.at(iSaloonCam+5)->csno)+" "+coach_1.at(iSaloonCam+5)->car_type_desc+"_"+coach_1.at(iSaloonCam+5)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iSaloonCam+5)->sno));
    }

    QTimer* label_timer = new QTimer(this);
    label_timer->setInterval(10000); // 10 seconds

    connect(label_timer, SIGNAL(timeout()),this,SLOT(self_ping_lan_icon()));
    connect(label_timer, SIGNAL(timeout()),this,SLOT(ping_voip_icon()));
    connect(label_timer, SIGNAL(timeout()),this,SLOT(wifi_status()));
    connect(label_timer, SIGNAL(timeout()),this,SLOT(ppp_status()));

    label_timer->start();

    QTimer* ping_cam_timer = new QTimer(this);
    ping_cam_timer->setInterval(600000); // 10 mins

    connect(ping_cam_timer, SIGNAL(timeout()),this,SLOT(iterateCamLogs()));

    ping_cam_timer->start(600000);

}


MainWindow::~MainWindow()
{
    delete ui;

    /* Stop playing */
    stop_all_cam();
    libvlc_media_player_stop (_mp10);
    libvlc_media_player_stop (_mp11);


    /* Free the media_player */
    libvlc_media_player_release (_mp);
    libvlc_media_player_release (_mp2);
    libvlc_media_player_release (_mp3);
    libvlc_media_player_release (_mp4);
    libvlc_media_player_release (_mp5);
    libvlc_media_player_release (_mp6);
    libvlc_media_player_release (_mp7);
    libvlc_media_player_release (_mp8);
    libvlc_media_player_release (_mp9);
    libvlc_media_player_release (_mp10);
    libvlc_media_player_release (_mp11);

    libvlc_release (_vlcinstance);
}

//=======================================================================
//Getting real time Date and Time
void MainWindow::statusDateTime()
{
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString time_text = time.toString("hh : mm : ss");
    QString date_text = date.toString("dd-MM-yyyy");
    ui->label_Time->setText(time_text);
    ui->label_Date->setText(date_text);

    time_text_recording = time.toString("hh.mm.ss");
    date_text_recording = date.toString("yyyy.MM.dd");

    //    statusCnt++;

    //    if(statusCnt==10){
    //        statusCnt = 0;
    //        slave_ping_update(papis_slaves);
    //    }
}
//=======================================================================

//TCP server connection

void MainWindow::etb_connected()
{
    etbLocalConnection = etbListner->nextPendingConnection();
    connect(etbLocalConnection, &QAbstractSocket::disconnected,
            etbLocalConnection, &QObject::deleteLater);
    connect(etbLocalConnection, SIGNAL(readyRead()), this, SLOT(etb_ready_read()));
}

void MainWindow::openetbcallwindow()
{
    etbWindow->setWindowFlag(Qt::FramelessWindowHint);
    etbWindow->showFullScreen();
    timeractive.elapsed();
}

//Function for opening the driver login dialog
void MainWindow::openetbcalldialog()
{
    etbCallPrompt *etbCallDialog = new etbCallPrompt(this);
    etbCallDialog->setModal(true);
    etbCallDialog->setWindowFlag(Qt::FramelessWindowHint);
    //continuing recording after ETB call terminates
    QObject::connect(etbCallDialog, SIGNAL(yesbuttonPressed()), this, SLOT(contRecording()));
    QObject::connect(etbCallDialog, SIGNAL(renameAudio()), this, SLOT(renameEtbAudio()));
    etbCallDialog->show();
}

void MainWindow::etb_ready_read()
{
    QByteArray block = etbLocalConnection->readAll();

    block.resize(16);

    if(block.contains("ST:")) //Starting of Call to ETB
    {
        QString msg = "192.168.1.223";
        etbLocalConnection->write(msg.toLocal8Bit());
        qDebug() << msg.toLocal8Bit();

        /*Stop Stream on Different View*/
        stop_view_1_cam();
        stop_view_Mosiac_cam();
        libvlc_media_player_stop (_mp10);

        //Shifting to CCTV View when ETB call starts

        openetbcallwindow();

        //        ui->stackedWidget_Dynamic->setCurrentIndex(5);

        //        //Starting respective CCTV feed (Hardcoded for now)
        //        const char* url_cctv =  "rtsp://192.168.1.221/video1.sdp";

        //        _m11 = libvlc_media_new_location(_vlcinstance, url_cctv);
        //        libvlc_media_player_set_media (_mp11, _m11);

        //        int windid11 = ui->frame_11->winId();
        //        libvlc_media_player_set_xwindow (_mp11, windid11);

        //        libvlc_media_player_play (_mp11);
        //        _isPlaying=true;

        recordedFileNameEtb = date_text_recording + "_" +time_text_recording;

        QString forRecordingStream = "gst-launch-1.0 -ev  rtspsrc location=rtsp://192.168.1.223/video1.sdp ! application/x-rtp, media=video, encoding-name=H264 ! queue ! rtph264depay "
                                     "! h264parse ! matroskamux ! filesink location="+pathToEtbArchives+date_text_recording+"_recordings/"+recordedFileNameEtb+"_etb_video.mp4 &";

        system(qPrintable(forRecordingStream));

        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", ETBU CALL STARTED\n";
            file.close();}
    }

    else if(block.contains("EN:")) //End of Call to ETB
    {
        //open etb prompt dialog
        openetbcalldialog();

        //Closing etb call window
        etbWindow->close();

        system("ps -A | grep gst | awk '{ printf $1 }' | xargs kill -2 $1");

        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", ETBU CALL ENDED\n";
            file.close();}

        //        libvlc_media_player_play (_mp);
        //        _isPlaying=true;
        //        libvlc_media_player_play (_mp2);
        //        _isPlaying=true;
        //        libvlc_media_player_play (_mp3);
        //        _isPlaying=true;
        //        libvlc_media_player_play (_mp4);
        //        _isPlaying=true;
        //        libvlc_media_player_play (_mp5);
        //        _isPlaying=true;

    }
}

//When train stops (Reverse)
void MainWindow::train_stops_connected()
{
    trainStopConnection = trainStopLister->nextPendingConnection();
    connect(trainStopConnection, &QAbstractSocket::disconnected,
            trainStopConnection, &QObject::deleteLater);
    connect(trainStopConnection, SIGNAL(readyRead()), this, SLOT(train_stops_readyRead()));
}

void MainWindow::train_stops_readyRead()
{
    QByteArray block = trainStopConnection->readAll();
    if(block.contains("RE:")) //Train Starts going in Reverse
    {
        QString msg = "192.168.1.224";
        trainStopConnection->write(msg.toLocal8Bit());
        qDebug() << msg.toLocal8Bit();

        /*Stop Stream on Different View*/
        stop_view_1_cam();
        stop_view_Mosiac_cam();
        libvlc_media_player_stop (_mp10);

        //Shifting to CCTV View when ETB call starts
        ui->stackedWidget_Dynamic->setCurrentIndex(7);

        //Starting feed of reverse view IPCAM (Hardcoded for now)
        const char* url_reverse1 =  "rtsp://192.168.1.224/video1.sdp";
        const char* url_reverse2 =  "rtsp://192.168.1.233/video1.sdp";
        const char* url_reverse3 =  "rtsp://192.168.1.234/video1.sdp";
        const char* url_reverse4 =  "rtsp://192.168.1.235/video1.sdp";

        _m13 = libvlc_media_new_location(_vlcinstance, url_reverse1);
        libvlc_media_player_set_media (_mp13, _m13);

        int windid13 = ui->frame_13->winId();
        libvlc_media_player_set_xwindow (_mp13, windid13);

        libvlc_media_player_play (_mp13);
        _isPlaying=true;

        _m14 = libvlc_media_new_location(_vlcinstance, url_reverse2);
        libvlc_media_player_set_media (_mp14, _m14);

        int windid14 = ui->frame_14->winId();
        libvlc_media_player_set_xwindow (_mp14, windid14);

        libvlc_media_player_play (_mp14);
        _isPlaying=true;

        _m15 = libvlc_media_new_location(_vlcinstance, url_reverse3);
        libvlc_media_player_set_media (_mp15, _m15);

        int windid15 = ui->frame_15->winId();
        libvlc_media_player_set_xwindow (_mp15, windid15);

        libvlc_media_player_play (_mp15);
        _isPlaying=true;

        _m16 = libvlc_media_new_location(_vlcinstance, url_reverse4);
        libvlc_media_player_set_media (_mp16, _m16);

        int windid16 = ui->frame_16->winId();
        libvlc_media_player_set_xwindow (_mp16, windid16);

        libvlc_media_player_play (_mp16);
        _isPlaying=true;
    }

    else if(block.contains("EN:")) //Ending reverse and sending HMI to default screen
    {
        //Stops reverse IPCAM feed
        libvlc_media_player_stop (_mp13);
        //Stops reverse IPCAM feed
        libvlc_media_player_stop (_mp14);
        //Stops reverse IPCAM feed
        libvlc_media_player_stop (_mp15);
        //Stops reverse IPCAM feed
        libvlc_media_player_stop (_mp16);

        //Returns to Default Screen
        opendefaultScreen();

        //        libvlc_media_player_play (_mp);
        //        _isPlaying=true;
        //        libvlc_media_player_play (_mp2);
        //        _isPlaying=true;
        //        libvlc_media_player_play (_mp3);
        //        _isPlaying=true;
        //        libvlc_media_player_play (_mp4);
        //        _isPlaying=true;
        //        libvlc_media_player_play (_mp5);
        //        _isPlaying=true;
    }


}

//Functions for downloading logs from NVR

void MainWindow::downloadLogs()
{
    managerLogs = new QNetworkAccessManager(this);

    connect(managerLogs,
            &QNetworkAccessManager::finished,
            this,
            &MainWindow::replyNVR);

    managerLogs->get(QNetworkRequest(QUrl("http://admin:admin@192.168.1.2/webs.cgi?PAGE=SystemLogForm")));
}

void MainWindow::replyNVR (QNetworkReply *replyStream)
{
    if(replyStream->error())
    {
        qDebug() << "ERROR!";
        qDebug() << replyStream->errorString();
    }
    else
    {
        qDebug() << replyStream->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug() << replyStream->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();
        qDebug() << replyStream->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        qDebug() << replyStream->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << replyStream->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();


        QDate date = QDate::currentDate();
        QTime time = QTime::currentTime();
        QString date_string ="";
        QString time_string ="";
        date_string = date.toString("yyyy.MM.dd");
        time_string = time.toString("hh.mm.ss");

        //        QString filename = date_string + time_string;

        QFile *file = new QFile(pathToLogs+date_string+"/"+date_string+"_logs");
        if(file->open(QFile::Append))
        {
            file->write(replyStream->readAll());
            file->flush();
            file->close();
        }
        delete file;
    }
    replyStream->deleteLater();
}

void MainWindow::slave_ping_update(QList <slave*> slaves)
{
    papis_slaves = slaves;

    hmi_ipAdd = slaves.at(2)->ip_addr;
    hmi_ipAdd_2 = slaves.at(3)->ip_addr;
    server_ipAdd = slaves.at(8)->ip_addr;
    server_ipAdd_2 = slaves.at(9)->ip_addr;


    //    QList <slave*> devices;

    //    for (int i = 0 ; i < slaves.size() ; i++){
    //        if(slaves.at(2)->activeStatus == ACTIVE){
    //            ui->label_lan->setStyleSheet("image: url(:/new/icons/lan_green.png);");
    //        }

    //        if(slaves.at(8)->activeStatus == ACTIVE){
    //            ui->label_voip->setStyleSheet("image: url(:/new/icons/voip_green.png);");
    //        }

    //        else{
    //            ui->label_lan->setStyleSheet("image: url(:/new/icons/lan_red.png);");
    //            ui->label_voip->setStyleSheet("image: url(:/new/icons/voip_red.png);");
    //        }
    //    }
}

void MainWindow::camera_ping_update(/*int pc,*/QList <camera*> cameras)
{
    hmi_slaves = cameras;

    QList <camera*> list;

    //Creating a list called rake, which is a list of lists . The inner list contains information of each IPCAM coach wise.
    for (int i = 0; i < cameras.size(); i ++){
        if(cameras.at(i)->csno == 1){
            list.append(cameras.at(i));
        }
    }
    rake.append(list);
    list.clear();

    for (int i = 0; i < cameras.size(); i ++){
        if(cameras.at(i)->csno == 2){
            list.append(cameras.at(i));
        }
    }
    rake.append(list);
    list.clear();

    for (int i = 0; i < cameras.size(); i ++){
        if(cameras.at(i)->csno == 3){
            list.append(cameras.at(i));
        }
    }
    rake.append(list);
    list.clear();

    for (int i = 0; i < cameras.size(); i ++){
        if(cameras.at(i)->csno == 4){
            list.append(cameras.at(i));
        }
    }
    rake.append(list);
    list.clear();

    for (int i = 0; i < cameras.size(); i ++){
        if(cameras.at(i)->csno == 5){
            list.append(cameras.at(i));
        }
    }
    rake.append(list);
    list.clear();

    for (int i = 0; i < cameras.size(); i ++){
        if(cameras.at(i)->csno == 6){
            list.append(cameras.at(i));
        }
    }
    rake.append(list);

    list.clear();

    for (int i = 0; i < cameras.size(); i ++){
        if(cameras.at(i)->csno == 7){
            list.append(cameras.at(i));
        }
    }
    rake.append(list);
    list.clear();

    for (int i = 0; i < cameras.size(); i ++){
        if(cameras.at(i)->csno == 8){
            list.append(cameras.at(i));
        }
    }
    rake.append(list);
    list.clear();

    for (int i = 0; i < cameras.size(); i ++){
        if(cameras.at(i)->csno == 9){
            list.append(cameras.at(i));
        }
    }
    rake.append(list);
    list.clear();

    for (int i = 0; i < cameras.size(); i ++){
        if(cameras.at(i)->csno == 10){
            list.append(cameras.at(i));
        }
    }
    rake.append(list);
    list.clear();

    for (int i = 0; i < cameras.size(); i ++){
        if(cameras.at(i)->csno == 11){
            list.append(cameras.at(i));
        }
    }
    rake.append(list);
    list.clear();

    for (int i = 0; i < cameras.size(); i ++){
        if(cameras.at(i)->csno == 12){
            list.append(cameras.at(i));
        }
    }
    rake.append(list);

    //populating coach wise lists
    coach_1 = rake[0];
    coach_2 = rake[1];
    coach_3 = rake[2];
    coach_4 = rake[3];
    coach_5 = rake[4];
    coach_6 = rake[5];
    coach_7 = rake[6];
    coach_8 = rake[7];
    coach_9 = rake[8];
    coach_10 = rake[9];
    coach_11 = rake[10];
    coach_12 = rake[11];

    /*Creating List for all Cameras*/
    //Adding cameras in coach 1 to all camera list
    for(int i = 0 ; i < coach_1.size() ; i ++){
        QString url = "rtsp://"+coach_1.at(i)->ip_addr+"/video1.sdp";
        listAllCams.append(url);
    }

    //Adding cameras in coach 2 to all camera list
    for(int i = 0 ; i < coach_2.size() ; i ++){
        QString url = "rtsp://"+coach_2.at(i)->ip_addr+"/video1.sdp";
        listAllCams.append(url);
    }

    //Adding cameras in coach 3 to all camera list
    for(int i = 0 ; i < coach_3.size() ; i ++){
        QString url = "rtsp://"+coach_3.at(i)->ip_addr+"/video1.sdp";
        listAllCams.append(url);
    }

    //Adding cameras in coach 4 to all camera list
    for(int i = 0 ; i < coach_4.size() ; i ++){
        QString url = "rtsp://"+coach_4.at(i)->ip_addr+"/video1.sdp";
        listAllCams.append(url);
    }
    //Adding cameras in coach 5 to all camera list
    for(int i = 0 ; i < coach_5.size() ; i ++){
        QString url = "rtsp://"+coach_5.at(i)->ip_addr+"/video1.sdp";
        listAllCams.append(url);
    }

    //Adding cameras in coach 6 to all camera list
    for(int i = 0 ; i < coach_6.size() ; i ++){
        QString url = "rtsp://"+coach_6.at(i)->ip_addr+"/video1.sdp";
        listAllCams.append(url);
    }

    //Adding cameras in coach 7 to all camera list
    for(int i = 0 ; i < coach_7.size() ; i ++){
        QString url = "rtsp://"+coach_7.at(i)->ip_addr+"/video1.sdp";
        listAllCams.append(url);
    }
    //Adding cameras in coach 8 to all camera list
    for(int i = 0 ; i < coach_8.size() ; i ++){
        QString url = "rtsp://"+coach_8.at(i)->ip_addr+"/video1.sdp";
        listAllCams.append(url);
    }
    //Adding cameras in coach 9 to all camera list
    for(int i = 0 ; i < coach_9.size() ; i ++){
        QString url = "rtsp://"+coach_9.at(i)->ip_addr+"/video1.sdp";
        listAllCams.append(url);
    }
    //Adding cameras in coach 10 to all camera list
    for(int i = 0 ; i < coach_10.size() ; i ++){
        QString url = "rtsp://"+coach_10.at(i)->ip_addr+"/video1.sdp";
        listAllCams.append(url);
    }
    //Adding cameras in coach 11 to all camera list
    for(int i = 0 ; i < coach_11.size() ; i ++){
        QString url = "rtsp://"+coach_11.at(i)->ip_addr+"/video1.sdp";
        listAllCams.append(url);
    }
    //Adding cameras in coach 12 to all camera list
    for(int i = 0 ; i < coach_12.size() ; i ++){
        QString url = "rtsp://"+coach_12.at(i)->ip_addr+"/video1.sdp";
        listAllCams.append(url);
    }

    /*Creating List for External Cameras*/
    //Adding Front CAM, Front Cab CAM, Front Left CAM and Front Right CAM
    for(int i = 0 ; i < 4 ; i ++){
        QString url = "rtsp://"+coach_1.at(i)->ip_addr+"/video1.sdp";
        listExternalCams.append(url);
    }
    //Adding Rear CAM, Rear Cab CAM, Rear Left CAM and Rear Right CAM
    for(int i = 0 ; i < 4 ; i ++){
        QString url = "rtsp://"+coach_12.at(i)->ip_addr+"/video1.sdp";
        listExternalCams.append(url);
    }

    /*Creating List for Saloon Cameras*/
    //Adding Saloon Cameras for Coach 1 to Saloon Cam List
    for(int i = 4 ; i < coach_1.size() ; i ++){
        QString url = "rtsp://"+coach_1.at(i)->ip_addr+"/video1.sdp";
        listSaloonCams.append(url);
    }
    //Adding Saloon Cameras for Coach 2 to Saloon Cam List
    for(int i = 0 ; i < 8 ; i ++){
        QString url = "rtsp://"+coach_2.at(i)->ip_addr+"/video1.sdp";
        listSaloonCams.append(url);
    }
    //Adding Saloon Cameras for Coach 3 to Saloon Cam List
    for(int i = 0 ; i < 8 ; i ++){
        QString url = "rtsp://"+coach_3.at(i)->ip_addr+"/video1.sdp";
        listSaloonCams.append(url);
    }
    //Adding Saloon Cameras for Coach 4 to Saloon Cam List
    for(int i = 0 ; i < 8 ; i ++){
        QString url = "rtsp://"+coach_4.at(i)->ip_addr+"/video1.sdp";
        listSaloonCams.append(url);
    }
    //Adding Saloon Cameras for Coach 5 to Saloon Cam List
    for(int i = 0 ; i < 8 ; i ++){
        QString url = "rtsp://"+coach_5.at(i)->ip_addr+"/video1.sdp";
        listSaloonCams.append(url);
    }
    //Adding Saloon Cameras for Coach 6 to Saloon Cam List
    for(int i = 0 ; i < 8 ; i ++){
        QString url = "rtsp://"+coach_6.at(i)->ip_addr+"/video1.sdp";
        listSaloonCams.append(url);
    }
    //Adding Saloon Cameras for Coach 7 to Saloon Cam List
    for(int i = 0 ; i < 8 ; i ++){
        QString url = "rtsp://"+coach_7.at(i)->ip_addr+"/video1.sdp";
        listSaloonCams.append(url);
    }
    //Adding Saloon Cameras for Coach 8 to Saloon Cam List
    for(int i = 0 ; i < 8 ; i ++){
        QString url = "rtsp://"+coach_8.at(i)->ip_addr+"/video1.sdp";
        listSaloonCams.append(url);
    }
    //Adding Saloon Cameras for Coach 9 to Saloon Cam List
    for(int i = 0 ; i < 8 ; i ++){
        QString url = "rtsp://"+coach_9.at(i)->ip_addr+"/video1.sdp";
        listSaloonCams.append(url);
    }
    //Adding Saloon Cameras for Coach 10 to Saloon Cam List
    for(int i = 0 ; i < 8 ; i ++){
        QString url = "rtsp://"+coach_10.at(i)->ip_addr+"/video1.sdp";
        listSaloonCams.append(url);
    }
    //Adding Saloon Cameras for Coach 11 to Saloon Cam List
    for(int i = 0 ; i < 8 ; i ++){
        QString url = "rtsp://"+coach_11.at(i)->ip_addr+"/video1.sdp";
        listSaloonCams.append(url);
    }
    //Adding Saloon Cameras for Coach 12 to Saloon Cam List
    for(int i = 4 ; i < coach_12.size() ; i ++){
        QString url = "rtsp://"+coach_12.at(i)->ip_addr+"/video1.sdp";
        listSaloonCams.append(url);
    }
}

//Function to update cam logs
void MainWindow::iterateCamLogs(){
    //Coach 1 IPCAM 1 - FRONT
    if(coach_1.at(0)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , FRONT CAM ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , FRONT CAM OFFLINE\n";
            file.close();}
    }
    //Coach 1 IPCAM 2 - FRONT RIGHT
    if(coach_1.at(1)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , FRONT RIGHT CAM ONLINE\n";
            file.close();}
    }
    else{
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , FRONT RIGHT CAM OFFLINE\n";
            file.close();}
    }
    //Coach 1 IPCAM 3 - FRONT LEFT
    if(coach_1.at(2)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , FRONT LEFT CAM ONLINE\n";
            file.close();}
    }
    else {
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , FRONT LEFT CAM OFFLINE\n";
            file.close();}
    }
    //Coach 1 IPCAM 4 - FRONT CAB
    if(coach_1.at(3)->activeStatus == ACTIVE){//log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , FRONT CAB CAM ONLINE\n";
            file.close();}
    }
    else{
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , FRONT CAB CAM OFFLINE\n";
            file.close();}
    }
    //Coach 1 IPCAM 5 - SALOON 1
    if(coach_1.at(4)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , SALOON CAM 1 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , SALOON CAM 1 OFFLINE\n";
            file.close();}
    }
    //Coach 1 IPCAM 6 - SALOON 2
    if(coach_1.at(5)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , SALOON CAM 2 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , SALOON CAM 2 OFFLINE\n";
            file.close();}
    }
    //Coach 1 IPCAM 7 - SALOON 3
    if(coach_1.at(6)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , SALOON CAM 3 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , SALOON CAM 3 OFFLINE\n";
            file.close();}
    }
    //Coach 1 IPCAM 8 - SALOON 4
    if(coach_1.at(7)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , SALOON CAM 4 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , SALOON CAM 4 OFFLINE\n";
            file.close();}
    }
    //Coach 1 IPCAM 9 - SALOON 5
    if(coach_1.at(8)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , SALOON CAM 5 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , SALOON CAM 5 OFFLINE\n";
            file.close();}
    }
    //Coach 1 IPCAM 10 - SALOON 6
    if(coach_1.at(9)->activeStatus == ACTIVE){
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , SALOON CAM 6 ONLINE\n";
            file.close();}
    }
    else{
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , SALOON CAM 6 OFFLINE\n";
            file.close();}
    }
    //Coach 1 IPCAM 11 - SALOON 7
    if(coach_1.at(10)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , SALOON CAM 7 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , SALOON CAM 7 OFFLINE\n";
            file.close();}
    }
    //Coach 1 IPCAM 12 - SALOON 8
    if(coach_1.at(11)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , SALOON CAM 8 ONLINE\n";
            file.close();}

    }
    else {
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 1 , SALOON CAM 8 OFFLINE\n";
            file.close();}
    }
    //COACH 2
    if(coach_2.at(0)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 2 , SALOON CAM 1 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 2 , SALOON CAM 1 OFFLINE\n";
            file.close();}
    }
    //COACH 2 IPCAM 2 - SALOON 2
    if(coach_2.at(1)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 2 , SALOON CAM 2 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 2 , SALOON CAM 2 OFFLINE\n";
            file.close();}
    }
    //COACH 2 IPCAM 3 - SALOON 3
    if(coach_2.at(2)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 2 , SALOON CAM 3 ONLINE\n";
            file.close();}
    }
    else {
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 2 , SALOON CAM 3 OFFLINE\n";
            file.close();}
    }
    //COACH 2 IPCAM 4 - SALOON 4
    if(coach_2.at(3)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 2 , SALOON CAM 4 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 2 , SALOON CAM 4 OFFLINE\n";
            file.close();}
    }
    //COACH 2 IPCAM 5 - SALOON 5
    if(coach_2.at(4)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 2 , SALOON CAM 5 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 2 , SALOON CAM 5 OFFLINE\n";
            file.close();}

    }
    //COACH 2 IPCAM 6 - SALOON 6
    if(coach_2.at(5)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 2 , SALOON CAM 6 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 2 , SALOON CAM 6 OFFLINE\n";
            file.close();}
    }
    //COACH 2 IPCAM 7 - SALOON 7
    if(coach_2.at(6)->activeStatus == ACTIVE){

        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 2 , SALOON CAM 7 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 2 , SALOON CAM 7 OFFLINE\n";
            file.close();}
    }
    //COACH 2 IPCAM 8 - SALOON 8
    if(coach_2.at(7)->activeStatus == ACTIVE){

        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 2 , SALOON CAM 8 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 2 , SALOON CAM 8 OFFLINE\n";
            file.close();}
    }
    //COACH 3
    //COACH 3 IPCAM 1 - SALOON 1
    if(coach_3.at(0)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 3 , SALOON CAM 1 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 3 , SALOON CAM 1 OFFLINE\n";
            file.close();}
    }
    //COACH 3 IPCAM 2 - SALOON 2
    if(coach_3.at(1)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 3 , SALOON CAM 2 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 3 , SALOON CAM 2 OFFLINE\n";
            file.close();}
    }

    //COACH 3 IPCAM 3 - SALOON 3
    if(coach_3.at(2)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 3 , SALOON CAM 3 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 3 , SALOON CAM 3 OFFLINE\n";
            file.close();}
    }

    //COACH 3 IPCAM 4 - SALOON 4
    if(coach_3.at(3)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 3 , SALOON CAM 4 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 3 , SALOON CAM 4 OFFLINE\n";
            file.close();}
    }

    //COACH 3 IPCAM 5 - SALOON 5
    if(coach_3.at(4)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 3 , SALOON CAM 5 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 3 , SALOON CAM 5 OFFLINE\n";
            file.close();}
    }

    //COACH 3 IPCAM 6 - SALOON 6
    if(coach_3.at(5)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 3 , SALOON CAM 6 ONLINE\n";
            file.close();}
    }
    else {
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 3 , SALOON CAM 6 OFFLINE\n";
            file.close();}
    }

    //COACH 3 IPCAM 7 - SALOON 7
    if(coach_3.at(6)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 3 , SALOON CAM 7 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 3 , SALOON CAM 7 OFFLINE\n";
            file.close();}
    }

    //COACH 3 IPCAM 8 - SALOON 8
    if(coach_3.at(7)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 3 , SALOON CAM 8 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 3 , SALOON CAM 8 OFFLINE\n";
            file.close();}
    }
    //COACH 4
    //COACH 4 IPCAM 1 - SALOON 1
    if(coach_4.at(0)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 4 , SALOON CAM 1 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 4 , SALOON CAM 1 OFFLINE\n";
            file.close();}
    }
    //COACH 4 IPCAM 2 - SALOON 2
    if(coach_4.at(1)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 4 , SALOON CAM 2 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 4 , SALOON CAM 2 ONFFLINE\n";
            file.close();}
    }

    //COACH 4 IPCAM 3 - SALOON 3
    if(coach_4.at(2)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 4 , SALOON CAM 3 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 4 , SALOON CAM 3 OFFLINE\n";
            file.close();}
    }

    //COACH 4 IPCAM 4 - SALOON 4
    if(coach_4.at(3)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 4 , SALOON CAM 4 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 4 , SALOON CAM 4 OFFLINE\n";
            file.close();}
    }

    //COACH 4 IPCAM 5 - SALOON 5
    if(coach_4.at(4)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 4 , SALOON CAM 5 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 4 , SALOON CAM 5 OFFLINE\n";
            file.close();}
    }

    //COACH 4 IPCAM 6 - SALOON 6
    if(coach_4.at(5)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 4 , SALOON CAM 6 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 4 , SALOON CAM 6 OFFLINE\n";
            file.close();}
    }

    //COACH 4 IPCAM 7 - SALOON 7
    if(coach_4.at(6)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 4 , SALOON CAM 7 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 4 , SALOON CAM 7 OFFLINE\n";
            file.close();}
    }

    //COACH 4 IPCAM 8 - SALOON 8
    if(coach_4.at(7)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 4 , SALOON CAM 8 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 4 , SALOON CAM 8 OFFLINE\n";
            file.close();}
    }
    //COACH 5
    //COACH 5 IPCAM 1 - SALOON 1
    if(coach_5.at(0)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 5 , SALOON CAM 1 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 5 , SALOON CAM 1 OFFLINE\n";
            file.close();}
    }
    //COACH 5 IPCAM 2 - SALOON 2
    if(coach_5.at(1)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 5 , SALOON CAM 2 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 5 , SALOON CAM 2 OFFLINE\n";
            file.close();}
    }

    //COACH 5 IPCAM 3 - SALOON 3
    if(coach_5.at(2)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 5 , SALOON CAM 3 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 5 , SALOON CAM 3 OFFLINE\n";
            file.close();}
    }

    //COACH 5 IPCAM 4 - SALOON 4
    if(coach_5.at(3)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 5 , SALOON CAM 4 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 5 , SALOON CAM 4 OFFLINE\n";
            file.close();}
    }

    //COACH 5 IPCAM 5 - SALOON 5
    if(coach_5.at(4)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 5 , SALOON CAM 5 ONLINE\n";
            file.close();}
    }
    else
    {
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 5 , SALOON CAM 5 OFFLINE\n";
            file.close();}
    }

    //COACH 5 IPCAM 6 - SALOON 6
    if(coach_5.at(5)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 5 , SALOON CAM 6 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 5 , SALOON CAM 6 OFFLINE\n";
            file.close();}
    }

    //COACH 5 IPCAM 7 - SALOON 7
    if(coach_5.at(6)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 5 , SALOON CAM 7 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 5 , SALOON CAM 7 OFFLINE\n";
            file.close();}
    }

    //COACH 5 IPCAM 8 - SALOON 8
    if(coach_5.at(7)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 5 , SALOON CAM 8 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 5 , SALOON CAM 8 OFFLINE\n";
            file.close();}
    }
    //COACH 6
    //COACH 6 IPCAM 1 - SALOON 1
    if(coach_6.at(0)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 6 , SALOON CAM 1 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 6 , SALOON CAM 1 OFFLINE\n";
            file.close();}
    }
    //COACH 6 IPCAM 2 - SALOON 2
    if(coach_6.at(1)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 6 , SALOON CAM 2 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 6 , SALOON CAM 2 OFFLINE\n";
            file.close();}
    }

    //COACH 6 IPCAM 3 - SALOON 3
    if(coach_6.at(2)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 6 , SALOON CAM 3 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 6 , SALOON CAM 3 OFFLINE\n";
            file.close();}
    }

    //COACH 6 IPCAM 4 - SALOON 4
    if(coach_6.at(3)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 6 , SALOON CAM 4 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 6 , SALOON CAM 4 OFFLINE\n";
            file.close();}
    }

    //COACH 6 IPCAM 5 - SALOON 5
    if(coach_6.at(4)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 6 , SALOON CAM 5 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 6 , SALOON CAM 5 OFFLINE\n";
            file.close();}
    }

    //COACH 6 IPCAM 6 - SALOON 6
    if(coach_6.at(5)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 6 , SALOON CAM 6 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 6 , SALOON CAM 6 OFFLINE\n";
            file.close();}
    }

    //COACH 6 IPCAM 7 - SALOON 7
    if(coach_6.at(6)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 6 , SALOON CAM 7 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 6 , SALOON CAM 7 OFFLINE\n";
            file.close();}
    }

    //COACH 6 IPCAM 8 - SALOON 8
    if(coach_6.at(7)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 6 , SALOON CAM 8 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 6 , SALOON CAM 8 OFFLINE\n";
            file.close();}
    }
    //COACH 7
    //COACH 7 IPCAM 1 - SALOON 1
    if(coach_7.at(0)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 7 , SALOON CAM 1 ONLINE\n";
            file.close();}

    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 7 , SALOON CAM 1 OFFLINE\n";
            file.close();}
    }
    //COACH 7 IPCAM 2 - SALOON 2
    if(coach_7.at(1)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 7 , SALOON CAM 2 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 7 , SALOON CAM 2 OFFLINE\n";
            file.close();}
    }

    //COACH 7 IPCAM 3 - SALOON 3
    if(coach_7.at(2)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 7 , SALOON CAM 3 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 7 , SALOON CAM 2 OFFLINE\n";
            file.close();}
    }

    //COACH 7 IPCAM 4 - SALOON 4
    if(coach_7.at(3)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 7 , SALOON CAM 4 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 7 , SALOON CAM 4 OFFLINE\n";
            file.close();}
    }

    //COACH 7 IPCAM 5 - SALOON 5
    if(coach_7.at(4)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 7 , SALOON CAM 5 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 7 , SALOON CAM 5 OFFLINE\n";
            file.close();}
    }

    //COACH 7 IPCAM 6 - SALOON 6
    if(coach_7.at(5)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 7 , SALOON CAM 6 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 7 , SALOON CAM 6 OFFLINE\n";
            file.close();}
    }

    //COACH 7 IPCAM 7 - SALOON 7
    if(coach_7.at(6)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 7 , SALOON CAM 7 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 7 , SALOON CAM 7 OFFLINE\n";
            file.close();}
    }

    //COACH 7 IPCAM 8 - SALOON 8
    if(coach_7.at(7)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 7 , SALOON CAM 8 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 7 , SALOON CAM 8 OFFLINE\n";
            file.close();}
    }

    //COACH 8
    //COACH 8 IPCAM 1 - SALOON 1
    if(coach_8.at(0)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 8 , SALOON CAM 1 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 8 , SALOON CAM 1 OFFLINE\n";
            file.close();}
    }
    //COACH 8 IPCAM 2 - SALOON 2
    if(coach_8.at(1)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 8 , SALOON CAM 2 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 8 , SALOON CAM 2 OFFLINE\n";
            file.close();}
    }

    //COACH 8 IPCAM 3 - SALOON 3
    if(coach_8.at(2)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 8 , SALOON CAM 3 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 8 , SALOON CAM 3 OFFLINE\n";
            file.close();}
    }

    //COACH 8 IPCAM 4 - SALOON 4
    if(coach_8.at(3)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 8 , SALOON CAM 4 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 8 , SALOON CAM 4 OFFLINE\n";
            file.close();}
    }

    //COACH 8 IPCAM 5 - SALOON 5
    if(coach_8.at(4)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 8 , SALOON CAM 5 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 8 , SALOON CAM 5 OFFLINE\n";
            file.close();}
    }

    //COACH 8 IPCAM 6 - SALOON 6
    if(coach_8.at(5)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 8 , SALOON CAM 6 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 8 , SALOON CAM 6 OFFLINE\n";
            file.close();}
    }

    //COACH 8 IPCAM 7 - SALOON 7
    if(coach_8.at(6)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 8 , SALOON CAM 7 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 8 , SALOON CAM 7 OFFLINE\n";
            file.close();}
    }

    //COACH 8 IPCAM 8 - SALOON 8
    if(coach_8.at(7)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 8 , SALOON CAM 8 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 8 , SALOON CAM 8 OFFLINE\n";
            file.close();}
    }
    //COACH 9
    //COACH 9 IPCAM 1 - SALOON 1
    if(coach_9.at(0)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 9 , SALOON CAM 1 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 9 , SALOON CAM 1 OFFLINE\n";
            file.close();}
    }
    //COACH 9 IPCAM 2 - SALOON 2
    if(coach_9.at(1)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 9 , SALOON CAM 2 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 9 , SALOON CAM 2 OFFLINE\n";
            file.close();}
    }

    //COACH 9 IPCAM 3 - SALOON 3
    if(coach_9.at(2)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 9 , SALOON CAM 3 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 9 , SALOON CAM 3 OFFLINE\n";
            file.close();}
    }

    //COACH 9 IPCAM 4 - SALOON 4
    if(coach_9.at(3)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 9 , SALOON CAM 4 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 9 , SALOON CAM 4 OFFLINE\n";
            file.close();}
    }

    //COACH 9 IPCAM 5 - SALOON 5
    if(coach_9.at(4)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 9 , SALOON CAM 5 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 9 , SALOON CAM 5 OFFLINE\n";
            file.close();}
    }

    //COACH 9 IPCAM 6 - SALOON 6
    if(coach_9.at(5)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 9 , SALOON CAM 6 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 9 , SALOON CAM 6 OFFLINE\n";
            file.close();}
    }

    //COACH 9 IPCAM 7 - SALOON 7
    if(coach_9.at(6)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 9 , SALOON CAM 7 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 9 , SALOON CAM 7 OFFLINE\n";
            file.close();}
    }

    //COACH 9 IPCAM 8 - SALOON 8
    if(coach_9.at(7)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 9 , SALOON CAM 8 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 9 , SALOON CAM 8 OFFLINE\n";
            file.close();}
    }
    //COACH 10
    //COACH 10 IPCAM 1 - SALOON 1
    if(coach_10.at(0)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 10 , SALOON CAM 1 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 10 , SALOON CAM 1 OFFLINE\n";
            file.close();}
    }
    //COACH 10 IPCAM 2 - SALOON 2
    if(coach_10.at(1)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 10 , SALOON CAM 2 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 10 , SALOON CAM 2 OFFLINE\n";
            file.close();}
    }
    //COACH 10 IPCAM 3 - SALOON 3
    if(coach_10.at(2)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 10 , SALOON CAM 3 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 10 , SALOON CAM 3 OFFLINE\n";
            file.close();}
    }
    //COACH 10 IPCAM 4 - SALOON 4
    if(coach_10.at(3)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 10 , SALOON CAM 4 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 10 , SALOON CAM 4 OFFLINE\n";
            file.close();}
    }
    //COACH 10 IPCAM 5 - SALOON 5
    if(coach_10.at(4)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 10 , SALOON CAM 5 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 10 , SALOON CAM 5 OFFLINE\n";
            file.close();}
    }
    //COACH 10 IPCAM 6 - SALOON 6
    if(coach_10.at(5)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 10 , SALOON CAM 6 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 10 , SALOON CAM 6 OFFLINE\n";
            file.close();}
    }
    //COACH 10 IPCAM 7 - SALOON 7
    if(coach_10.at(6)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 10 , SALOON CAM 7 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 10 , SALOON CAM 7 OFFLINE\n";
            file.close();}
    }
    //COACH 10 IPCAM 8 - SALOON 8
    if(coach_10.at(7)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 10 , SALOON CAM 8 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 10 , SALOON CAM 8 OFFLINE\n";
            file.close();}
    }
    //COACH 11
    //COACH 11 IPCAM 1 - SALOON 1
    if(coach_11.at(0)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 11 , SALOON CAM 1 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 11 , SALOON CAM 1 OFFLINE\n";
            file.close();}
    }
    //COACH 11 IPCAM 2 - SALOON 2
    if(coach_11.at(1)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 11 , SALOON CAM 2 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 11 , SALOON CAM 2 OFFLINE\n";
            file.close();}
    }
    //COACH 11 IPCAM 3 - SALOON 3
    if(coach_11.at(2)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 11 , SALOON CAM 3 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 11 , SALOON CAM 3 OFFLINE\n";
            file.close();}
    }
    //COACH 11 IPCAM 4 - SALOON 4
    if(coach_11.at(3)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 11 , SALOON CAM 4 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 11 , SALOON CAM 4 OFFLINE\n";
            file.close();}
    }
    //COACH 11 IPCAM 5 - SALOON 5
    if(coach_11.at(4)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 11 , SALOON CAM 5 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 11 , SALOON CAM 5 OFFLINE\n";
            file.close();}
    }
    //COACH 11 IPCAM 6 - SALOON 6
    if(coach_11.at(5)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 11 , SALOON CAM 6 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 11 , SALOON CAM 6 OFFLINE\n";
            file.close();}
    }
    //COACH 11 IPCAM 7 - SALOON 7
    if(coach_11.at(6)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 11 , SALOON CAM 7 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 11 , SALOON CAM 7 OFFLINE\n";
            file.close();}
    }
    //COACH 11 IPCAM 8 - SALOON 8
    if(coach_11.at(7)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 11 , SALOON CAM 8 ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 11 , SALOON CAM 8 OFFLINE\n";
            file.close();}
    }
    //COACH 12
    //Coach 12 IPCAM 1 - REAR
    if(coach_12.at(0)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , REAR CAM ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , REAR CAM OFFLINE\n";
            file.close();}
    }
    //Coach 12 IPCAM 2 - REAR RIGHT
    if(coach_12.at(1)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , REAR RIGHT CAM ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , REAR RIGHT CAM OFFLINE\n";
            file.close();}
    }
    //Coach 12 IPCAM 3 - REAR LEFT
    if(coach_12.at(2)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , REAR LEFT CAM ONLINE\n";
            file.close();}

    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , REAR LEFT CAM OFFLINE\n";
            file.close();}
    }
    //Coach 12 IPCAM 4 - REAR CAB
    if(coach_12.at(3)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , REAR CAB CAM ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , REAR CAB CAM OFFLINE\n";
            file.close();}
    }
    //Coach 12 IPCAM 5 - SALOON 1
    if(coach_12.at(4)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , SALOON 1 CAM ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , SALOON 1 CAM OFFLINE\n";
            file.close();}
    }
    //Coach 12 IPCAM 6 - SALOON 2
    if(coach_12.at(5)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , SALOON 2 CAM ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , SALOON 2 CAM OFFLINE\n";
            file.close();}
    }
    //Coach 12 IPCAM 7 - SALOON 3
    if(coach_12.at(6)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , SALOON 3 CAM ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , SALOON 3 CAM OFFLINE\n";
            file.close();}
    }
    //Coach 12 IPCAM 8 - SALOON 4
    if(coach_12.at(7)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , SALOON 4 CAM ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , SALOON 4 CAM OFFLINE\n";
            file.close();}
    }
    //Coach 1 IPCAM 9 - SALOON 5
    if(coach_12.at(8)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , SALOON 5 CAM ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , SALOON 5 CAM OFFLINE\n";
            file.close();}
    }
    //Coach 1 IPCAM 12 - SALOON 6
    if(coach_12.at(9)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , SALOON 6 CAM ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , SALOON 6 CAM OFFLINE\n";
            file.close();}
    }
    //Coach 1 IPCAM 12 - SALOON 7
    if(coach_12.at(10)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , SALOON 7 CAM ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , SALOON 7 CAM OFFLINE\n";
            file.close();}
    }
    //Coach 1 IPCAM 12 - SALOON 8
    if(coach_12.at(11)->activeStatus == ACTIVE){
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , SALOON 8 CAM ONLINE\n";
            file.close();}
    }
    else{
        //log
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream ts(&file);
            ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", COACH 12 , SALOON 8 CAM OFFLINE\n";
            file.close();}
    }
}

//function to check LAN connectivity of HMI
void MainWindow::self_ping_lan_icon(){
    QProcess pingProcess;
    pingProcess.start("ping", QStringList() << "-c 1" << hmi_ipAdd);
    pingProcess.waitForFinished();
    QString output = pingProcess.readAllStandardOutput();
    bool isReachable = output.contains("1 received");
    if (isReachable) {
        ui->label_lan->setStyleSheet("image: url(:/new/icons/lan_green.png);border-style: solid;border-width:1px;border-color: rgb(186, 189, 182);");
    } else {
        ui->label_lan->setStyleSheet("image: url(:/new/icons/lan_red.png);border-style: solid;border-width:1px;border-color: rgb(186, 189, 182);");
    }
}

//function to check LAN connectivity of SIPSERVER
void MainWindow::ping_voip_icon(){
    QProcess pingProcess;
    pingProcess.start("ping", QStringList() << "-c 1" << server_ipAdd);
    pingProcess.waitForFinished();
    QString output = pingProcess.readAllStandardOutput();
    bool isReachable = output.contains("1 received");
    if (isReachable) {
        ui->label_voip->setStyleSheet("image: url(:/new/icons/voip_green.png);border-style: solid;border-width:1px;border-color: rgb(186, 189, 182);");
    } else {
        ui->label_voip->setStyleSheet("image: url(:/new/icons/voip_red.png);border-style: solid;border-width:1px;border-color: rgb(186, 189, 182);");
    }
}

//function to check wifi connectivity of HMI
void MainWindow::wifi_status(){
    QString interfaceName = "wwp0s20u2i4";
    //    QString interfaceName = "wlxd03745f1e12a";
    //    QString interfaceName = "wlp2s0";
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    bool isWlan0UpAndActive = false;

    foreach (const QNetworkInterface &interface, interfaces) {
        if (interface.name() == interfaceName) {
            if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
                    interface.flags().testFlag(QNetworkInterface::IsRunning)) {
                isWlan0UpAndActive = true;
                break;
            }
        }
    }
    if (isWlan0UpAndActive) {
        ui->label_wifi->setStyleSheet("image: url(:/new/icons/wifi-signal_green.png);border-style: solid;border-width:1px;border-color: rgb(186, 189, 182);");
    } else {
        ui->label_wifi->setStyleSheet("image: url(:/new/icons/wifi-signal_red.png);border-style: solid;border-width:1px;border-color: rgb(186, 189, 182);");
    }
}

//function to check 4G connectivity of HMI
void MainWindow::ppp_status(){
    QString interfaceName = "ppp0";
    //    QString interfaceName = "wlxd03745f1e12a";
    //    QString interfaceName = "wlp2s0";
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    bool isWlan0UpAndActive = false;

    foreach (const QNetworkInterface &interface, interfaces) {
        if (interface.name() == interfaceName) {
            if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
                    interface.flags().testFlag(QNetworkInterface::IsRunning)) {
                isWlan0UpAndActive = true;
                break;
            }
        }
    }
    if (isWlan0UpAndActive) {
        ui->label_4g->setStyleSheet("image: url(:/new/icons/4g_green.png);border-style: solid;border-width:1px;border-color: rgb(186, 189, 182);");
    } else {
        ui->label_4g->setStyleSheet("image: url(:/new/icons/4g_red.png);border-style: solid;border-width:1px;border-color: rgb(186, 189, 182);");
    }
}

//=======================================================================

extern int success;
extern int loginfail;

void MainWindow::on_pushButton_DriverAccess_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(2);

    ui->pushButton_camView_1->setEnabled(true);
    ui->pushButton_camView_2->setEnabled(true);
    ui->pushButton_camView_full->setEnabled(true);
    ui->pushButton_return->setEnabled(true);
    ui->pushButton_left_up->setEnabled(true);
    ui->pushButton_right_up->setEnabled(true);
    ui->pushButton_left_down->setEnabled(true);
    ui->pushButton_right_down->setEnabled(true);
    ui->pushButton_frame_1->setEnabled(true);
    ui->pushButton_frame_2->setEnabled(true);
    ui->pushButton_frame_3->setEnabled(true);
    ui->pushButton_frame_4->setEnabled(true);
    ui->pushButton_frame_5->setEnabled(true);
    ui->pushButton_car1->setEnabled(true);
    ui->pushButton_Menu->setEnabled(true);
    ui->pushButton_Menu->setStyleSheet("image:url(:/new/icons/menu_2.png);");


    //    libvlc_media_player_play (_mp);
    //    _isPlaying=true;
    //    libvlc_media_player_play (_mp2);
    //    _isPlaying=true;
    //    libvlc_media_player_play (_mp3);
    //    _isPlaying=true;
    //    libvlc_media_player_play (_mp4);
    //    _isPlaying=true;
    //    libvlc_media_player_play (_mp5);
    //    _isPlaying=true;
}


void MainWindow::on_pushButton_MainAccess_clicked()
{
    openlogindialog();

    /*Stop Stream on Different View*/
    stop_view_1_cam();
    stop_view_Mosiac_cam();
    libvlc_media_player_stop (_mp10);
}


void MainWindow::on_pushButton_Lock_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(0);

    ui->pushButton_camView_1->setEnabled(false);
    ui->pushButton_camView_2->setEnabled(false);
    ui->pushButton_camView_full->setEnabled(false);
    ui->pushButton_return->setEnabled(false);
    ui->pushButton_left_up->setEnabled(false);
    ui->pushButton_right_up->setEnabled(false);
    ui->pushButton_left_down->setEnabled(false);
    ui->pushButton_right_down->setEnabled(false);
    ui->pushButton_frame_1->setEnabled(false);
    ui->pushButton_frame_2->setEnabled(false);
    ui->pushButton_frame_3->setEnabled(false);
    ui->pushButton_frame_4->setEnabled(false);
    ui->pushButton_frame_5->setEnabled(false);
    ui->pushButton_car1->setEnabled(false);
    ui->pushButton_Menu->setEnabled(false);

    opendefaultScreen();
    stop_view_1_cam();
    stop_view_Mosiac_cam();
    libvlc_media_player_stop (_mp10);
    libvlc_media_player_stop (_mp11);
    libvlc_media_player_stop (_mp12);
    libvlc_media_player_stop (_mp13);

    timeractive.start();

    //stopping gstreamer
    system("ps -A | grep gst | awk '{ printf $1 }' | xargs kill -2 $1");

    //Updating HMI Logs
    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        // File was successfully created
        QTextStream ts(&file);
        ts<<QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss.zzz")<<", DEVICE LOCKED\n";
        file.close();
    }
}

//Menu Button
void MainWindow::on_pushButton_Menu_clicked()
{
    //For the first click on menu button
    buttonpress++;
    if (buttonpress == 1){
        openlogindialog();
    }

    /*this opens the login window again(on pressing the menu button)
    if window remains inactive for a minute or if login window is closed*/
    else if (loginfail == 1 || timeractive.elapsed() >= 60000){
        openlogindialog();
    }
    else if(success == 1){
        openMenuPage();

        /*Stop Stream on Different View*/
    }
    stop_view_1_cam();
    stop_view_Mosiac_cam();
    libvlc_media_player_stop (_mp10);

    //Resetting parameters for recording page (Page 6 of stackedWidget_Dynamic)
    ui->pushButton_record->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->pushButton_stop->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->label_recording_status->setText("");

    //stopping gstreamer
    system("ps -A | grep gst | awk '{ printf $1 }' | xargs kill -2 $1");
}

void MainWindow::opendefaultScreen()
{
    DefaultS *defaultS = new DefaultS(this);
    defaultS->setWindowFlag(Qt::FramelessWindowHint);
    defaultS->showFullScreen();
    timeractive.start();
}


//Function for opening the driver login dialog
//void MainWindow::opendriverlogindialog(){

//    DriverLogin *driverDialog = new DriverLogin(this);
//    driverDialog->setModal(true);
//    driverDialog->setWindowFlag(Qt::FramelessWindowHint);

//    //This connect logic connects the ok button press (with right password) with the menu page of the stack widget
//    QObject::connect(driverDialog, SIGNAL(okbuttonPressed_driver()), this, SLOT(openDefaultCam()));
//    driverDialog->exec();
//    timeractive.elapsed();
//}

//Function for opening the login dialog
void MainWindow::openlogindialog(){
    //    LoginDialog *loginDialog = new LoginDialog();
    //    loginDialog->setWindowFlags(Qt::FramelessWindowHint);
    //    loginDialog->show();
    // ***************** MODEL APPROACH *****************
    //    LoginDialog LoginDialog;
    //    LoginDialog.setModal(true);

    LoginDialog *loginDialog = new LoginDialog(this);
    loginDialog->setModal(true);
    loginDialog->setWindowFlag(Qt::FramelessWindowHint);

    //This connect logic connects the ok button press (with right password) with the menu page of the stack widget
    QObject::connect(loginDialog, SIGNAL(okbuttonPressed()), this, SLOT(openMenuPage()));
    loginDialog->exec();
    timeractive.elapsed();
}

//void MainWindow::openDefaultCam()
//{
//    ui->stackedWidget_Dynamic->setCurrentIndex(2);
//    ui->pushButton_camView_1->setEnabled(true);
//    ui->pushButton_camView_2->setEnabled(true);
//    ui->pushButton_camView_full->setEnabled(true);
//    ui->pushButton_return->setEnabled(true);
//    ui->pushButton_car1->setEnabled(true);
//    ui->pushButton_Menu->setEnabled(true);
//}

//public slot which opens the second page (index 1) of the stackWidget_Dynamic that is menu .
//menu page opened via login window
void MainWindow::openMenuPage()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(1);
    ui->pushButton_camView_1->setEnabled(true);
    ui->pushButton_camView_2->setEnabled(true);
    ui->pushButton_camView_full->setEnabled(true);
    ui->pushButton_return->setEnabled(true);
    ui->pushButton_left_up->setEnabled(true);
    ui->pushButton_right_up->setEnabled(true);
    ui->pushButton_left_down->setEnabled(true);
    ui->pushButton_right_down->setEnabled(true);
    ui->pushButton_frame_1->setEnabled(true);
    ui->pushButton_frame_2->setEnabled(true);
    ui->pushButton_frame_3->setEnabled(true);
    ui->pushButton_frame_4->setEnabled(true);
    ui->pushButton_frame_5->setEnabled(true);
    ui->pushButton_car1->setEnabled(true);
    ui->pushButton_Menu->setEnabled(true);
}

//=======================================================================

//=======================================================================

//takes back to menu page through return buttons present in individual windows
void MainWindow::openMenuPagereturn()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(1);
}
//=======================================================================

//=======================================================================
//Clicking this button would open the info window
void MainWindow::on_pushButton_info_clicked()
{
    InfoWindow *infowindow = new InfoWindow(this);
    infowindow->setWindowFlag(Qt::FramelessWindowHint);
    //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
    QObject::connect(infowindow, SIGNAL(homebuttonPressedInfo()), this, SLOT(openHomePage()));
    //This connect logic connects the return button press with the second page ( index 1) of stackWidget_Dynamic
    QObject::connect(infowindow, SIGNAL(returnbuttonPressedInfo()), this, SLOT(openMenuPagereturn()));
    infowindow->showFullScreen();

    //opens login dialog if main menu is inactive for a minute
    if (timeractive.elapsed() >= 60000){
        openlogindialog();
    }
    else{
        //timer to keep the window active
        timeractive.start();
    }
}

//Clicking this button would open the device status window
void MainWindow::on_pushButton_deviceStatus_clicked()
{
    DeviceWindow *devicewindow = new DeviceWindow();
    devicewindow->setWindowFlag(Qt::FramelessWindowHint);
    //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
    QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
    //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
    QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
    devicewindow->showFullScreen();

    //opens login dialog if main menu is inactive for a minute
    if (timeractive.elapsed() >= 60000){
        openlogindialog();
    }
    else{
        //timer to keep the window active
        timeractive.start();
    }
}

//Clicking this button would open the video archive window
void MainWindow::on_pushButton_vidArchive_clicked()
{

    VideoArchiveWindow *videoarcwindow = new VideoArchiveWindow(this);
    videoarcwindow->setWindowFlag(Qt::FramelessWindowHint);
    //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
    QObject::connect(videoarcwindow, SIGNAL(homebuttonPressedVid()), this, SLOT(openHomePage()));
    //This connect logic connects the return button press with the second page ( index 1) of stackWidget_Dynamic
    QObject::connect(videoarcwindow, SIGNAL(returnbuttonPressedVid()), this, SLOT(openMenuPagereturn()));
    videoarcwindow->showFullScreen();

    //opens login dialog if main menu is inactive for a minute
    if (timeractive.elapsed() >= 60000){
        openlogindialog();
    }
    else{
        //timer to keep the window active
        timeractive.start();
    }
}

//Clicking this button would open the ETB archive window
void MainWindow::on_pushButton_etbArchive_clicked()
{
    etbarchivewindow *etbarcwindow = new etbarchivewindow(this);
    etbarcwindow->setWindowFlag(Qt::FramelessWindowHint);
    //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
    QObject::connect(etbarcwindow, SIGNAL(homebuttonPressedetb()), this, SLOT(openHomePage()));
    //This connect logic connects the return button press with the second page ( index 1) of stackWidget_Dynamic
    QObject::connect(etbarcwindow, SIGNAL(returnbuttonPressedetb()), this, SLOT(openMenuPagereturn()));
    etbarcwindow->showFullScreen();

    //opens login dialog if main menu is inactive for a minute
    if (timeractive.elapsed() >= 60000){
        openlogindialog();
    }
    else{
        //timer to keep the window active
        timeractive.start();
    }
}

//Clicking this button would open the NVR window
void MainWindow::on_pushButton_nvr_clicked()
{
    NVRWindow *nvrwindow = new NVRWindow(this);
    nvrwindow->setWindowFlag(Qt::FramelessWindowHint);
    //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
    QObject::connect(nvrwindow, SIGNAL(homebuttonPressedNVR()), this, SLOT(openHomePage()));
    //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the NVR window
    QObject::connect(nvrwindow, SIGNAL(returnbuttonPressedNVR()), this, SLOT(openMenuPagereturn()));
    nvrwindow->showFullScreen();

    //opens login dialog if main menu is inactive for a minute
    if (timeractive.elapsed() >= 60000){
        openlogindialog();
    }
    else{
        //timer to keep the window active
        timeractive.start();
    }
}

//Clicking this button would open the Settings window
void MainWindow::on_pushButton_settings_clicked()
{

    SettingsWindow *settingswindow = new SettingsWindow(this);
    settingswindow->setWindowFlag(Qt::FramelessWindowHint);
    //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
    QObject::connect(settingswindow, SIGNAL(homebuttonPressedSettings()), this, SLOT(openHomePage()));
    //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Settings window
    QObject::connect(settingswindow, SIGNAL(returnbuttonPressedSettings()), this, SLOT(openMenuPagereturn()));
    settingswindow->showFullScreen();

    //opens login dialog if main menu is inactive for a minute
    if (timeractive.elapsed() >= 60000){
        openlogindialog();
    }
    else{
        //timer to keep the window active
        timeractive.start();
    }
}

//Clicking this button would open the Logs window
void MainWindow::on_pushButton_logs_clicked()
{
    LogsWindow *logswindow = new LogsWindow(this);
    logswindow->setWindowFlag(Qt::FramelessWindowHint);
    //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
    QObject::connect(logswindow, SIGNAL(homebuttonPressedlogs()), this, SLOT(openHomePage()));
    //This connect logic connects the return button press with the second page ( index 1) of stackWidget_Dynamic
    QObject::connect(logswindow, SIGNAL(returnbuttonPressedlogs()), this, SLOT(openMenuPagereturn()));
    logswindow->showFullScreen();

    //opens login dialog if main menu is inactive for a minute
    if (timeractive.elapsed() >= 60000){
        openlogindialog();
    }
    else{
        //timer to keep the window active
        timeractive.start();
    }
}

void MainWindow::on_pushButton_test_clicked()
{
    //opens login dialog if main menu is inactive for a minute
    if (timeractive.elapsed() >= 60000){
        openlogindialog();
    }
    else{
        //timer to keep the window active
        timeractive.start();
    }
}

//public slot which opens the first page (index 0) of the stackWidget_Dynamic
void MainWindow::openHomePage()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(2);
}


//=======================================================================


//Code for playing videos on labels
void MainWindow::playVideo(){

    //            player=new QMediaPlayer ();
    //            video=new QVideoWidget (ui->label_playVideo) ;
    //            video->show();
    //            player->setVideoOutput(video);
    //            player->setMedia(QUrl("http://192.168.1.10:12060/live.flv?devid=0098004B9A&chl=9&st=0&audio=1"));
    //            video->setGeometry(0,0,ui->label_playVideo->width(),ui->label_playVideo->height());
    //            video->setAspectRatioMode(Qt::IgnoreAspectRatio);
    //            player->play();

    //        player=new QMediaPlayer ();
    //        video=new QVideoWidget (ui->label_playVideo_2) ;
    //        video->show();
    //        player->setVideoOutput(video);
    //        player->setMedia(QUrl("gst-pipeline: udpsrc port=2245 caps=\"application/x-rtp, media=(string)video, clock-rate=(int)90000, payload=(int)96, encoding-name=(string)H264\" ! rtph264depay ! identity silent=0 ! avdec_h264 ! videoconvert ! xvimagesink name=\"qtvideosink\""));

    //        video->setGeometry(0,0,ui->label_playVideo_2->width(),ui->label_playVideo_2->height());
    //        video->setAspectRatioMode(Qt::IgnoreAspectRatio);
    //        player->play();

}


//=======================================================================

//=======================================================================
//List of URLs of IPCAM

//QList<const char*> listSaloonCams = { "rtsp://192.168.1.222/video1.sdp", "rtsp://192.168.1.223/video1.sdp", "rtsp://192.168.1.225/video1.sdp",
//                                      "rtsp://192.168.1.226/video1.sdp", "rtsp://192.168.1.227/video1.sdp", "rtsp://192.168.1.228/video1.sdp",
//                                      "rtsp://192.168.1.229/video1.sdp", "rtsp://192.168.1.230/video1.sdp", "rtsp://192.168.1.231/video1.sdp",
//                                      "rtsp://192.168.1.232/video1.sdp", "rtsp://192.168.1.233/video1.sdp", "rtsp://192.168.1.234/video1.sdp"};

void MainWindow::playStream(){

    //Frames for External Cameras (Front, Back and Front Left/Right and Back Left/Right Cameras
    _m = libvlc_media_new_location(_vlcinstance, listExternalCams[0].toLocal8Bit());
    libvlc_media_player_set_media (_mp, _m);

    _m2 = libvlc_media_new_location(_vlcinstance, listExternalCams[1].toLocal8Bit());
    libvlc_media_player_set_media (_mp2, _m2);

    _m3 = libvlc_media_new_location(_vlcinstance, listExternalCams[2].toLocal8Bit());
    libvlc_media_player_set_media (_mp3, _m3);

    //Frames for Saloon Cameras
    _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[0].toLocal8Bit());
    libvlc_media_player_set_media (_mp4, _m4);

    _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[1].toLocal8Bit());
    libvlc_media_player_set_media (_mp5, _m5);

    _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[0].toLocal8Bit());
    libvlc_media_player_set_media (_mp6, _m6);

    _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[1].toLocal8Bit());
    libvlc_media_player_set_media (_mp7, _m7);

    _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[2].toLocal8Bit());
    libvlc_media_player_set_media (_mp8, _m8);

    _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[3].toLocal8Bit());
    libvlc_media_player_set_media (_mp9, _m9);

    //Frame for viewing all cameras
    _m10 = libvlc_media_new_location(_vlcinstance, listAllCams[0].toLocal8Bit());
    libvlc_media_player_set_media (_mp10, _m10);

    /* Get our media instance to use our window */
#if defined(Q_OS_WIN)
    libvlc_media_player_set_drawable(_mp, reinterpret_cast<unsigned int>(_videoWidget->winId()));
    //libvlc_media_player_set_drawable(_mp, reinterpret_cast<unsigned int>(_videoWidget->winId()), &_vlcexcep ); // [20101215 JG] Used for versions prior to VLC 1.2.0.
    //libvlc_media_player_set_hwnd(_mp, _videoWidget->winId(), &_vlcexcep ); // for vlc 1.0
#elif defined(Q_OS_MAC)
    libvlc_media_player_set_drawable(_mp, _videoWidget->winId());
    //libvlc_media_player_set_drawable(_mp, _videoWidget->winId(), &_vlcexcep ); // [20101215 JG] Used for versions prior to VLC 1.2.0.
    //libvlc_media_player_set_agl (_mp, _videoWidget->winId(), &_vlcexcep); // for vlc 1.0
#else //Linux
    //[20101201 Ondrej Spilka] obsolete call on libVLC >=1.1.5
    //libvlc_media_player_set_drawable(_mp, _videoWidget->winId(), &_vlcexcep );
    //libvlc_media_player_set_xwindow(_mp, _videoWidget->winId(), &_vlcexcep ); // for vlc 1.0

    /* again note X11 handle on Linux is needed
        winID() returns X11 handle when QX11EmbedContainer us used */

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mp, windid );

    int windid2 = ui->frame_2->winId();
    libvlc_media_player_set_xwindow (_mp2, windid2 );

    int windid3 = ui->frame_3->winId();
    libvlc_media_player_set_xwindow (_mp3, windid3 );

    int windid4 = ui->frame_4->winId();
    libvlc_media_player_set_xwindow (_mp4, windid4 );

    int windid5 = ui->frame_5->winId();
    libvlc_media_player_set_xwindow (_mp5, windid5 );

    int windid6 = ui->frame_6->winId();
    libvlc_media_player_set_xwindow (_mp6, windid6 );

    int windid7 = ui->frame_7->winId();
    libvlc_media_player_set_xwindow (_mp7, windid7 );

    int windid8 = ui->frame_8->winId();
    libvlc_media_player_set_xwindow (_mp8, windid8 );

    int windid9 = ui->frame_9->winId();
    libvlc_media_player_set_xwindow (_mp9, windid9 );

    int windid10 = ui->frame_10->winId();
    libvlc_media_player_set_xwindow (_mp10, windid10 );

    libvlc_media_player_play (_mp);
    _isPlaying=true;
    libvlc_media_player_play (_mp2);
    _isPlaying=true;
    libvlc_media_player_play (_mp3);
    _isPlaying=true;
    libvlc_media_player_play (_mp4);
    _isPlaying=true;
    libvlc_media_player_play (_mp5);
    _isPlaying=true;

#endif
}

//Buttons on the main window

void MainWindow::on_pushButton_camView_1_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(2);
    //    returncounter_main = 1;

    //timer to keep the window active
    //timeractive.start();
    //   player->stop();
    //   video->close();

    //   qDebug() << player->state();

    /*Play Stream*/
    //    libvlc_media_player_play (_mp);
    //    _isPlaying=true;
    //    libvlc_media_player_play (_mp2);
    //    _isPlaying=true;
    //    libvlc_media_player_play (_mp3);
    //    _isPlaying=true;
    //    libvlc_media_player_play (_mp4);
    //    _isPlaying=true;
    //    libvlc_media_player_play (_mp5);
    //    _isPlaying=true;

    /*Stop Stream on Different View*/
    stop_view_Mosiac_cam();
    libvlc_media_player_stop (_mp10);
    libvlc_media_player_stop (_mp12);


    //Resetting parameters for recording page (Page 6 of stackedWidget_Dynamic)
    ui->pushButton_record->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->pushButton_stop->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->label_recording_status->setText("");

    if(iExtCam==0){
        ui->label_frame_1->setText("COACH - "+QString::number(coach_1.at(iExtCam)->csno)+" "+coach_1.at(iExtCam)->car_type_desc+"_"+coach_1.at(iExtCam)->camera_type_desc);
        ui->label_frame_2->setText("COACH - "+QString::number(coach_1.at(iExtCam+1)->csno)+" "+coach_1.at(iExtCam+1)->car_type_desc+"_"+coach_1.at(iExtCam+1)->camera_type_desc);
        ui->label_frame_3->setText("COACH - "+QString::number(coach_1.at(iExtCam+2)->csno)+" "+coach_1.at(iExtCam+2)->car_type_desc+"_"+coach_1.at(iExtCam+2)->camera_type_desc);
    }

    if(iSaloonCam==0){
        ui->label_frame_4->setText("COACH - "+QString::number(coach_1.at(iSaloonCam+4)->csno)+" "+coach_1.at(iSaloonCam+4)->car_type_desc+"_"+coach_1.at(iSaloonCam+4)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iSaloonCam+4)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_1.at(iSaloonCam+5)->csno)+" "+coach_1.at(iSaloonCam+5)->car_type_desc+"_"+coach_1.at(iSaloonCam+5)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iSaloonCam+5)->sno));
    }

    system("ps -A | grep gst | awk '{ printf $1 }' | xargs kill -2 $1");
}

void MainWindow::stop_view_1_cam(){
    libvlc_media_player_stop (_mp);
    libvlc_media_player_stop (_mp2);
    libvlc_media_player_stop (_mp3);
    libvlc_media_player_stop (_mp4);
    libvlc_media_player_stop (_mp5);
}

//For Mosiac View
void MainWindow::on_pushButton_camView_2_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(3);
    returncounter_main = 1;
    //timer to keep the window active
    //timeractive.start();
    //    player->stop();
    //    video->close();

    //    qDebug() << player->state();

    /*Stop Stream on Different View*/
    stop_view_1_cam();
    libvlc_media_player_stop (_mp10);
    libvlc_media_player_stop (_mp12);

    /*Play Stream*/
    //    libvlc_media_player_play (_mp6);
    //    _isPlaying=true;
    //    libvlc_media_player_play (_mp7);
    //    _isPlaying=true;
    //    libvlc_media_player_play (_mp8);
    //    _isPlaying=true;
    //    libvlc_media_player_play (_mp9);
    //    _isPlaying=true;


    //Resetting parameters for recording page (Page 6 of stackedWidget_Dynamic)
    ui->pushButton_record->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->pushButton_stop->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->label_recording_status->setText("");

    if(iMosiacCam==0){
        ui->label_frame_6->setText("COACH - "+QString::number(coach_1.at(iMosiacCam+4)->csno)+" "+coach_1.at(iMosiacCam+4)->car_type_desc+"_"+coach_1.at(iMosiacCam+4)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam+4)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_1.at(iMosiacCam+5)->csno)+" "+coach_1.at(iMosiacCam+5)->car_type_desc+"_"+coach_1.at(iMosiacCam+5)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam+5)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_1.at(iMosiacCam+6)->csno)+" "+coach_1.at(iMosiacCam+6)->car_type_desc+"_"+coach_1.at(iMosiacCam+6)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam+6)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_1.at(iMosiacCam+7)->csno)+" "+coach_1.at(iMosiacCam+7)->car_type_desc+"_"+coach_1.at(iMosiacCam+7)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam+7)->sno));
    }

    system("ps -A | grep gst | awk '{ printf $1 }' | xargs kill -2 $1");

}

void MainWindow::stop_view_Mosiac_cam(){
    libvlc_media_player_stop (_mp6);
    libvlc_media_player_stop (_mp7);
    libvlc_media_player_stop (_mp8);
    libvlc_media_player_stop (_mp9);
}

//For Full View
void MainWindow::on_pushButton_camView_full_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(4);
    returncounter_main = 1;
    //timer to keep the window active
    //timeractive.start();

    //    video->show();
    //    player->play();

    //    qDebug() << player->state();

    /*Stop Stream on Different View*/
    stop_view_1_cam();
    stop_view_Mosiac_cam();

    /*Play Stream*/
    //    libvlc_media_player_play (_mp10);
    //    _isPlaying=true;


    //Resetting parameters for recording page (Page 6 of stackedWidget_Dynamic)
    ui->pushButton_record->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->pushButton_stop->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->label_recording_status->setText("");

    if(iFullCam==0){
        ui->label_frame_10->setText("COACH NO - "+QString::number(coach_1.at(iFullCam)->csno)+" COACH TYPE - "+coach_1.at(iFullCam)->car_type_desc);
        ui->label_cam_no_2->setText(coach_1.at(iFullCam)->camera_type_desc);
    }

    system("ps -A | grep gst | awk '{ printf $1 }' | xargs kill -2 $1");

}

void MainWindow::stop_all_cam(){
    libvlc_media_player_stop (_mp);
    libvlc_media_player_stop (_mp2);
    libvlc_media_player_stop (_mp3);
    libvlc_media_player_stop (_mp4);
    libvlc_media_player_stop (_mp5);
    libvlc_media_player_stop (_mp6);
    libvlc_media_player_stop (_mp7);
    libvlc_media_player_stop (_mp8);
    libvlc_media_player_stop (_mp9);
    libvlc_media_player_stop (_mp10);
    libvlc_media_player_stop (_mp12);
}

void MainWindow::on_pushButton_return_clicked()
{

    if(returncounter_main == 0){
        ui->stackedWidget_Dynamic->setCurrentIndex(0);
        ui->pushButton_Menu->setEnabled(false);
        ui->pushButton_camView_1->setEnabled(false);
        ui->pushButton_camView_2->setEnabled(false);
        ui->pushButton_camView_full->setEnabled(false);
    }

    else if(returncounter_main == 1){
        ui->stackedWidget_Dynamic->setCurrentIndex(2);
        returncounter_main --;
        /*Play Stream*/

        //        libvlc_media_player_play (_mp);
        //        _isPlaying=true;
        //        libvlc_media_player_play (_mp2);
        //        _isPlaying=true;
        //        libvlc_media_player_play (_mp3);
        //        _isPlaying=true;
        //        libvlc_media_player_play (_mp4);
        //        _isPlaying=true;
        //        libvlc_media_player_play (_mp5);
        //        _isPlaying=true;


        //        if (timeractive.elapsed() >= 60000){
        //            openlogindialog();
        //        }
        //        else{
        //            //timer to keep the window active
        //            timeractive.start();
        //        }
    }

    //    qDebug() << player->state();

    libvlc_media_player_stop (_mp12);



    //Resetting parameters for recording page (Page 6 of stackedWidget_Dynamic)
    ui->pushButton_record->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->pushButton_stop->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->label_recording_status->setText("");

}
//=======================================================================
//Function to iterate External IPCAM for 5 cam view (top 3 frames)

void MainWindow::iterateExternalCams()
{
    if(iExtCam == 0) {

        if(coach_1.at(iExtCam)->activeStatus==ACTIVE){
            _m = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam].toLocal8Bit()); //External Cam list index 0 (Coach 1 , 1st CAM i.e Front CAM)
            libvlc_media_player_set_media (_mp, _m);
            int windid = ui->frame->winId();
            libvlc_media_player_set_xwindow (_mp, windid );
            libvlc_media_player_play (_mp);
            _isPlaying=true;
        }
        if(coach_1.at(iExtCam+1)->activeStatus==ACTIVE){
            _m2 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+1].toLocal8Bit()); //External Cam list index 1 (Coach 1 , 2nd CAM i.e Front Right)
            libvlc_media_player_set_media (_mp2, _m2);
            int windid2 = ui->frame_2->winId();
            libvlc_media_player_set_xwindow (_mp2, windid2 );
            libvlc_media_player_play (_mp2);
            _isPlaying=true;
        }
        if(coach_1.at(iExtCam+2)->activeStatus==ACTIVE){
            _m3 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+2].toLocal8Bit()); //External Cam list index 2 (Coach 1 , 3rd CAM i.e Front Left)
            libvlc_media_player_set_media (_mp3, _m3);
            int windid3 = ui->frame_3->winId();
            libvlc_media_player_set_xwindow (_mp3, windid3 );
            libvlc_media_player_play (_mp3);
            _isPlaying=true;
        }
        ui->label_frame_1->setText("COACH - "+QString::number(coach_1.at(iExtCam)->csno)+" "+coach_1.at(iExtCam)->car_type_desc+"_"+coach_1.at(iExtCam)->camera_type_desc);
        ui->label_frame_2->setText("COACH - "+QString::number(coach_1.at(iExtCam+1)->csno)+" "+coach_1.at(iExtCam+1)->car_type_desc+"_"+coach_1.at(iExtCam+1)->camera_type_desc);
        ui->label_frame_3->setText("COACH - "+QString::number(coach_1.at(iExtCam+2)->csno)+" "+coach_1.at(iExtCam+2)->car_type_desc+"_"+coach_1.at(iExtCam+2)->camera_type_desc);
    }

    else if(iExtCam == 1) {

        if(coach_1.at(iExtCam)->activeStatus==ACTIVE){
            _m = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam].toLocal8Bit()); //External Cam list index 1 (Coach 1 , 2nd CAM i.e Front Right)
            libvlc_media_player_set_media (_mp, _m);
            int windid = ui->frame->winId();
            libvlc_media_player_set_xwindow (_mp, windid );
            libvlc_media_player_play (_mp);
            _isPlaying=true;
        }
        if(coach_1.at(iExtCam+1)->activeStatus==ACTIVE){
            _m2 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+1].toLocal8Bit()); //External Cam list index 2 (Coach 1 , 3rd CAM i.e Front Left)
            libvlc_media_player_set_media (_mp2, _m2);
            int windid2 = ui->frame_2->winId();
            libvlc_media_player_set_xwindow (_mp2, windid2 );
            libvlc_media_player_play (_mp2);
            _isPlaying=true;
        }
        if(coach_1.at(iExtCam+2)->activeStatus==ACTIVE){
            _m3 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+2].toLocal8Bit()); //External Cam list index 3 (Coach 1 , 4th CAM i.e Front CAB)
            libvlc_media_player_set_media (_mp3, _m3);
            int windid3 = ui->frame_3->winId();
            libvlc_media_player_set_xwindow (_mp3, windid3 );
            libvlc_media_player_play (_mp3);
            _isPlaying=true;
        }
        ui->label_frame_1->setText("COACH - "+QString::number(coach_1.at(iExtCam)->csno)+" "+coach_1.at(iExtCam)->car_type_desc+"_"+coach_1.at(iExtCam)->camera_type_desc);
        ui->label_frame_2->setText("COACH - "+QString::number(coach_1.at(iExtCam+1)->csno)+" "+coach_1.at(iExtCam+1)->car_type_desc+"_"+coach_1.at(iExtCam+1)->camera_type_desc);
        ui->label_frame_3->setText("COACH - "+QString::number(coach_1.at(iExtCam+2)->csno)+" "+coach_1.at(iExtCam+2)->car_type_desc+"_"+coach_1.at(iExtCam+2)->camera_type_desc);
    }

    else if(iExtCam == 2){

        if(coach_1.at(iExtCam)->activeStatus==ACTIVE){
            _m = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam].toLocal8Bit()); //External Cam list index 2 (Coach 1 , 3rd CAM i.e Front Left)
            libvlc_media_player_set_media (_mp, _m);
            int windid = ui->frame->winId();
            libvlc_media_player_set_xwindow (_mp, windid );
            libvlc_media_player_play (_mp);
            _isPlaying=true;
        }
        if(coach_1.at(iExtCam+1)->activeStatus==ACTIVE){
            _m2 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+1].toLocal8Bit()); //External Cam list index 3 (Coach 1 , 4th CAM i.e Front CAB)
            libvlc_media_player_set_media (_mp2, _m2);
            int windid2 = ui->frame_2->winId();
            libvlc_media_player_set_xwindow (_mp2, windid2 );
            libvlc_media_player_play (_mp2);
            _isPlaying=true;
        }
        if(coach_12.at(iExtCam-2)->activeStatus==ACTIVE){
            _m3 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+2].toLocal8Bit()); //External Cam list index 4 (Coach 12 , 1st CAM i.e Rear CAM)
            libvlc_media_player_set_media (_mp3, _m3);
            int windid3 = ui->frame_3->winId();
            libvlc_media_player_set_xwindow (_mp3, windid3 );
            libvlc_media_player_play (_mp3);
            _isPlaying=true;
        }
        ui->label_frame_1->setText("COACH - "+QString::number(coach_1.at(iExtCam)->csno)+" "+coach_1.at(iExtCam)->car_type_desc+"_"+coach_1.at(iExtCam)->camera_type_desc);
        ui->label_frame_2->setText("COACH - "+QString::number(coach_1.at(iExtCam+1)->csno)+" "+coach_1.at(iExtCam+1)->car_type_desc+"_"+coach_1.at(iExtCam+1)->camera_type_desc);
        ui->label_frame_3->setText("COACH - "+QString::number(coach_12.at(iExtCam-2)->csno)+" "+coach_12.at(iExtCam-2)->car_type_desc+"_"+coach_12.at(iExtCam-2)->camera_type_desc);
    }

    else if(iExtCam == 3){
        if(coach_1.at(iExtCam)->activeStatus==ACTIVE){
            _m = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam].toLocal8Bit()); //External Cam list index 3 (Coach 1 , 4th CAM i.e Front CAB)
            libvlc_media_player_set_media (_mp, _m);
            int windid = ui->frame->winId();
            libvlc_media_player_set_xwindow (_mp, windid );
            libvlc_media_player_play (_mp);
            _isPlaying=true;
        }
        if(coach_12.at(iExtCam-3)->activeStatus==ACTIVE){
            _m2 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+1].toLocal8Bit()); //External Cam list index 4 (Coach 12 , 1st CAM i.e Rear CAM)
            libvlc_media_player_set_media (_mp2, _m2);
            int windid2 = ui->frame_2->winId();
            libvlc_media_player_set_xwindow (_mp2, windid2 );
            libvlc_media_player_play (_mp2);
            _isPlaying=true;
        }
        if(coach_12.at(iExtCam-2)->activeStatus==ACTIVE){
            _m3 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+2].toLocal8Bit()); //External Cam list index 5 (Coach 12 , 2nd CAM i.e Rear Right)
            libvlc_media_player_set_media (_mp3, _m3);
            int windid3 = ui->frame_3->winId();
            libvlc_media_player_set_xwindow (_mp3, windid3 );
            libvlc_media_player_play (_mp3);
            _isPlaying=true;
        }
        ui->label_frame_1->setText("COACH - "+QString::number(coach_1.at(iExtCam)->csno)+" "+coach_1.at(iExtCam)->car_type_desc+"_"+coach_1.at(iExtCam)->camera_type_desc);
        ui->label_frame_2->setText("COACH - "+QString::number(coach_12.at(iExtCam-3)->csno)+" "+coach_12.at(iExtCam-3)->car_type_desc+"_"+coach_12.at(iExtCam-3)->camera_type_desc);
        ui->label_frame_3->setText("COACH - "+QString::number(coach_12.at(iExtCam-2)->csno)+" "+coach_12.at(iExtCam-2)->car_type_desc+"_"+coach_12.at(iExtCam-2)->camera_type_desc);
    }

    else if(iExtCam == 4){
        if(coach_12.at(iExtCam-4)->activeStatus==ACTIVE){
            _m = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam].toLocal8Bit()); //External Cam list index 4 (Coach 12 , 1st CAM i.e Rear CAM)
            libvlc_media_player_set_media (_mp, _m);
            int windid = ui->frame->winId();
            libvlc_media_player_set_xwindow (_mp, windid );
            libvlc_media_player_play (_mp);
            _isPlaying=true;
        }
        if(coach_12.at(iExtCam-3)->activeStatus==ACTIVE){
            _m2 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+1].toLocal8Bit()); //External Cam list index 5 (Coach 12 , 2nd CAM i.e Rear Right)
            libvlc_media_player_set_media (_mp2, _m2);
            int windid2 = ui->frame_2->winId();
            libvlc_media_player_set_xwindow (_mp2, windid2 );
            libvlc_media_player_play (_mp2);
            _isPlaying=true;
        }
        if(coach_12.at(iExtCam-2)->activeStatus==ACTIVE){
            _m3 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+2].toLocal8Bit()); //External Cam list index 6 (Coach 12 , 3rd CAM i.e Rear Left)
            libvlc_media_player_set_media (_mp3, _m3);
            int windid3 = ui->frame_3->winId();
            libvlc_media_player_set_xwindow (_mp3, windid3 );
            libvlc_media_player_play (_mp3);
            _isPlaying=true;
        }
        ui->label_frame_1->setText("COACH - "+QString::number(coach_12.at(iExtCam-4)->csno)+" "+coach_12.at(iExtCam-4)->car_type_desc+"_"+coach_12.at(iExtCam-4)->camera_type_desc);
        ui->label_frame_2->setText("COACH - "+QString::number(coach_12.at(iExtCam-3)->csno)+" "+coach_12.at(iExtCam-3)->car_type_desc+"_"+coach_12.at(iExtCam-3)->camera_type_desc);
        ui->label_frame_3->setText("COACH - "+QString::number(coach_12.at(iExtCam-2)->csno)+" "+coach_12.at(iExtCam-2)->car_type_desc+"_"+coach_12.at(iExtCam-2)->camera_type_desc);
    }

    else if(iExtCam == 5){
        if(coach_12.at(iExtCam-4)->activeStatus==ACTIVE){
            _m = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam].toLocal8Bit()); //External Cam list index 5 (Coach 12 , 2nd CAM i.e Rear Right)
            libvlc_media_player_set_media (_mp, _m);
            int windid = ui->frame->winId();
            libvlc_media_player_set_xwindow (_mp, windid );
            libvlc_media_player_play (_mp);
            _isPlaying=true;
        }
        if(coach_12.at(iExtCam-3)->activeStatus==ACTIVE){
            _m2 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+1].toLocal8Bit()); //External Cam list index 6 (Coach 12 , 3rd CAM i.e Rear Left)
            libvlc_media_player_set_media (_mp2, _m2);
            int windid2 = ui->frame_2->winId();
            libvlc_media_player_set_xwindow (_mp2, windid2 );
            libvlc_media_player_play (_mp2);
            _isPlaying=true;
        }
        if(coach_12.at(iExtCam-2)->activeStatus==ACTIVE){
            _m3 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+2].toLocal8Bit()); //External Cam list index 7 (Coach 12 , 4th CAM i.e Rear CAB)
            libvlc_media_player_set_media (_mp3, _m3);
            int windid3 = ui->frame_3->winId();
            libvlc_media_player_set_xwindow (_mp3, windid3 );
            libvlc_media_player_play (_mp3);
            _isPlaying=true;
        }
        ui->label_frame_1->setText("COACH - "+QString::number(coach_12.at(iExtCam-4)->csno)+" "+coach_12.at(iExtCam-4)->car_type_desc+"_"+coach_12.at(iExtCam-4)->camera_type_desc);
        ui->label_frame_2->setText("COACH - "+QString::number(coach_12.at(iExtCam-3)->csno)+" "+coach_12.at(iExtCam-3)->car_type_desc+"_"+coach_12.at(iExtCam-3)->camera_type_desc);
        ui->label_frame_3->setText("COACH - "+QString::number(coach_12.at(iExtCam-2)->csno)+" "+coach_12.at(iExtCam-2)->car_type_desc+"_"+coach_12.at(iExtCam-2)->camera_type_desc);
    }

    else if(iExtCam == 6){
        if(coach_12.at(iExtCam-4)->activeStatus==ACTIVE){
            _m = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam].toLocal8Bit()); //External Cam list index 6 (Coach 12 , 3rd CAM i.e Rear Left)
            libvlc_media_player_set_media (_mp, _m);
            int windid = ui->frame->winId();
            libvlc_media_player_set_xwindow (_mp, windid );
            libvlc_media_player_play (_mp);
            _isPlaying=true;
        }
        if(coach_12.at(iExtCam-3)->activeStatus==ACTIVE) {
            _m2 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+1].toLocal8Bit()); //External Cam list index 7 (Coach 12 , 4th CAM i.e Rear CAB)
            libvlc_media_player_set_media (_mp2, _m2);
            int windid2 = ui->frame_2->winId();
            libvlc_media_player_set_xwindow (_mp2, windid2 );
            libvlc_media_player_play (_mp2);
            _isPlaying=true;
        }
        if(coach_1.at(iExtCam-6)->activeStatus==ACTIVE){
            _m3 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam-6].toLocal8Bit()); //External Cam list index 0 (Coach 1 , 1st CAM i.e Front CAM)
            libvlc_media_player_set_media (_mp3, _m3);
            int windid3 = ui->frame_3->winId();
            libvlc_media_player_set_xwindow (_mp3, windid3 );
            libvlc_media_player_play (_mp3);
            _isPlaying=true;
        }
        ui->label_frame_1->setText("COACH - "+QString::number(coach_12.at(iExtCam-4)->csno)+" "+coach_12.at(iExtCam-4)->car_type_desc+"_"+coach_12.at(iExtCam-4)->camera_type_desc);
        ui->label_frame_2->setText("COACH - "+QString::number(coach_12.at(iExtCam-3)->csno)+" "+coach_12.at(iExtCam-3)->car_type_desc+"_"+coach_12.at(iExtCam-3)->camera_type_desc);
        ui->label_frame_3->setText("COACH - "+QString::number(coach_1.at(iExtCam-6)->csno)+" "+coach_1.at(iExtCam-6)->car_type_desc+"_"+coach_1.at(iExtCam-6)->camera_type_desc);

    }

    else{
        //Stopping player
        libvlc_media_player_stop (_mp);
        libvlc_media_player_stop (_mp2);
        libvlc_media_player_stop (_mp3);
    }
}

//Button on the 5 camera view page
void MainWindow::on_pushButton_left_up_clicked()
{
    //timer to keep the window active
    timeractive.start();

    //Stopping player
    libvlc_media_player_stop (_mp);
    libvlc_media_player_stop (_mp2);
    libvlc_media_player_stop (_mp3);
    libvlc_media_player_stop (_mp12);

    if(iExtCam==0){
        iExtCam = listExternalCams.length() - 2; //4
    }

    iExtCam --;


    iterateExternalCams();

}
void MainWindow::on_pushButton_right_up_clicked()
{
    //timer to keep the window active
    timeractive.start();

    //Stopping player
    libvlc_media_player_stop (_mp);
    libvlc_media_player_stop (_mp2);
    libvlc_media_player_stop (_mp3);
    libvlc_media_player_stop (_mp12);

    if(iExtCam==listExternalCams.length() - 3) //3
    {
        iExtCam = -1;
    }

    iExtCam ++;


    iterateExternalCams();
}

//Function to iterate Saloon IPCAM for 5 cam view ( botton 2 frames)

void MainWindow::playerFrame4(){
    libvlc_media_player_set_media (_mp4, _m4);
    int windid4 = ui->frame_4->winId();
    libvlc_media_player_set_xwindow (_mp4, windid4 );
    libvlc_media_player_play (_mp4);
    _isPlaying=true;
}

void MainWindow::playerFrame5(){
    libvlc_media_player_set_media (_mp5, _m5);
    int windid5 = ui->frame_5->winId();
    libvlc_media_player_set_xwindow (_mp5, windid5 );
    libvlc_media_player_play (_mp5);
    _isPlaying=true;
}

void MainWindow::iterateSaloonCams()
{

    //Iterates Saloon Camera 1-7 for coach 1
    if(iSaloonCam >= 0 && iSaloonCam < 7) {

        if(coach_1.at(iSaloonCam+4)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit());
            playerFrame4();
        }
        if(coach_1.at(iSaloonCam+5)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit());
            playerFrame5();
        }
        //        ui->label_frame_4->setText(listSaloonCams[iSaloonCam]);
        //        ui->label_frame_5->setText(listSaloonCams[iSaloonCam+1]);
        ui->label_frame_4->setText("COACH - "+QString::number(coach_1.at(iSaloonCam+4)->csno)+" "+coach_1.at(iSaloonCam+4)->car_type_desc+"_"+coach_1.at(iSaloonCam+4)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iSaloonCam+4)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_1.at(iSaloonCam+5)->csno)+" "+coach_1.at(iSaloonCam+5)->car_type_desc+"_"+coach_1.at(iSaloonCam+5)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iSaloonCam+5)->sno));
    }

    //For Saloom Cam 8 of coach 1 & Saloon Cam 1 of coach 2
    if(iSaloonCam == 7) {
        if(coach_1.at(iSaloonCam+4)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit()); //Coach 1 Saloon Camera 8 (index 7 in listSaloonCam)
            playerFrame4();
        }
        if(coach_2.at(iSaloonCam-7)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit()); //Coach 2 Saloon Camera 1 (index 8 in listSaloonCam)
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_1.at(iSaloonCam+4)->csno)+" "+coach_1.at(iSaloonCam+4)->car_type_desc+"_"+coach_1.at(iSaloonCam+4)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iSaloonCam+4)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_2.at(iSaloonCam-7)->csno)+" "+coach_2.at(iSaloonCam-7)->car_type_desc+"_"+coach_2.at(iSaloonCam-7)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iSaloonCam-7)->sno));
    }

    //Iterates Saloon Camera 1-7 for coach 2
    if(iSaloonCam >= 8 && iSaloonCam < 15) {
        if(coach_2.at(iSaloonCam-8)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit());
            playerFrame4();
        }
        if(coach_2.at(iSaloonCam-7)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit());
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_2.at(iSaloonCam-8)->csno)+" "+coach_2.at(iSaloonCam-8)->car_type_desc+"_"+coach_2.at(iSaloonCam-8)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iSaloonCam-8)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_2.at(iSaloonCam-7)->csno)+" "+coach_2.at(iSaloonCam-7)->car_type_desc+"_"+coach_2.at(iSaloonCam-7)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iSaloonCam-7)->sno));
    }

    //For Saloom Cam 8 of coach 2 & Saloon Cam 1 of coach 3
    if(iSaloonCam == 15) {
        if(coach_2.at(iSaloonCam-8)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit()); //Coach 2 Saloon Camera 8 (index 15 in listSaloonCam)
            playerFrame4();
        }
        if(coach_3.at(iSaloonCam-15)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit()); //Coach 3 Saloon Camera 1 (index 16 in listSaloonCam)
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_2.at(iSaloonCam-8)->csno)+" "+coach_2.at(iSaloonCam-8)->car_type_desc+"_"+coach_2.at(iSaloonCam-8)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iSaloonCam-8)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_3.at(iSaloonCam-15)->csno)+" "+coach_3.at(iSaloonCam-15)->car_type_desc+"_"+coach_3.at(iSaloonCam-15)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iSaloonCam-15)->sno));
    }

    //Iterates Saloon Camera 1-7 for coach 3
    if(iSaloonCam >= 16 && iSaloonCam < 23) {
        if(coach_3.at(iSaloonCam-16)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit());
            playerFrame4();
        }
        if(coach_3.at(iSaloonCam-15)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit());
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_3.at(iSaloonCam-16)->csno)+" "+coach_3.at(iSaloonCam-16)->car_type_desc+"_"+coach_3.at(iSaloonCam-16)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iSaloonCam-16)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_3.at(iSaloonCam-15)->csno)+" "+coach_3.at(iSaloonCam-15)->car_type_desc+"_"+coach_3.at(iSaloonCam-15)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iSaloonCam-15)->sno));
    }

    //For Saloom Cam 8 of coach 3 & Saloon Cam 1 of coach 4
    if(iSaloonCam == 23) {
        if(coach_3.at(iSaloonCam-16)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit()); //Coach 3 Saloon Camera 8 (index 23 in listSaloonCam)
            playerFrame4();
        }
        if(coach_4.at(iSaloonCam-23)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit()); //Coach 4 Saloon Camera 1 (index 24 in listSaloonCam)
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_3.at(iSaloonCam-16)->csno)+" "+coach_3.at(iSaloonCam-16)->car_type_desc+"_"+coach_3.at(iSaloonCam-16)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iSaloonCam-16)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_4.at(iSaloonCam-23)->csno)+" "+coach_4.at(iSaloonCam-23)->car_type_desc+"_"+coach_4.at(iSaloonCam-23)->camera_type_desc+
                                   "_"+QString::number(coach_4.at(iSaloonCam-23)->sno));
    }

    //Iterates Saloon Camera 1-7 for coach 4
    if(iSaloonCam >= 24 && iSaloonCam < 31) {
        if(coach_4.at(iSaloonCam-24)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit());
            playerFrame4();
        }
        if(coach_4.at(iSaloonCam-23)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit());
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_4.at(iSaloonCam-24)->csno)+" "+coach_4.at(iSaloonCam-24)->car_type_desc+"_"+coach_4.at(iSaloonCam-24)->camera_type_desc+"_"
                                   +QString::number(coach_4.at(iSaloonCam-24)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_4.at(iSaloonCam-23)->csno)+" "+coach_4.at(iSaloonCam-23)->car_type_desc+"_"+coach_4.at(iSaloonCam-23)->camera_type_desc+"_"
                                   +QString::number(coach_4.at(iSaloonCam-23)->sno));
    }

    //For Saloom Cam 8 of coach 4 & Saloon Cam 1 of coach 5
    if(iSaloonCam == 31) {
        if(coach_4.at(iSaloonCam-24)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit()); //Coach 5 Saloon Camera 8 (index 39 in listSaloonCam)
            playerFrame4();
        }
        if(coach_5.at(iSaloonCam-31)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit()); //Coach 6 Saloon Camera 1 (index 40 in listSaloonCam)
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_4.at(iSaloonCam-24)->csno)+" "+coach_4.at(iSaloonCam-24)->car_type_desc+"_"+coach_4.at(iSaloonCam-24)->camera_type_desc+"_"
                                   +QString::number(coach_4.at(iSaloonCam-24)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_5.at(iSaloonCam-31)->csno)+" "+coach_5.at(iSaloonCam-31)->car_type_desc+"_"+coach_5.at(iSaloonCam-31)->camera_type_desc+"_"
                                   +QString::number(coach_5.at(iSaloonCam-31)->sno));
    }

    //Iterates Saloon Camera 1-7 for coach 5
    if(iSaloonCam >= 32 && iSaloonCam < 39) {
        if(coach_5.at(iSaloonCam-32)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit());
            playerFrame4();
        }
        if(coach_5.at(iSaloonCam-31)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit());
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_5.at(iSaloonCam-32)->csno)+" "+coach_5.at(iSaloonCam-32)->car_type_desc+"_"+coach_5.at(iSaloonCam-32)->camera_type_desc+"_"
                                   +QString::number(coach_5.at(iSaloonCam-32)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_5.at(iSaloonCam-31)->csno)+" "+coach_5.at(iSaloonCam-31)->car_type_desc+"_"+coach_5.at(iSaloonCam-31)->camera_type_desc+"_"
                                   +QString::number(coach_5.at(iSaloonCam-31)->sno));
    }

    //For Saloom Cam 8 of coach 5 & Saloon Cam 1 of coach 6
    if(iSaloonCam == 39) {
        if(coach_5.at(iSaloonCam-32)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit()); //Coach 5 Saloon Camera 8 (index 39 in listSaloonCam)
            playerFrame4();
        }
        if(coach_6.at(iSaloonCam-39)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit()); //Coach 6 Saloon Camera 1 (index 40 in listSaloonCam)
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_5.at(iSaloonCam-32)->csno)+" "+coach_5.at(iSaloonCam-32)->car_type_desc+"_"+coach_5.at(iSaloonCam-32)->camera_type_desc+"_"
                                   +QString::number(coach_5.at(iSaloonCam-32)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_6.at(iSaloonCam-39)->csno)+" "+coach_6.at(iSaloonCam-39)->car_type_desc+"_"+coach_6.at(iSaloonCam-39)->camera_type_desc+"_"
                                   +QString::number(coach_6.at(iSaloonCam-39)->sno));
    }

    //Iterates Saloon Camera 1-7 for coach 6
    if(iSaloonCam >= 40 && iSaloonCam < 47) {
        if(coach_6.at(iSaloonCam-40)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit());
            playerFrame4();
        }
        if(coach_6.at(iSaloonCam-39)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit());
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_5.at(iSaloonCam-40)->csno)+" "+coach_6.at(iSaloonCam-40)->car_type_desc+"_"+coach_6.at(iSaloonCam-40)->camera_type_desc+"_"
                                   +QString::number(coach_6.at(iSaloonCam-40)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_5.at(iSaloonCam-39)->csno)+" "+coach_6.at(iSaloonCam-39)->car_type_desc+"_"+coach_6.at(iSaloonCam-39)->camera_type_desc+"_"
                                   +QString::number(coach_6.at(iSaloonCam-39)->sno));
    }

    //For Saloom Cam 8 of coach 6 & Saloon Cam 1 of coach 7
    if(iSaloonCam == 47) {
        if(coach_6.at(iSaloonCam-40)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit()); //Coach 6 Saloon Camera 8 (index 47 in listSaloonCam)
            playerFrame4();
        }
        if(coach_7.at(iSaloonCam-47)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit()); //Coach 7 Saloon Camera 1 (index 48 in listSaloonCam)
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_6.at(iSaloonCam-40)->csno)+" "+coach_6.at(iSaloonCam-40)->car_type_desc+"_"+coach_6.at(iSaloonCam-40)->camera_type_desc+"_"
                                   +QString::number(coach_6.at(iSaloonCam-40)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_7.at(iSaloonCam-47)->csno)+" "+coach_7.at(iSaloonCam-47)->car_type_desc+"_"+coach_7.at(iSaloonCam-47)->camera_type_desc+"_"
                                   +QString::number(coach_7.at(iSaloonCam-47)->sno));
    }

    //Iterates Saloon Camera 1-7 for coach 7
    if(iSaloonCam >= 48 && iSaloonCam < 55) {
        if(coach_7.at(iSaloonCam-48)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit());
            playerFrame4();
        }
        if(coach_7.at(iSaloonCam-47)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit());
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_7.at(iSaloonCam-48)->csno)+" "+coach_7.at(iSaloonCam-48)->car_type_desc+"_"+coach_7.at(iSaloonCam-48)->camera_type_desc+"_"
                                   +QString::number(coach_7.at(iSaloonCam-48)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_7.at(iSaloonCam-47)->csno)+" "+coach_7.at(iSaloonCam-47)->car_type_desc+"_"+coach_7.at(iSaloonCam-47)->camera_type_desc+"_"
                                   +QString::number(coach_7.at(iSaloonCam-47)->sno));
    }

    //For Saloom Cam 8 of coach 7 & Saloon Cam 1 of coach 8
    if(iSaloonCam == 55) {
        if(coach_7.at(iSaloonCam-48)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit()); //Coach 7 Saloon Camera 8 (index 55 in listSaloonCam)
            playerFrame4();
        }
        if(coach_8.at(iSaloonCam-55)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit()); //Coach 8 Saloon Camera 1 (index 56 in listSaloonCam)
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_8.at(iSaloonCam-48)->csno)+" "+coach_7.at(iSaloonCam-48)->car_type_desc+"_"+coach_7.at(iSaloonCam-48)->camera_type_desc+"_"
                                   +QString::number(coach_7.at(iSaloonCam-48)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_8.at(iSaloonCam-55)->csno)+" "+coach_8.at(iSaloonCam-55)->car_type_desc+"_"+coach_8.at(iSaloonCam-55)->camera_type_desc+"_"
                                   +QString::number(coach_8.at(iSaloonCam-55)->sno));
    }

    //Iterates Saloon Camera 1-7 for coach 8
    if(iSaloonCam >= 56 && iSaloonCam < 63) {
        if(coach_8.at(iSaloonCam-56)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit());
            playerFrame4();
        }
        if(coach_8.at(iSaloonCam-55)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit());
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_8.at(iSaloonCam-56)->csno)+" "+coach_8.at(iSaloonCam-56)->car_type_desc+"_"+coach_8.at(iSaloonCam-56)->camera_type_desc+"_"
                                   +QString::number(coach_8.at(iSaloonCam-56)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_8.at(iSaloonCam-55)->csno)+" "+coach_8.at(iSaloonCam-55)->car_type_desc+"_"+coach_8.at(iSaloonCam-55)->camera_type_desc+"_"
                                   +QString::number(coach_8.at(iSaloonCam-55)->sno));
    }

    //For Saloom Cam 8 of coach 8 & Saloon Cam 1 of coach 9
    if(iSaloonCam == 63) {
        if(coach_8.at(iSaloonCam-56)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit()); //Coach 8 Saloon Camera 8 (index 63 in listSaloonCam)
            playerFrame4();
        }
        if(coach_9.at(iSaloonCam-63)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit()); //Coach 9 Saloon Camera 1 (index 64 in listSaloonCam)
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_8.at(iSaloonCam-56)->csno)+" "+coach_8.at(iSaloonCam-56)->car_type_desc+"_"+coach_8.at(iSaloonCam-56)->camera_type_desc+"_"
                                   +QString::number(coach_8.at(iSaloonCam-56)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_9.at(iSaloonCam-63)->csno)+" "+coach_9.at(iSaloonCam-63)->car_type_desc+"_"+coach_9.at(iSaloonCam-63)->camera_type_desc+"_"
                                   +QString::number(coach_9.at(iSaloonCam-63)->sno));
    }
    //Iterates Saloon Camera 1-7 for coach 9
    if(iSaloonCam >= 64 && iSaloonCam < 71) {
        if(coach_9.at(iSaloonCam-64)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit());
            playerFrame4();
        }
        if(coach_9.at(iSaloonCam-63)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit());
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_9.at(iSaloonCam-64)->csno)+" "+coach_9.at(iSaloonCam-64)->car_type_desc+"_"+coach_9.at(iSaloonCam-64)->camera_type_desc+"_"
                                   +QString::number(coach_9.at(iSaloonCam-64)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_9.at(iSaloonCam-63)->csno)+" "+coach_9.at(iSaloonCam-63)->car_type_desc+"_"+coach_9.at(iSaloonCam-63)->camera_type_desc+"_"
                                   +QString::number(coach_9.at(iSaloonCam-63)->sno));
    }

    //For Saloom Cam 8 of coach 9 & Saloon Cam 1 of coach 10
    if(iSaloonCam == 71) {
        if(coach_9.at(iSaloonCam-64)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit()); //Coach 9 Saloon Camera 8 (index 71 in listSaloonCam)
            playerFrame4();
        }
        if(coach_10.at(iSaloonCam-71)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit()); //Coach 10 Saloon Camera 1 (index 72 in listSaloonCam)
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_9.at(iSaloonCam-64)->csno)+" "+coach_9.at(iSaloonCam-64)->car_type_desc+"_"+coach_9.at(iSaloonCam-64)->camera_type_desc+
                                   "_"+QString::number(coach_9.at(iSaloonCam-64)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_10.at(iSaloonCam-71)->csno)+" "+coach_10.at(iSaloonCam-71)->car_type_desc+"_"+coach_10.at(iSaloonCam-71)->camera_type_desc+
                                   "_"+QString::number(coach_10.at(iSaloonCam-71)->sno));
    }

    //Iterates Saloon Camera 1-7 for coach 10
    if(iSaloonCam >= 72 && iSaloonCam < 79) {
        if(coach_10.at(iSaloonCam-72)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit());
            playerFrame4();
        }
        if(coach_10.at(iSaloonCam-71)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit());
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_10.at(iSaloonCam-72)->csno)+" "+coach_10.at(iSaloonCam-72)->car_type_desc+"_"+coach_10.at(iSaloonCam-72)->camera_type_desc+"_"
                                   +QString::number(coach_10.at(iSaloonCam-72)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_10.at(iSaloonCam-71)->csno)+" "+coach_10.at(iSaloonCam-71)->car_type_desc+"_"+coach_10.at(iSaloonCam-71)->camera_type_desc+"_"
                                   +QString::number(coach_10.at(iSaloonCam-71)->sno));
    }

    //For Saloom Cam 8 of coach 10 & Saloon Cam 1 of coach 11
    if(iSaloonCam == 79) {
        if(coach_10.at(iSaloonCam-72)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit()); //Coach 10 Saloon Camera 8 (index 79 in listSaloonCam)
            playerFrame4();
        }
        if(coach_11.at(iSaloonCam-79)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit()); //Coach 11 Saloon Camera 1 (index 80 in listSaloonCam)
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_10.at(iSaloonCam-72)->csno)+" "+coach_10.at(iSaloonCam-72)->car_type_desc+"_"+coach_10.at(iSaloonCam-72)->camera_type_desc+"_"
                                   +QString::number(coach_10.at(iSaloonCam-72)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_11.at(iSaloonCam-79)->csno)+" "+coach_11.at(iSaloonCam-79)->car_type_desc+"_"+coach_11.at(iSaloonCam-79)->camera_type_desc+"_"
                                   +QString::number(coach_11.at(iSaloonCam-79)->sno));
    }

    //Iterates Saloon Camera 1-7 for coach 11
    if(iSaloonCam >= 80 && iSaloonCam < 87) {
        if(coach_11.at(iSaloonCam-80)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit());
            playerFrame4();
        }
        if(coach_11.at(iSaloonCam-79)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit());
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_11.at(iSaloonCam-80)->csno)+" "+coach_11.at(iSaloonCam-80)->car_type_desc+"_"+coach_11.at(iSaloonCam-80)->camera_type_desc+"_"
                                   +QString::number(coach_11.at(iSaloonCam-80)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_11.at(iSaloonCam-79)->csno)+" "+coach_11.at(iSaloonCam-79)->car_type_desc+"_"+coach_11.at(iSaloonCam-79)->camera_type_desc+"_"
                                   +QString::number(coach_11.at(iSaloonCam-79)->sno));
    }

    //For Saloom Cam 8 of coach 11 & Saloon Cam 1 of coach 12
    if(iSaloonCam == 87) {
        if(coach_11.at(iSaloonCam-80)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit()); //Coach 11 Saloon Camera 8 (index 87 in listSaloonCam)
            playerFrame4();
        }
        if(coach_12.at(iSaloonCam-87)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit()); //Coach 12 Saloon Camera 1 (index 88 in listSaloonCam)
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_11.at(iSaloonCam-80)->csno)+" "+coach_11.at(iSaloonCam-80)->car_type_desc+"_"+coach_11.at(iSaloonCam-80)->camera_type_desc+"_"
                                   +QString::number(coach_11.at(iSaloonCam-80)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_12.at(iSaloonCam-83)->csno)+" "+coach_12.at(iSaloonCam-83)->car_type_desc+"_"+coach_12.at(iSaloonCam-83)->camera_type_desc+"_"
                                   +QString::number(coach_12.at(iSaloonCam-83)->sno));
    }

    //Iterates Saloon Camera 1-7 for coach 12
    if(iSaloonCam >= 88 && iSaloonCam < 95) {
        if(coach_12.at(iSaloonCam-84)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit());
            playerFrame4();
        }
        if(coach_12.at(iSaloonCam-83)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit());
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_12.at(iSaloonCam-84)->csno)+" "+coach_12.at(iSaloonCam-84)->car_type_desc+"_"+coach_12.at(iSaloonCam-84)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iSaloonCam-84)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_12.at(iSaloonCam-83)->csno)+" "+coach_12.at(iSaloonCam-83)->car_type_desc+"_"+coach_12.at(iSaloonCam-83)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iSaloonCam-83)->sno));
    }

    //For Saloom Cam 8 of coach 12 & Saloon Cam 1 of coach 1
    if(iSaloonCam == 95) {
        if(coach_12.at(iSaloonCam-84)->activeStatus==ACTIVE){
            _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit()); //Coach 12 Saloon Camera 8 (index 95 in listSaloonCam)
            playerFrame4();
            _isPlaying=true;
        }
        if(coach_12.at(iSaloonCam-91)->activeStatus==ACTIVE){
            _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit()); //Coach 1 Saloon Camera 1 (index 0 in listSaloonCam)
            playerFrame5();
        }
        ui->label_frame_4->setText("COACH - "+QString::number(coach_12.at(iSaloonCam-84)->csno)+" "+coach_12.at(iSaloonCam-84)->car_type_desc+"_"+coach_12.at(iSaloonCam-84)->camera_type_desc+"_"
                                   +QString::number(coach_12.at(iSaloonCam-84)->sno));
        ui->label_frame_5->setText("COACH - "+QString::number(coach_1.at(iSaloonCam-91)->csno)+" "+coach_1.at(iSaloonCam-91)->car_type_desc+"_"+coach_1.at(iSaloonCam-91)->camera_type_desc+"_"
                                   +QString::number(coach_1.at(iSaloonCam-91)->sno));
    }

}

void MainWindow::on_pushButton_left_down_clicked()
{
    //timer to keep the window active
    timeractive.start();

    //    Stopping player
    libvlc_media_player_stop (_mp4);
    libvlc_media_player_stop (_mp5);
    libvlc_media_player_stop (_mp12);

    if(iSaloonCam==0){
        iSaloonCam = listSaloonCams.length() - 1; //11
    }

    iSaloonCam --;

    iterateSaloonCams();
}

void MainWindow::on_pushButton_right_down_clicked()
{
    //timer to keep the window active
    timeractive.start();

    //    Stopping player
    libvlc_media_player_stop (_mp4);
    libvlc_media_player_stop (_mp5);
    libvlc_media_player_stop (_mp12);

    if(iSaloonCam==listSaloonCams.length() - 2) //10
    {
        iSaloonCam = -1;
    }

    iSaloonCam ++;

    iterateSaloonCams();
}
//=======================================================================

//=======================================================================
//Function for playing rtsp stream in mosiacView _mp6
void MainWindow::playerMosiac1(){
    libvlc_media_player_set_media (_mp6, _m6);
    int windid6 = ui->frame_6->winId();
    libvlc_media_player_set_xwindow (_mp6, windid6 );
    libvlc_media_player_play (_mp6);
    _isPlaying=true;
}
//Function for playing rtsp stream in mosiacView _mp7
void MainWindow::playerMosiac2(){
    libvlc_media_player_set_media (_mp7, _m7);
    int windid7 = ui->frame_7->winId();
    libvlc_media_player_set_xwindow (_mp7, windid7 );
    libvlc_media_player_play (_mp7);
    _isPlaying=true;
}
//Function for playing rtsp stream in mosiacView _mp8
void MainWindow::playerMosiac3(){
    libvlc_media_player_set_media (_mp8, _m8);
    int windid8 = ui->frame_8->winId();
    libvlc_media_player_set_xwindow (_mp8, windid8 );
    libvlc_media_player_play (_mp8);
    _isPlaying=true;
}
//Function for playing rtsp stream in mosiacView _mp9
void MainWindow::playerMosiac4(){
    libvlc_media_player_set_media (_mp9, _m9);
    int windid9 = ui->frame_9->winId();
    libvlc_media_player_set_xwindow (_mp9, windid9 );
    libvlc_media_player_play (_mp9);
    _isPlaying=true;
}

//Function to iterate cameras in the Mosiac Cam View
void MainWindow::iterateMosiacCamView()
{

    //Iterates Saloon Camera 1-8 for coach 1
    if(iMosiacCam >= 0 && iMosiacCam < 5) {

        if(coach_1.at(iMosiacCam+4)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_1.at(iMosiacCam+5)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_1.at(iMosiacCam+6)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_1.at(iMosiacCam+7)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_1.at(iMosiacCam+4)->csno)+" "+coach_1.at(iMosiacCam+4)->car_type_desc+"_"+coach_1.at(iMosiacCam+4)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam+4)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_1.at(iMosiacCam+5)->csno)+" "+coach_1.at(iMosiacCam+5)->car_type_desc+"_"+coach_1.at(iMosiacCam+5)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam+5)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_1.at(iMosiacCam+6)->csno)+" "+coach_1.at(iMosiacCam+6)->car_type_desc+"_"+coach_1.at(iMosiacCam+6)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam+6)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_1.at(iMosiacCam+7)->csno)+" "+coach_1.at(iMosiacCam+7)->car_type_desc+"_"+coach_1.at(iMosiacCam+7)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam+7)->sno));
    }
    //Iterates Saloon Camera 6,7 & 8 for coach 1 and Saloon Camera 1 for coach 2
    else if(iMosiacCam == 5) {

        if(coach_1.at(iMosiacCam+4)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_1.at(iMosiacCam+5)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_1.at(iMosiacCam+6)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_2.at(iMosiacCam-5)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_1.at(iMosiacCam+4)->csno)+" "+coach_1.at(iMosiacCam+4)->car_type_desc+"_"+coach_1.at(iMosiacCam+4)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam+4)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_1.at(iMosiacCam+5)->csno)+" "+coach_1.at(iMosiacCam+5)->car_type_desc+"_"+coach_1.at(iMosiacCam+5)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam+5)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_1.at(iMosiacCam+6)->csno)+" "+coach_1.at(iMosiacCam+6)->car_type_desc+"_"+coach_1.at(iMosiacCam+6)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam+6)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_2.at(iMosiacCam-5)->csno)+" "+coach_2.at(iMosiacCam-5)->car_type_desc+"_"+coach_2.at(iMosiacCam-5)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iMosiacCam-5)->sno));
    }
    //Iterates Saloon Camera 7 & 8 for coach 1 and Saloon Camera 1,2 for coach 2
    else if(iMosiacCam == 6) {

        if(coach_1.at(iMosiacCam+4)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_1.at(iMosiacCam+5)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_2.at(iMosiacCam-6)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_2.at(iMosiacCam-5)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_1.at(iMosiacCam+4)->csno)+" "+coach_1.at(iMosiacCam+4)->car_type_desc+"_"+coach_1.at(iMosiacCam+4)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam+4)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_1.at(iMosiacCam+5)->csno)+" "+coach_1.at(iMosiacCam+5)->car_type_desc+"_"+coach_1.at(iMosiacCam+5)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam+5)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_2.at(iMosiacCam-6)->csno)+" "+coach_2.at(iMosiacCam-6)->car_type_desc+"_"+coach_2.at(iMosiacCam-6)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iMosiacCam-6)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_2.at(iMosiacCam-5)->csno)+" "+coach_2.at(iMosiacCam-5)->car_type_desc+"_"+coach_2.at(iMosiacCam-5)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iMosiacCam-5)->sno));
    }
    //Iterates Saloon Camera 8 for coach 1 and Saloon Camera 1,2 & 3 for coach 2
    else if(iMosiacCam == 7) {

        if(coach_1.at(iMosiacCam+4)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_2.at(iMosiacCam-7)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_2.at(iMosiacCam-6)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_2.at(iMosiacCam-5)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_1.at(iMosiacCam+4)->csno)+" "+coach_1.at(iMosiacCam+4)->car_type_desc+"_"+coach_1.at(iMosiacCam+4)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam+4)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_2.at(iMosiacCam-7)->csno)+" "+coach_2.at(iMosiacCam-7)->car_type_desc+"_"+coach_2.at(iMosiacCam-7)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iMosiacCam-7)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_2.at(iMosiacCam-6)->csno)+" "+coach_2.at(iMosiacCam-6)->car_type_desc+"_"+coach_2.at(iMosiacCam-6)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iMosiacCam-6)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_2.at(iMosiacCam-5)->csno)+" "+coach_2.at(iMosiacCam-5)->car_type_desc+"_"+coach_2.at(iMosiacCam-5)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iMosiacCam-5)->sno));
    }

    //Iterates Saloon Camera 1-8 for coach 2
    else if(iMosiacCam >= 8 && iMosiacCam < 13) {

        if(coach_2.at(iMosiacCam-8)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_2.at(iMosiacCam-7)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_2.at(iMosiacCam-6)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_2.at(iMosiacCam-5)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_2.at(iMosiacCam-8)->csno)+" "+coach_2.at(iMosiacCam-8)->car_type_desc+"_"+coach_2.at(iMosiacCam-8)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iMosiacCam-8)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_2.at(iMosiacCam-7)->csno)+" "+coach_2.at(iMosiacCam-7)->car_type_desc+"_"+coach_2.at(iMosiacCam-7)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iMosiacCam-7)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_2.at(iMosiacCam-6)->csno)+" "+coach_2.at(iMosiacCam-6)->car_type_desc+"_"+coach_2.at(iMosiacCam-6)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iMosiacCam-6)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_2.at(iMosiacCam-5)->csno)+" "+coach_2.at(iMosiacCam-5)->car_type_desc+"_"+coach_2.at(iMosiacCam-5)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iMosiacCam-5)->sno));
    }
    //Iterates Saloon Camera 6,7 & 8 for coach 2 and Saloon Camera 1 for coach 3
    else if(iMosiacCam == 13) {

        if(coach_2.at(iMosiacCam-8)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_2.at(iMosiacCam-7)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_2.at(iMosiacCam-6)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_3.at(iMosiacCam-13)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_2.at(iMosiacCam-8)->csno)+" "+coach_2.at(iMosiacCam-8)->car_type_desc+"_"+coach_2.at(iMosiacCam-8)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iMosiacCam-8)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_2.at(iMosiacCam-7)->csno)+" "+coach_2.at(iMosiacCam-7)->car_type_desc+"_"+coach_2.at(iMosiacCam-7)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iMosiacCam-7)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_2.at(iMosiacCam-6)->csno)+" "+coach_2.at(iMosiacCam-6)->car_type_desc+"_"+coach_2.at(iMosiacCam-6)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iMosiacCam-6)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_3.at(iMosiacCam-13)->csno)+" "+coach_3.at(iMosiacCam-13)->car_type_desc+"_"+coach_3.at(iMosiacCam-13)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iMosiacCam-13)->sno));
    }
    //Iterates Saloon Camera 7 & 8 for coach 2 and Saloon Camera 1,2 for coach 3
    else if(iMosiacCam == 14) {

        if(coach_2.at(iMosiacCam-8)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_2.at(iMosiacCam-7)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_3.at(iMosiacCam-14)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_3.at(iMosiacCam-13)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_2.at(iMosiacCam-8)->csno)+" "+coach_2.at(iMosiacCam-8)->car_type_desc+"_"+coach_2.at(iMosiacCam-8)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iMosiacCam-8)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_2.at(iMosiacCam-7)->csno)+" "+coach_2.at(iMosiacCam-7)->car_type_desc+"_"+coach_2.at(iMosiacCam-7)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iMosiacCam-7)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_3.at(iMosiacCam-14)->csno)+" "+coach_3.at(iMosiacCam-14)->car_type_desc+"_"+coach_3.at(iMosiacCam-14)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iMosiacCam-14)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_3.at(iMosiacCam-13)->csno)+" "+coach_3.at(iMosiacCam-13)->car_type_desc+"_"+coach_3.at(iMosiacCam-13)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iMosiacCam-13)->sno));
    }
    //Iterates Saloon Camera 8 for coach 2 and Saloon Camera 1,2 & 3 for coach 3
    else if(iMosiacCam == 15) {

        if(coach_2.at(iMosiacCam-8)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_3.at(iMosiacCam-15)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_3.at(iMosiacCam-14)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_3.at(iMosiacCam-13)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_2.at(iMosiacCam-8)->csno)+" "+coach_2.at(iMosiacCam-8)->car_type_desc+"_"+coach_2.at(iMosiacCam-8)->camera_type_desc+
                                   "_"+QString::number(coach_2.at(iMosiacCam-8)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_3.at(iMosiacCam-15)->csno)+" "+coach_3.at(iMosiacCam-15)->car_type_desc+"_"+coach_3.at(iMosiacCam-15)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iMosiacCam-15)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_3.at(iMosiacCam-14)->csno)+" "+coach_3.at(iMosiacCam-14)->car_type_desc+"_"+coach_3.at(iMosiacCam-14)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iMosiacCam-14)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_3.at(iMosiacCam-13)->csno)+" "+coach_3.at(iMosiacCam-13)->car_type_desc+"_"+coach_3.at(iMosiacCam-13)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iMosiacCam-13)->sno));
    }

    //Iterates Saloon Camera 1-8 for coach 3
    else if(iMosiacCam >= 16 && iMosiacCam < 21) {

        if(coach_3.at(iMosiacCam-16)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_3.at(iMosiacCam-15)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_3.at(iMosiacCam-14)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_3.at(iMosiacCam-13)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_3.at(iMosiacCam-16)->csno)+" "+coach_3.at(iMosiacCam-16)->car_type_desc+"_"+coach_3.at(iMosiacCam-16)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iMosiacCam-16)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_3.at(iMosiacCam-15)->csno)+" "+coach_3.at(iMosiacCam-15)->car_type_desc+"_"+coach_3.at(iMosiacCam-15)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iMosiacCam-15)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_3.at(iMosiacCam-14)->csno)+" "+coach_3.at(iMosiacCam-14)->car_type_desc+"_"+coach_3.at(iMosiacCam-14)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iMosiacCam-14)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_3.at(iMosiacCam-13)->csno)+" "+coach_3.at(iMosiacCam-13)->car_type_desc+"_"+coach_3.at(iMosiacCam-13)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iMosiacCam-13)->sno));
    }
    //Iterates Saloon Camera 6,7 & 8 for coach 3 and Saloon Camera 1 for coach 4
    else if(iMosiacCam == 21) {

        if(coach_3.at(iMosiacCam-16)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_3.at(iMosiacCam-15)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_3.at(iMosiacCam-14)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_4.at(iMosiacCam-21)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_3.at(iMosiacCam-16)->csno)+" "+coach_3.at(iMosiacCam-16)->car_type_desc+"_"+coach_3.at(iMosiacCam-16)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iMosiacCam-16)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_3.at(iMosiacCam-15)->csno)+" "+coach_3.at(iMosiacCam-15)->car_type_desc+"_"+coach_3.at(iMosiacCam-15)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iMosiacCam-15)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_3.at(iMosiacCam-14)->csno)+" "+coach_3.at(iMosiacCam-14)->car_type_desc+"_"+coach_3.at(iMosiacCam-14)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iMosiacCam-14)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_4.at(iMosiacCam-21)->csno)+" "+coach_4.at(iMosiacCam-21)->car_type_desc+"_"+coach_4.at(iMosiacCam-21)->camera_type_desc+
                                   "_"+QString::number(coach_4.at(iMosiacCam-21)->sno));
    }
    //Iterates Saloon Camera 7 & 8 for coach 3 and Saloon Camera 1,2 for coach 4
    else if(iMosiacCam == 22) {

        if(coach_3.at(iMosiacCam-16)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_3.at(iMosiacCam-15)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_4.at(iMosiacCam-22)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_4.at(iMosiacCam-21)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_3.at(iMosiacCam-16)->csno)+" "+coach_3.at(iMosiacCam-16)->car_type_desc+"_"+coach_3.at(iMosiacCam-16)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iMosiacCam-16)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_3.at(iMosiacCam-15)->csno)+" "+coach_3.at(iMosiacCam-15)->car_type_desc+"_"+coach_3.at(iMosiacCam-15)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iMosiacCam-15)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_4.at(iMosiacCam-22)->csno)+" "+coach_4.at(iMosiacCam-22)->car_type_desc+"_"+coach_4.at(iMosiacCam-22)->camera_type_desc+
                                   "_"+QString::number(coach_4.at(iMosiacCam-22)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_4.at(iMosiacCam-21)->csno)+" "+coach_4.at(iMosiacCam-21)->car_type_desc+"_"+coach_4.at(iMosiacCam-21)->camera_type_desc+
                                   "_"+QString::number(coach_4.at(iMosiacCam-21)->sno));
    }
    //Iterates Saloon Camera 8 for coach 3 and Saloon Camera 1,2 & 3 for coach 4
    else if(iMosiacCam == 23) {

        if(coach_3.at(iMosiacCam-16)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_4.at(iMosiacCam-23)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_4.at(iMosiacCam-22)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_4.at(iMosiacCam-21)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_3.at(iMosiacCam-16)->csno)+" "+coach_3.at(iMosiacCam-16)->car_type_desc+"_"+coach_3.at(iMosiacCam-16)->camera_type_desc+
                                   "_"+QString::number(coach_3.at(iMosiacCam-16)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_4.at(iMosiacCam-23)->csno)+" "+coach_4.at(iMosiacCam-23)->car_type_desc+"_"+coach_4.at(iMosiacCam-23)->camera_type_desc+
                                   "_"+QString::number(coach_4.at(iMosiacCam-23)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_4.at(iMosiacCam-22)->csno)+" "+coach_4.at(iMosiacCam-22)->car_type_desc+"_"+coach_4.at(iMosiacCam-22)->camera_type_desc+
                                   "_"+QString::number(coach_4.at(iMosiacCam-22)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_4.at(iMosiacCam-21)->csno)+" "+coach_4.at(iMosiacCam-21)->car_type_desc+"_"+coach_4.at(iMosiacCam-21)->camera_type_desc+
                                   "_"+QString::number(coach_4.at(iMosiacCam-21)->sno));
    }

    //Iterates Saloon Camera 1-8 for coach 4
    else if(iMosiacCam >= 24 && iMosiacCam < 29) {

        if(coach_4.at(iMosiacCam-24)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_4.at(iMosiacCam-23)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_4.at(iMosiacCam-22)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_4.at(iMosiacCam-21)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_4.at(iMosiacCam-24)->csno)+" "+coach_4.at(iMosiacCam-24)->car_type_desc+"_"+coach_4.at(iMosiacCam-24)->camera_type_desc+
                                   "_"+QString::number(coach_4.at(iMosiacCam-24)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_4.at(iMosiacCam-23)->csno)+" "+coach_4.at(iMosiacCam-23)->car_type_desc+"_"+coach_4.at(iMosiacCam-23)->camera_type_desc+
                                   "_"+QString::number(coach_4.at(iMosiacCam-23)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_4.at(iMosiacCam-22)->csno)+" "+coach_4.at(iMosiacCam-22)->car_type_desc+"_"+coach_4.at(iMosiacCam-22)->camera_type_desc+
                                   "_"+QString::number(coach_4.at(iMosiacCam-22)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_4.at(iMosiacCam-21)->csno)+" "+coach_4.at(iMosiacCam-21)->car_type_desc+"_"+coach_4.at(iMosiacCam-21)->camera_type_desc+
                                   "_"+QString::number(coach_4.at(iMosiacCam-21)->sno));
    }
    //Iterates Saloon Camera 6,7 & 8 for coach 4 and Saloon Camera 1 for coach 5
    else if(iMosiacCam == 29) {

        if(coach_4.at(iMosiacCam-24)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_4.at(iMosiacCam-23)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_4.at(iMosiacCam-22)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_5.at(iMosiacCam-29)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_4.at(iMosiacCam-24)->csno)+" "+coach_4.at(iMosiacCam-24)->car_type_desc+"_"+coach_4.at(iMosiacCam-24)->camera_type_desc+
                                   "_"+QString::number(coach_4.at(iMosiacCam-24)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_4.at(iMosiacCam-23)->csno)+" "+coach_4.at(iMosiacCam-23)->car_type_desc+"_"+coach_4.at(iMosiacCam-23)->camera_type_desc+
                                   "_"+QString::number(coach_4.at(iMosiacCam-23)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_4.at(iMosiacCam-22)->csno)+" "+coach_4.at(iMosiacCam-22)->car_type_desc+"_"+coach_4.at(iMosiacCam-22)->camera_type_desc+
                                   "_"+QString::number(coach_4.at(iMosiacCam-22)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_5.at(iMosiacCam-29)->csno)+" "+coach_5.at(iMosiacCam-29)->car_type_desc+"_"+coach_5.at(iMosiacCam-29)->camera_type_desc+
                                   "_"+QString::number(coach_5.at(iMosiacCam-29)->sno));
    }
    //Iterates Saloon Camera 7 & 8 for coach 4 and Saloon Camera 1,2 for coach 5
    else if(iMosiacCam == 30) {

        if(coach_4.at(iMosiacCam-24)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_4.at(iMosiacCam-23)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_5.at(iMosiacCam-30)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_5.at(iMosiacCam-29)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_4.at(iMosiacCam-24)->csno)+" "+coach_4.at(iMosiacCam-24)->car_type_desc+"_"+coach_4.at(iMosiacCam-24)->camera_type_desc+
                                   "_"+QString::number(coach_4.at(iMosiacCam-24)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_4.at(iMosiacCam-23)->csno)+" "+coach_4.at(iMosiacCam-23)->car_type_desc+"_"+coach_4.at(iMosiacCam-23)->camera_type_desc+
                                   "_"+QString::number(coach_4.at(iMosiacCam-23)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_5.at(iMosiacCam-30)->csno)+" "+coach_5.at(iMosiacCam-30)->car_type_desc+"_"+coach_5.at(iMosiacCam-30)->camera_type_desc+
                                   "_"+QString::number(coach_5.at(iMosiacCam-30)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_5.at(iMosiacCam-29)->csno)+" "+coach_5.at(iMosiacCam-29)->car_type_desc+"_"+coach_5.at(iMosiacCam-29)->camera_type_desc+
                                   "_"+QString::number(coach_5.at(iMosiacCam-29)->sno));
    }
    //Iterates Saloon Camera 8 for coach 4 and Saloon Camera 1,2 & 3 for coach 5
    else if(iMosiacCam == 31) {

        if(coach_4.at(iMosiacCam-24)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_5.at(iMosiacCam-31)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_5.at(iMosiacCam-30)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_5.at(iMosiacCam-29)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_4.at(iMosiacCam-24)->csno)+" "+coach_4.at(iMosiacCam-24)->car_type_desc+"_"+coach_4.at(iMosiacCam-24)->camera_type_desc+
                                   "_"+QString::number(coach_4.at(iMosiacCam-24)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_5.at(iMosiacCam-31)->csno)+" "+coach_5.at(iMosiacCam-31)->car_type_desc+"_"+coach_5.at(iMosiacCam-31)->camera_type_desc+
                                   "_"+QString::number(coach_5.at(iMosiacCam-31)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_5.at(iMosiacCam-30)->csno)+" "+coach_5.at(iMosiacCam-30)->car_type_desc+"_"+coach_5.at(iMosiacCam-30)->camera_type_desc+
                                   "_"+QString::number(coach_5.at(iMosiacCam-30)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_5.at(iMosiacCam-29)->csno)+" "+coach_5.at(iMosiacCam-29)->car_type_desc+"_"+coach_5.at(iMosiacCam-29)->camera_type_desc+
                                   "_"+QString::number(coach_5.at(iMosiacCam-29)->sno));
    }
    //Iterates Saloon Camera 1-8 for coach 5
    else if(iMosiacCam >= 32 && iMosiacCam < 37) {

        if(coach_5.at(iMosiacCam-32)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_5.at(iMosiacCam-31)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_5.at(iMosiacCam-30)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_5.at(iMosiacCam-29)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_5.at(iMosiacCam-32)->csno)+" "+coach_5.at(iMosiacCam-32)->car_type_desc+"_"+coach_5.at(iMosiacCam-32)->camera_type_desc+
                                   "_"+QString::number(coach_5.at(iMosiacCam-32)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_5.at(iMosiacCam-31)->csno)+" "+coach_5.at(iMosiacCam-31)->car_type_desc+"_"+coach_5.at(iMosiacCam-31)->camera_type_desc+
                                   "_"+QString::number(coach_5.at(iMosiacCam-31)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_5.at(iMosiacCam-30)->csno)+" "+coach_5.at(iMosiacCam-30)->car_type_desc+"_"+coach_5.at(iMosiacCam-30)->camera_type_desc+
                                   "_"+QString::number(coach_5.at(iMosiacCam-30)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_5.at(iMosiacCam-29)->csno)+" "+coach_5.at(iMosiacCam-29)->car_type_desc+"_"+coach_5.at(iMosiacCam-29)->camera_type_desc+
                                   "_"+QString::number(coach_5.at(iMosiacCam-29)->sno));
    }
    //Iterates Saloon Camera 6,7 & 8 for coach 5 and Saloon Camera 1 for coach 6
    else if(iMosiacCam == 37) {

        if(coach_5.at(iMosiacCam-32)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_5.at(iMosiacCam-31)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_5.at(iMosiacCam-30)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_6.at(iMosiacCam-37)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_5.at(iMosiacCam-32)->csno)+" "+coach_5.at(iMosiacCam-32)->car_type_desc+"_"+coach_5.at(iMosiacCam-32)->camera_type_desc+
                                   "_"+QString::number(coach_5.at(iMosiacCam-32)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_5.at(iMosiacCam-31)->csno)+" "+coach_5.at(iMosiacCam-31)->car_type_desc+"_"+coach_5.at(iMosiacCam-31)->camera_type_desc+
                                   "_"+QString::number(coach_5.at(iMosiacCam-31)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_5.at(iMosiacCam-30)->csno)+" "+coach_5.at(iMosiacCam-30)->car_type_desc+"_"+coach_5.at(iMosiacCam-30)->camera_type_desc+
                                   "_"+QString::number(coach_5.at(iMosiacCam-30)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_6.at(iMosiacCam-37)->csno)+" "+coach_6.at(iMosiacCam-37)->car_type_desc+"_"+coach_6.at(iMosiacCam-37)->camera_type_desc+
                                   "_"+QString::number(coach_6.at(iMosiacCam-37)->sno));
    }
    //Iterates Saloon Camera 7 & 8 for coach 5 and Saloon Camera 1,2 for coach 6
    else if(iMosiacCam == 38) {

        if(coach_5.at(iMosiacCam-32)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_5.at(iMosiacCam-31)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_6.at(iMosiacCam-38)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_6.at(iMosiacCam-37)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_5.at(iMosiacCam-32)->csno)+" "+coach_5.at(iMosiacCam-32)->car_type_desc+"_"+coach_5.at(iMosiacCam-32)->camera_type_desc+
                                   "_"+QString::number(coach_5.at(iMosiacCam-32)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_5.at(iMosiacCam-31)->csno)+" "+coach_5.at(iMosiacCam-31)->car_type_desc+"_"+coach_5.at(iMosiacCam-31)->camera_type_desc+
                                   "_"+QString::number(coach_5.at(iMosiacCam-31)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_6.at(iMosiacCam-38)->csno)+" "+coach_6.at(iMosiacCam-38)->car_type_desc+"_"+coach_6.at(iMosiacCam-38)->camera_type_desc+
                                   "_"+QString::number(coach_6.at(iMosiacCam-38)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_6.at(iMosiacCam-37)->csno)+" "+coach_6.at(iMosiacCam-37)->car_type_desc+"_"+coach_6.at(iMosiacCam-37)->camera_type_desc+
                                   "_"+QString::number(coach_6.at(iMosiacCam-37)->sno));
    }
    //Iterates Saloon Camera 8 for coach 5 and Saloon Camera 1,2 & 3 for coach 6
    else if(iMosiacCam == 39) {

        if(coach_5.at(iMosiacCam-32)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_6.at(iMosiacCam-39)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_6.at(iMosiacCam-38)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_6.at(iMosiacCam-37)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_5.at(iMosiacCam-32)->csno)+" "+coach_5.at(iMosiacCam-32)->car_type_desc+"_"+coach_5.at(iMosiacCam-32)->camera_type_desc+
                                   "_"+QString::number(coach_5.at(iMosiacCam-32)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_6.at(iMosiacCam-39)->csno)+" "+coach_6.at(iMosiacCam-39)->car_type_desc+"_"+coach_6.at(iMosiacCam-39)->camera_type_desc+
                                   "_"+QString::number(coach_6.at(iMosiacCam-39)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_6.at(iMosiacCam-38)->csno)+" "+coach_6.at(iMosiacCam-38)->car_type_desc+"_"+coach_6.at(iMosiacCam-38)->camera_type_desc+
                                   "_"+QString::number(coach_6.at(iMosiacCam-38)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_6.at(iMosiacCam-37)->csno)+" "+coach_6.at(iMosiacCam-37)->car_type_desc+"_"+coach_6.at(iMosiacCam-37)->camera_type_desc+
                                   "_"+QString::number(coach_6.at(iMosiacCam-37)->sno));
    }
    //Iterates Saloon Camera 1-8 for coach 6
    else if(iMosiacCam >= 40 && iMosiacCam < 45) {

        if(coach_6.at(iMosiacCam-40)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_6.at(iMosiacCam-39)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_6.at(iMosiacCam-38)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_6.at(iMosiacCam-37)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_6.at(iMosiacCam-40)->csno)+" "+coach_6.at(iMosiacCam-40)->car_type_desc+"_"+coach_6.at(iMosiacCam-40)->camera_type_desc+
                                   "_"+QString::number(coach_6.at(iMosiacCam-40)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_6.at(iMosiacCam-39)->csno)+" "+coach_6.at(iMosiacCam-39)->car_type_desc+"_"+coach_6.at(iMosiacCam-39)->camera_type_desc+
                                   "_"+QString::number(coach_6.at(iMosiacCam-39)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_6.at(iMosiacCam-38)->csno)+" "+coach_6.at(iMosiacCam-38)->car_type_desc+"_"+coach_6.at(iMosiacCam-38)->camera_type_desc+
                                   "_"+QString::number(coach_6.at(iMosiacCam-38)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_6.at(iMosiacCam-37)->csno)+" "+coach_6.at(iMosiacCam-37)->car_type_desc+"_"+coach_6.at(iMosiacCam-37)->camera_type_desc+
                                   "_"+QString::number(coach_6.at(iMosiacCam-37)->sno));
    }
    //Iterates Saloon Camera 6,7 & 8 for coach 6 and Saloon Camera 1 for coach 7
    else if(iMosiacCam == 45) {

        if(coach_6.at(iMosiacCam-40)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_6.at(iMosiacCam-39)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_6.at(iMosiacCam-38)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_7.at(iMosiacCam-45)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_6.at(iMosiacCam-40)->csno)+" "+coach_6.at(iMosiacCam-40)->car_type_desc+"_"+coach_6.at(iMosiacCam-40)->camera_type_desc+
                                   "_"+QString::number(coach_6.at(iMosiacCam-40)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_6.at(iMosiacCam-39)->csno)+" "+coach_6.at(iMosiacCam-39)->car_type_desc+"_"+coach_6.at(iMosiacCam-39)->camera_type_desc+
                                   "_"+QString::number(coach_6.at(iMosiacCam-39)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_6.at(iMosiacCam-38)->csno)+" "+coach_6.at(iMosiacCam-38)->car_type_desc+"_"+coach_6.at(iMosiacCam-38)->camera_type_desc+
                                   "_"+QString::number(coach_6.at(iMosiacCam-38)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_7.at(iMosiacCam-45)->csno)+" "+coach_7.at(iMosiacCam-45)->car_type_desc+"_"+coach_7.at(iMosiacCam-45)->camera_type_desc+
                                   "_"+QString::number(coach_7.at(iMosiacCam-45)->sno));
    }
    //Iterates Saloon Camera 7 & 8 for coach 6 and Saloon Camera 1,2 for coach 7
    else if(iMosiacCam == 46) {
        if(coach_6.at(iMosiacCam-40)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_6.at(iMosiacCam-39)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_7.at(iMosiacCam-46)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_7.at(iMosiacCam-45)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_6.at(iMosiacCam-40)->csno)+" "+coach_6.at(iMosiacCam-40)->car_type_desc+"_"+coach_6.at(iMosiacCam-40)->camera_type_desc+
                                   "_"+QString::number(coach_6.at(iMosiacCam-40)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_6.at(iMosiacCam-39)->csno)+" "+coach_6.at(iMosiacCam-39)->car_type_desc+"_"+coach_6.at(iMosiacCam-39)->camera_type_desc+
                                   "_"+QString::number(coach_6.at(iMosiacCam-39)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_7.at(iMosiacCam-46)->csno)+" "+coach_7.at(iMosiacCam-46)->car_type_desc+"_"+coach_7.at(iMosiacCam-46)->camera_type_desc+
                                   "_"+QString::number(coach_7.at(iMosiacCam-46)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_7.at(iMosiacCam-45)->csno)+" "+coach_7.at(iMosiacCam-45)->car_type_desc+"_"+coach_7.at(iMosiacCam-45)->camera_type_desc+
                                   "_"+QString::number(coach_7.at(iMosiacCam-45)->sno));
    }
    //Iterates Saloon Camera 8 for coach 6 and Saloon Camera 1,2 & 3 for coach 7
    else if(iMosiacCam == 47) {

        if(coach_6.at(iMosiacCam-40)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_7.at(iMosiacCam-47)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_7.at(iMosiacCam-46)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_7.at(iMosiacCam-45)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_6.at(iMosiacCam-40)->csno)+" "+coach_6.at(iMosiacCam-40)->car_type_desc+"_"+coach_6.at(iMosiacCam-40)->camera_type_desc+
                                   "_"+QString::number(coach_6.at(iMosiacCam-40)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_7.at(iMosiacCam-47)->csno)+" "+coach_7.at(iMosiacCam-47)->car_type_desc+"_"+coach_7.at(iMosiacCam-47)->camera_type_desc+
                                   "_"+QString::number(coach_7.at(iMosiacCam-47)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_7.at(iMosiacCam-46)->csno)+" "+coach_7.at(iMosiacCam-46)->car_type_desc+"_"+coach_7.at(iMosiacCam-46)->camera_type_desc+
                                   "_"+QString::number(coach_7.at(iMosiacCam-46)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_7.at(iMosiacCam-45)->csno)+" "+coach_7.at(iMosiacCam-45)->car_type_desc+"_"+coach_7.at(iMosiacCam-45)->camera_type_desc+
                                   "_"+QString::number(coach_7.at(iMosiacCam-45)->sno));
    }

    //Iterates Saloon Camera 1-8 for coach 7
    else if(iMosiacCam >= 48 && iMosiacCam < 53) {

        if(coach_7.at(iMosiacCam-48)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_7.at(iMosiacCam-47)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_7.at(iMosiacCam-46)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_7.at(iMosiacCam-45)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_7.at(iMosiacCam-48)->csno)+" "+coach_7.at(iMosiacCam-48)->car_type_desc+"_"+coach_7.at(iMosiacCam-48)->camera_type_desc+
                                   "_"+QString::number(coach_7.at(iMosiacCam-48)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_7.at(iMosiacCam-47)->csno)+" "+coach_7.at(iMosiacCam-47)->car_type_desc+"_"+coach_7.at(iMosiacCam-47)->camera_type_desc+
                                   "_"+QString::number(coach_7.at(iMosiacCam-47)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_7.at(iMosiacCam-46)->csno)+" "+coach_7.at(iMosiacCam-46)->car_type_desc+"_"+coach_7.at(iMosiacCam-46)->camera_type_desc+
                                   "_"+QString::number(coach_7.at(iMosiacCam-46)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_7.at(iMosiacCam-45)->csno)+" "+coach_7.at(iMosiacCam-45)->car_type_desc+"_"+coach_7.at(iMosiacCam-45)->camera_type_desc+
                                   "_"+QString::number(coach_7.at(iMosiacCam-45)->sno));
    }
    //Iterates Saloon Camera 6,7 & 8 for coach 7 and Saloon Camera 1 for coach 8
    else if(iMosiacCam == 53) {

        if(coach_7.at(iMosiacCam-48)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_7.at(iMosiacCam-47)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_7.at(iMosiacCam-46)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_8.at(iMosiacCam-53)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_7.at(iMosiacCam-48)->csno)+" "+coach_7.at(iMosiacCam-48)->car_type_desc+"_"+coach_7.at(iMosiacCam-48)->camera_type_desc+
                                   "_"+QString::number(coach_7.at(iMosiacCam-48)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_7.at(iMosiacCam-47)->csno)+" "+coach_7.at(iMosiacCam-47)->car_type_desc+"_"+coach_7.at(iMosiacCam-47)->camera_type_desc+
                                   "_"+QString::number(coach_7.at(iMosiacCam-47)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_7.at(iMosiacCam-46)->csno)+" "+coach_7.at(iMosiacCam-46)->car_type_desc+"_"+coach_7.at(iMosiacCam-46)->camera_type_desc+
                                   "_"+QString::number(coach_7.at(iMosiacCam-46)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_8.at(iMosiacCam-53)->csno)+" "+coach_8.at(iMosiacCam-53)->car_type_desc+"_"+coach_8.at(iMosiacCam-53)->camera_type_desc+
                                   "_"+QString::number(coach_8.at(iMosiacCam-53)->sno));
    }
    //Iterates Saloon Camera 7 & 8 for coach 6 and Saloon Camera 1,2 for coach 7
    else if(iMosiacCam == 54) {
        if(coach_7.at(iMosiacCam-48)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_7.at(iMosiacCam-47)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_8.at(iMosiacCam-54)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_8.at(iMosiacCam-53)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_7.at(iMosiacCam-48)->csno)+" "+coach_7.at(iMosiacCam-48)->car_type_desc+"_"+coach_7.at(iMosiacCam-48)->camera_type_desc+
                                   "_"+QString::number(coach_7.at(iMosiacCam-48)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_7.at(iMosiacCam-47)->csno)+" "+coach_7.at(iMosiacCam-47)->car_type_desc+"_"+coach_7.at(iMosiacCam-47)->camera_type_desc+
                                   "_"+QString::number(coach_7.at(iMosiacCam-47)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_8.at(iMosiacCam-54)->csno)+" "+coach_8.at(iMosiacCam-54)->car_type_desc+"_"+coach_8.at(iMosiacCam-54)->camera_type_desc+
                                   "_"+QString::number(coach_8.at(iMosiacCam-54)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_8.at(iMosiacCam-53)->csno)+" "+coach_8.at(iMosiacCam-53)->car_type_desc+"_"+coach_8.at(iMosiacCam-53)->camera_type_desc+
                                   "_"+QString::number(coach_8.at(iMosiacCam-53)->sno));
    }
    //Iterates Saloon Camera 8 for coach 6 and Saloon Camera 1,2 & 3 for coach 7
    else if(iMosiacCam == 55) {

        if(coach_7.at(iMosiacCam-48)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_8.at(iMosiacCam-55)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_8.at(iMosiacCam-54)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_8.at(iMosiacCam-53)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_7.at(iMosiacCam-48)->csno)+" "+coach_7.at(iMosiacCam-48)->car_type_desc+"_"+coach_7.at(iMosiacCam-48)->camera_type_desc+
                                   "_"+QString::number(coach_7.at(iMosiacCam-48)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_8.at(iMosiacCam-55)->csno)+" "+coach_8.at(iMosiacCam-55)->car_type_desc+"_"+coach_8.at(iMosiacCam-55)->camera_type_desc+
                                   "_"+QString::number(coach_8.at(iMosiacCam-55)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_8.at(iMosiacCam-54)->csno)+" "+coach_8.at(iMosiacCam-54)->car_type_desc+"_"+coach_8.at(iMosiacCam-54)->camera_type_desc+
                                   "_"+QString::number(coach_8.at(iMosiacCam-54)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_8.at(iMosiacCam-53)->csno)+" "+coach_8.at(iMosiacCam-53)->car_type_desc+"_"+coach_8.at(iMosiacCam-53)->camera_type_desc+
                                   "_"+QString::number(coach_8.at(iMosiacCam-53)->sno));
    }
    //Iterates Saloon Camera 1-8 for coach 8
    else if(iMosiacCam >= 56 && iMosiacCam < 61) {

        if(coach_8.at(iMosiacCam-56)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_8.at(iMosiacCam-55)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_8.at(iMosiacCam-54)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_8.at(iMosiacCam-53)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_8.at(iMosiacCam-56)->csno)+" "+coach_8.at(iMosiacCam-56)->car_type_desc+"_"+coach_8.at(iMosiacCam-56)->camera_type_desc+
                                   "_"+QString::number(coach_8.at(iMosiacCam-56)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_8.at(iMosiacCam-55)->csno)+" "+coach_8.at(iMosiacCam-55)->car_type_desc+"_"+coach_8.at(iMosiacCam-55)->camera_type_desc+
                                   "_"+QString::number(coach_8.at(iMosiacCam-55)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_8.at(iMosiacCam-54)->csno)+" "+coach_8.at(iMosiacCam-54)->car_type_desc+"_"+coach_8.at(iMosiacCam-54)->camera_type_desc+
                                   "_"+QString::number(coach_8.at(iMosiacCam-54)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_8.at(iMosiacCam-53)->csno)+" "+coach_8.at(iMosiacCam-53)->car_type_desc+"_"+coach_8.at(iMosiacCam-53)->camera_type_desc+
                                   "_"+QString::number(coach_8.at(iMosiacCam-53)->sno));
    }
    //Iterates Saloon Camera 6,7 & 8 for coach 8 and Saloon Camera 1 for coach 9
    else if(iMosiacCam == 61) {

        if(coach_8.at(iMosiacCam-56)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_8.at(iMosiacCam-55)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_8.at(iMosiacCam-54)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_9.at(iMosiacCam-61)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_8.at(iMosiacCam-56)->csno)+" "+coach_8.at(iMosiacCam-56)->car_type_desc+"_"+coach_8.at(iMosiacCam-56)->camera_type_desc+
                                   "_"+QString::number(coach_8.at(iMosiacCam-56)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_8.at(iMosiacCam-55)->csno)+" "+coach_8.at(iMosiacCam-55)->car_type_desc+"_"+coach_8.at(iMosiacCam-55)->camera_type_desc+
                                   "_"+QString::number(coach_8.at(iMosiacCam-55)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_8.at(iMosiacCam-54)->csno)+" "+coach_8.at(iMosiacCam-54)->car_type_desc+"_"+coach_8.at(iMosiacCam-54)->camera_type_desc+
                                   "_"+QString::number(coach_8.at(iMosiacCam-54)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-61)->csno)+" "+coach_9.at(iMosiacCam-61)->car_type_desc+"_"+coach_9.at(iMosiacCam-61)->camera_type_desc+
                                   "_"+QString::number(coach_9.at(iMosiacCam-61)->sno));
    }
    //Iterates Saloon Camera 7 & 8 for coach 8 and Saloon Camera 1,2 for coach 9
    else if(iMosiacCam == 62) {
        if(coach_8.at(iMosiacCam-56)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_8.at(iMosiacCam-55)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_9.at(iMosiacCam-62)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_9.at(iMosiacCam-61)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_8.at(iMosiacCam-56)->csno)+" "+coach_8.at(iMosiacCam-56)->car_type_desc+"_"+coach_8.at(iMosiacCam-56)->camera_type_desc+
                                   "_"+QString::number(coach_8.at(iMosiacCam-56)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_8.at(iMosiacCam-55)->csno)+" "+coach_8.at(iMosiacCam-55)->car_type_desc+"_"+coach_8.at(iMosiacCam-55)->camera_type_desc+
                                   "_"+QString::number(coach_8.at(iMosiacCam-55)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-62)->csno)+" "+coach_9.at(iMosiacCam-62)->car_type_desc+"_"+coach_9.at(iMosiacCam-62)->camera_type_desc+
                                   "_"+QString::number(coach_9.at(iMosiacCam-62)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-61)->csno)+" "+coach_9.at(iMosiacCam-61)->car_type_desc+"_"+coach_9.at(iMosiacCam-61)->camera_type_desc+
                                   "_"+QString::number(coach_9.at(iMosiacCam-61)->sno));
    }
    //Iterates Saloon Camera 8 for coach 8 and Saloon Camera 1,2 & 3 for coach 9
    else if(iMosiacCam == 63) {

        if(coach_8.at(iMosiacCam-56)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_9.at(iMosiacCam-63)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_9.at(iMosiacCam-62)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_9.at(iMosiacCam-61)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_8.at(iMosiacCam-56)->csno)+" "+coach_8.at(iMosiacCam-56)->car_type_desc+"_"+coach_8.at(iMosiacCam-56)->camera_type_desc+
                                   "_"+QString::number(coach_8.at(iMosiacCam-56)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-63)->csno)+" "+coach_9.at(iMosiacCam-63)->car_type_desc+"_"+coach_9.at(iMosiacCam-63)->camera_type_desc+
                                   "_"+QString::number(coach_9.at(iMosiacCam-63)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-62)->csno)+" "+coach_9.at(iMosiacCam-62)->car_type_desc+"_"+coach_9.at(iMosiacCam-62)->camera_type_desc+
                                   "_"+QString::number(coach_9.at(iMosiacCam-62)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-61)->csno)+" "+coach_9.at(iMosiacCam-61)->car_type_desc+"_"+coach_9.at(iMosiacCam-61)->camera_type_desc+
                                   "_"+QString::number(coach_9.at(iMosiacCam-61)->sno));
    }
    //Iterates Saloon Camera 1-8 for coach 9
    else if(iMosiacCam >= 64 && iMosiacCam < 69) {

        if(coach_9.at(iMosiacCam-64)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_9.at(iMosiacCam-63)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_9.at(iMosiacCam-62)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_9.at(iMosiacCam-61)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-64)->csno)+" "+coach_9.at(iMosiacCam-64)->car_type_desc+"_"+coach_9.at(iMosiacCam-64)->camera_type_desc+
                                   "_"+QString::number(coach_9.at(iMosiacCam-64)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-63)->csno)+" "+coach_9.at(iMosiacCam-63)->car_type_desc+"_"+coach_9.at(iMosiacCam-63)->camera_type_desc+
                                   "_"+QString::number(coach_9.at(iMosiacCam-63)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-62)->csno)+" "+coach_9.at(iMosiacCam-62)->car_type_desc+"_"+coach_9.at(iMosiacCam-62)->camera_type_desc+
                                   "_"+QString::number(coach_9.at(iMosiacCam-62)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-61)->csno)+" "+coach_9.at(iMosiacCam-61)->car_type_desc+"_"+coach_9.at(iMosiacCam-61)->camera_type_desc+
                                   "_"+QString::number(coach_9.at(iMosiacCam-61)->sno));
    }
    //Iterates Saloon Camera 6,7 & 8 for coach 9 and Saloon Camera 1 for coach 10
    else if(iMosiacCam == 69) {

        if(coach_9.at(iMosiacCam-64)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_9.at(iMosiacCam-63)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_9.at(iMosiacCam-62)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_10.at(iMosiacCam-69)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-64)->csno)+" "+coach_9.at(iMosiacCam-64)->car_type_desc+"_"+coach_9.at(iMosiacCam-64)->camera_type_desc+
                                   "_"+QString::number(coach_9.at(iMosiacCam-64)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-63)->csno)+" "+coach_9.at(iMosiacCam-63)->car_type_desc+"_"+coach_9.at(iMosiacCam-63)->camera_type_desc+
                                   "_"+QString::number(coach_9.at(iMosiacCam-63)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-62)->csno)+" "+coach_9.at(iMosiacCam-62)->car_type_desc+"_"+coach_9.at(iMosiacCam-62)->camera_type_desc+
                                   "_"+QString::number(coach_9.at(iMosiacCam-62)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_10.at(iMosiacCam-69)->csno)+" "+coach_10.at(iMosiacCam-69)->car_type_desc+"_"+coach_10.at(iMosiacCam-69)->camera_type_desc+
                                   "_"+QString::number(coach_10.at(iMosiacCam-69)->sno));
    }
    //Iterates Saloon Camera 7 & 8 for coach 9 and Saloon Camera 1,2 for coach 10
    else if(iMosiacCam == 70) {
        if(coach_9.at(iMosiacCam-64)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_9.at(iMosiacCam-63)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_10.at(iMosiacCam-70)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_10.at(iMosiacCam-69)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-64)->csno)+" "+coach_9.at(iMosiacCam-64)->car_type_desc+"_"+coach_9.at(iMosiacCam-64)->camera_type_desc+
                                   "_"+QString::number(coach_9.at(iMosiacCam-64)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-63)->csno)+" "+coach_9.at(iMosiacCam-63)->car_type_desc+"_"+coach_9.at(iMosiacCam-63)->camera_type_desc+
                                   "_"+QString::number(coach_9.at(iMosiacCam-63)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_10.at(iMosiacCam-70)->csno)+" "+coach_10.at(iMosiacCam-70)->car_type_desc+"_"+coach_10.at(iMosiacCam-70)->camera_type_desc+
                                   "_"+QString::number(coach_10.at(iMosiacCam-70)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_10.at(iMosiacCam-69)->csno)+" "+coach_10.at(iMosiacCam-69)->car_type_desc+"_"+coach_10.at(iMosiacCam-69)->camera_type_desc+
                                   "_"+QString::number(coach_10.at(iMosiacCam-69)->sno));
    }
    //Iterates Saloon Camera 8 for coach 9 and Saloon Camera 1,2 & 3 for coach 10
    else if(iMosiacCam == 71) {
        if(coach_9.at(iMosiacCam-64)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_10.at(iMosiacCam-71)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_10.at(iMosiacCam-70)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_10.at(iMosiacCam-69)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-64)->csno)+" "+coach_9.at(iMosiacCam-64)->car_type_desc+"_"+coach_9.at(iMosiacCam-64)->camera_type_desc+
                                   "_"+QString::number(coach_9.at(iMosiacCam-64)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_10.at(iMosiacCam-71)->csno)+" "+coach_10.at(iMosiacCam-71)->car_type_desc+"_"+coach_10.at(iMosiacCam-71)->camera_type_desc+
                                   "_"+QString::number(coach_10.at(iMosiacCam-71)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-70)->csno)+" "+coach_10.at(iMosiacCam-70)->car_type_desc+"_"+coach_10.at(iMosiacCam-70)->camera_type_desc+
                                   "_"+QString::number(coach_10.at(iMosiacCam-70)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_9.at(iMosiacCam-69)->csno)+" "+coach_10.at(iMosiacCam-69)->car_type_desc+"_"+coach_10.at(iMosiacCam-69)->camera_type_desc+
                                   "_"+QString::number(coach_10.at(iMosiacCam-69)->sno));
    }
    //Iterates Saloon Camera 1-8 for coach 10
    else if(iMosiacCam >= 72 && iMosiacCam < 77) {

        if(coach_10.at(iMosiacCam-72)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_10.at(iMosiacCam-71)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_10.at(iMosiacCam-70)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_10.at(iMosiacCam-69)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_10.at(iMosiacCam-72)->csno)+" "+coach_10.at(iMosiacCam-72)->car_type_desc+"_"+coach_10.at(iMosiacCam-72)->camera_type_desc+
                                   "_"+QString::number(coach_10.at(iMosiacCam-72)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_10.at(iMosiacCam-71)->csno)+" "+coach_10.at(iMosiacCam-71)->car_type_desc+"_"+coach_10.at(iMosiacCam-71)->camera_type_desc+
                                   "_"+QString::number(coach_10.at(iMosiacCam-71)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_10.at(iMosiacCam-70)->csno)+" "+coach_10.at(iMosiacCam-70)->car_type_desc+"_"+coach_10.at(iMosiacCam-70)->camera_type_desc+
                                   "_"+QString::number(coach_10.at(iMosiacCam-70)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_10.at(iMosiacCam-69)->csno)+" "+coach_10.at(iMosiacCam-69)->car_type_desc+"_"+coach_10.at(iMosiacCam-69)->camera_type_desc+
                                   "_"+QString::number(coach_10.at(iMosiacCam-69)->sno));
    }
    //Iterates Saloon Camera 6,7 & 8 for coach 10 and Saloon Camera 1 for coach 11
    else if(iMosiacCam == 77) {

        if(coach_10.at(iMosiacCam-72)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_10.at(iMosiacCam-71)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_10.at(iMosiacCam-70)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_10.at(iMosiacCam-77)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_10.at(iMosiacCam-72)->csno)+" "+coach_10.at(iMosiacCam-72)->car_type_desc+"_"+coach_10.at(iMosiacCam-72)->camera_type_desc+
                                   "_"+QString::number(coach_10.at(iMosiacCam-72)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_10.at(iMosiacCam-71)->csno)+" "+coach_10.at(iMosiacCam-71)->car_type_desc+"_"+coach_10.at(iMosiacCam-71)->camera_type_desc+
                                   "_"+QString::number(coach_10.at(iMosiacCam-71)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_10.at(iMosiacCam-70)->csno)+" "+coach_10.at(iMosiacCam-70)->car_type_desc+"_"+coach_10.at(iMosiacCam-70)->camera_type_desc+
                                   "_"+QString::number(coach_10.at(iMosiacCam-70)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_11.at(iMosiacCam-77)->csno)+" "+coach_11.at(iMosiacCam-77)->car_type_desc+"_"+coach_11.at(iMosiacCam-77)->camera_type_desc+
                                   "_"+QString::number(coach_11.at(iMosiacCam-77)->sno));
    }
    //Iterates Saloon Camera 7 & 8 for coach 10 and Saloon Camera 1,2 for coach 11
    else if(iMosiacCam == 78) {
        if(coach_10.at(iMosiacCam-72)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_10.at(iMosiacCam-71)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_11.at(iMosiacCam-78)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_11.at(iMosiacCam-77)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_10.at(iMosiacCam-72)->csno)+" "+coach_10.at(iMosiacCam-72)->car_type_desc+"_"+coach_10.at(iMosiacCam-72)->camera_type_desc+
                                   "_"+QString::number(coach_10.at(iMosiacCam-72)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_10.at(iMosiacCam-71)->csno)+" "+coach_10.at(iMosiacCam-71)->car_type_desc+"_"+coach_10.at(iMosiacCam-71)->camera_type_desc+
                                   "_"+QString::number(coach_10.at(iMosiacCam-71)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_11.at(iMosiacCam-78)->csno)+" "+coach_11.at(iMosiacCam-78)->car_type_desc+"_"+coach_11.at(iMosiacCam-78)->camera_type_desc+
                                   "_"+QString::number(coach_11.at(iMosiacCam-78)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_11.at(iMosiacCam-77)->csno)+" "+coach_11.at(iMosiacCam-77)->car_type_desc+"_"+coach_11.at(iMosiacCam-77)->camera_type_desc+
                                   "_"+QString::number(coach_11.at(iMosiacCam-77)->sno));
    }
    //Iterates Saloon Camera 8 for coach 10 and Saloon Camera 1,2 & 3 for coach 11
    else if(iMosiacCam == 79) {
        if(coach_10.at(iMosiacCam-72)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_11.at(iMosiacCam-79)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_11.at(iMosiacCam-78)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_11.at(iMosiacCam-77)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_10.at(iMosiacCam-72)->csno)+" "+coach_10.at(iMosiacCam-72)->car_type_desc+"_"+coach_10.at(iMosiacCam-72)->camera_type_desc+
                                   "_"+QString::number(coach_10.at(iMosiacCam-72)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_11.at(iMosiacCam-79)->csno)+" "+coach_11.at(iMosiacCam-79)->car_type_desc+"_"+coach_11.at(iMosiacCam-79)->camera_type_desc+
                                   "_"+QString::number(coach_11.at(iMosiacCam-79)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_11.at(iMosiacCam-78)->csno)+" "+coach_11.at(iMosiacCam-78)->car_type_desc+"_"+coach_11.at(iMosiacCam-78)->camera_type_desc+
                                   "_"+QString::number(coach_11.at(iMosiacCam-78)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_11.at(iMosiacCam-77)->csno)+" "+coach_11.at(iMosiacCam-77)->car_type_desc+"_"+coach_11.at(iMosiacCam-77)->camera_type_desc+
                                   "_"+QString::number(coach_11.at(iMosiacCam-77)->sno));
    }
    //Iterates Saloon Camera 1-8 for coach 11
    else if(iMosiacCam >= 80 && iMosiacCam < 85) {

        if(coach_11.at(iMosiacCam-80)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_11.at(iMosiacCam-79)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_11.at(iMosiacCam-78)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_11.at(iMosiacCam-77)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_11.at(iMosiacCam-80)->csno)+" "+coach_11.at(iMosiacCam-80)->car_type_desc+"_"+coach_11.at(iMosiacCam-80)->camera_type_desc+
                                   "_"+QString::number(coach_11.at(iMosiacCam-80)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_11.at(iMosiacCam-79)->csno)+" "+coach_11.at(iMosiacCam-79)->car_type_desc+"_"+coach_11.at(iMosiacCam-79)->camera_type_desc+
                                   "_"+QString::number(coach_11.at(iMosiacCam-79)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_11.at(iMosiacCam-78)->csno)+" "+coach_11.at(iMosiacCam-78)->car_type_desc+"_"+coach_11.at(iMosiacCam-78)->camera_type_desc+
                                   "_"+QString::number(coach_11.at(iMosiacCam-78)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_11.at(iMosiacCam-77)->csno)+" "+coach_11.at(iMosiacCam-77)->car_type_desc+"_"+coach_11.at(iMosiacCam-77)->camera_type_desc+
                                   "_"+QString::number(coach_11.at(iMosiacCam-77)->sno));
    }
    //Iterates Saloon Camera 6,7 & 8 for coach 11 and Saloon Camera 1 for coach 12
    else if(iMosiacCam == 85) {

        if(coach_11.at(iMosiacCam-80)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_11.at(iMosiacCam-79)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_11.at(iMosiacCam-78)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_12.at(iMosiacCam-81)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_11.at(iMosiacCam-80)->csno)+" "+coach_11.at(iMosiacCam-80)->car_type_desc+"_"+coach_11.at(iMosiacCam-80)->camera_type_desc+
                                   "_"+QString::number(coach_11.at(iMosiacCam-80)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_11.at(iMosiacCam-79)->csno)+" "+coach_11.at(iMosiacCam-79)->car_type_desc+"_"+coach_11.at(iMosiacCam-79)->camera_type_desc+
                                   "_"+QString::number(coach_11.at(iMosiacCam-79)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_11.at(iMosiacCam-78)->csno)+" "+coach_11.at(iMosiacCam-78)->car_type_desc+"_"+coach_11.at(iMosiacCam-78)->camera_type_desc+
                                   "_"+QString::number(coach_11.at(iMosiacCam-78)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_12.at(iMosiacCam-81)->csno)+" "+coach_12.at(iMosiacCam-81)->car_type_desc+"_"+coach_12.at(iMosiacCam-81)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iMosiacCam-81)->sno));
    }
    //Iterates Saloon Camera 7 & 8 for coach 11 and Saloon Camera 1,2 for coach 12
    else if(iMosiacCam == 86) {
        if(coach_11.at(iMosiacCam-80)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_11.at(iMosiacCam-79)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_12.at(iMosiacCam-82)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_12.at(iMosiacCam-81)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_11.at(iMosiacCam-80)->csno)+" "+coach_11.at(iMosiacCam-80)->car_type_desc+"_"+coach_11.at(iMosiacCam-80)->camera_type_desc+
                                   "_"+QString::number(coach_11.at(iMosiacCam-80)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_11.at(iMosiacCam-79)->csno)+" "+coach_11.at(iMosiacCam-79)->car_type_desc+"_"+coach_11.at(iMosiacCam-79)->camera_type_desc+
                                   "_"+QString::number(coach_11.at(iMosiacCam-79)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_12.at(iMosiacCam-82)->csno)+" "+coach_12.at(iMosiacCam-82)->car_type_desc+"_"+coach_12.at(iMosiacCam-82)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iMosiacCam-82)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_12.at(iMosiacCam-81)->csno)+" "+coach_12.at(iMosiacCam-81)->car_type_desc+"_"+coach_12.at(iMosiacCam-81)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iMosiacCam-81)->sno));
    }
    //Iterates Saloon Camera 8 for coach 11 and Saloon Camera 1,2 & 3 for coach 12
    else if(iMosiacCam == 87) {
        if(coach_11.at(iMosiacCam-80)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_12.at(iMosiacCam-83)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_12.at(iMosiacCam-82)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_12.at(iMosiacCam-81)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_11.at(iMosiacCam-80)->csno)+" "+coach_11.at(iMosiacCam-80)->car_type_desc+"_"+coach_11.at(iMosiacCam-80)->camera_type_desc+
                                   "_"+QString::number(coach_11.at(iMosiacCam-80)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_12.at(iMosiacCam-83)->csno)+" "+coach_12.at(iMosiacCam-83)->car_type_desc+"_"+coach_12.at(iMosiacCam-83)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iMosiacCam-83)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_12.at(iMosiacCam-82)->csno)+" "+coach_12.at(iMosiacCam-82)->car_type_desc+"_"+coach_12.at(iMosiacCam-82)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iMosiacCam-82)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_12.at(iMosiacCam-81)->csno)+" "+coach_12.at(iMosiacCam-81)->car_type_desc+"_"+coach_12.at(iMosiacCam-81)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iMosiacCam-81)->sno));
    }
    //Iterates Saloon Camera 1-8 for coach 12
    else if(iMosiacCam >= 88 && iMosiacCam < 93) {

        if(coach_12.at(iMosiacCam-84)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_12.at(iMosiacCam-83)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_12.at(iMosiacCam-82)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_12.at(iMosiacCam-81)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_12.at(iMosiacCam-84)->csno)+" "+coach_12.at(iMosiacCam-84)->car_type_desc+"_"+coach_12.at(iMosiacCam-84)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iMosiacCam-84)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_12.at(iMosiacCam-83)->csno)+" "+coach_12.at(iMosiacCam-83)->car_type_desc+"_"+coach_12.at(iMosiacCam-83)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iMosiacCam-83)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_12.at(iMosiacCam-82)->csno)+" "+coach_12.at(iMosiacCam-82)->car_type_desc+"_"+coach_12.at(iMosiacCam-82)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iMosiacCam-82)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_12.at(iMosiacCam-81)->csno)+" "+coach_12.at(iMosiacCam-81)->car_type_desc+"_"+coach_12.at(iMosiacCam-81)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iMosiacCam-81)->sno));
    }
    //Iterates Saloon Camera 6,7 & 8 for coach 12 and Saloon Camera 1 for coach 1
    else if(iMosiacCam == 93) {

        if(coach_12.at(iMosiacCam-84)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_12.at(iMosiacCam-83)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_12.at(iMosiacCam-82)->activeStatus==ACTIVE){
            _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac3();
        }
        if(coach_1.at(iMosiacCam-89)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_12.at(iMosiacCam-84)->csno)+" "+coach_12.at(iMosiacCam-84)->car_type_desc+"_"+coach_12.at(iMosiacCam-84)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iMosiacCam-84)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_12.at(iMosiacCam-83)->csno)+" "+coach_12.at(iMosiacCam-83)->car_type_desc+"_"+coach_12.at(iMosiacCam-83)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iMosiacCam-83)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_12.at(iMosiacCam-82)->csno)+" "+coach_12.at(iMosiacCam-82)->car_type_desc+"_"+coach_12.at(iMosiacCam-82)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iMosiacCam-82)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_1.at(iMosiacCam-89)->csno)+" "+coach_1.at(iMosiacCam-89)->car_type_desc+"_"+coach_1.at(iMosiacCam-89)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam-89)->sno));
    }
    //Iterates Saloon Camera 7 & 8 for coach 12 and Saloon Camera 1,2 for coach 1
    else if(iMosiacCam == 94) {
        if(coach_12.at(iMosiacCam-84)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_12.at(iMosiacCam-83)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_1.at(iMosiacCam-90)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_1.at(iMosiacCam-89)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_12.at(iMosiacCam-84)->csno)+" "+coach_12.at(iMosiacCam-84)->car_type_desc+"_"+coach_12.at(iMosiacCam-84)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iMosiacCam-84)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_12.at(iMosiacCam-83)->csno)+" "+coach_12.at(iMosiacCam-83)->car_type_desc+"_"+coach_12.at(iMosiacCam-83)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iMosiacCam-83)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_1.at(iMosiacCam-90)->csno)+" "+coach_1.at(iMosiacCam-90)->car_type_desc+"_"+coach_1.at(iMosiacCam-90)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam-90)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_1.at(iMosiacCam-89)->csno)+" "+coach_1.at(iMosiacCam-89)->car_type_desc+"_"+coach_1.at(iMosiacCam-89)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam-89)->sno));
    }
    //Iterates Saloon Camera 8 for coach 12 and Saloon Camera 1,2 & 3 for coach 1
    else if(iMosiacCam == 95) {
        if(coach_12.at(iMosiacCam-84)->activeStatus==ACTIVE){
            _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
            playerMosiac1();
        }
        if(coach_1.at(iMosiacCam-91)->activeStatus==ACTIVE){
            _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
            playerMosiac2();
        }
        if(coach_1.at(iMosiacCam-90)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
            playerMosiac4();
        }
        if(coach_1.at(iMosiacCam-89)->activeStatus==ACTIVE){
            _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
            playerMosiac4();
        }
        ui->label_frame_6->setText("COACH - "+QString::number(coach_12.at(iMosiacCam-84)->csno)+" "+coach_12.at(iMosiacCam-84)->car_type_desc+"_"+coach_12.at(iMosiacCam-84)->camera_type_desc+
                                   "_"+QString::number(coach_12.at(iMosiacCam-84)->sno));
        ui->label_frame_7->setText("COACH - "+QString::number(coach_1.at(iMosiacCam-91)->csno)+" "+coach_1.at(iMosiacCam-91)->car_type_desc+"_"+coach_1.at(iMosiacCam-91)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam-91)->sno));
        ui->label_frame_8->setText("COACH - "+QString::number(coach_1.at(iMosiacCam-90)->csno)+" "+coach_1.at(iMosiacCam-90)->car_type_desc+"_"+coach_1.at(iMosiacCam-90)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam-90)->sno));
        ui->label_frame_9->setText("COACH - "+QString::number(coach_1.at(iMosiacCam-89)->csno)+" "+coach_1.at(iMosiacCam-89)->car_type_desc+"_"+coach_1.at(iMosiacCam-89)->camera_type_desc+
                                   "_"+QString::number(coach_1.at(iMosiacCam-89)->sno));
    }
}
//Button on the 4 camera view page
void MainWindow::on_pushButton_left_clicked()
{
    //timer to keep the window active
    timeractive.start();

    //Stopping player
    stop_view_Mosiac_cam();
    libvlc_media_player_stop (_mp12);


    if(iMosiacCam==0){
        iMosiacCam = listSaloonCams.length() - 3; //9
    }

    iMosiacCam --;

    iterateMosiacCamView();
}
void MainWindow::on_pushButton_right_clicked()
{
    //timer to keep the window active
    timeractive.start();

    //Stopping player
    stop_view_Mosiac_cam();
    libvlc_media_player_stop (_mp12);


    if(iMosiacCam==listSaloonCams.length() - 4) //8
    {
        iMosiacCam = -1;
    }

    iMosiacCam ++;

    iterateMosiacCamView();
}
//=======================================================================

//=======================================================================
//Function for playing rtsp stream in fullview _mp10
void MainWindow::playerFullCam(){
    libvlc_media_player_set_media (_mp10, _m10);
    int windid10 = ui->frame_10->winId();
    libvlc_media_player_set_xwindow (_mp10, windid10 );
    libvlc_media_player_play (_mp10);
    _isPlaying=true;
}

//Function for iterating Full Cam View
void MainWindow::iterateFullCamView()
{

    //coach 1
    if(iFullCam >= 0 && iFullCam < 4){
        if(coach_1.at(iFullCam)->activeStatus==ACTIVE){
            _m10 = libvlc_media_new_location(_vlcinstance, listAllCams[iFullCam].toLocal8Bit());
            playerFullCam();

        }
        ui->label_frame_10->setText("COACH NO - "+QString::number(coach_1.at(iFullCam)->csno)+" COACH TYPE - "+coach_1.at(iFullCam)->car_type_desc);
        ui->label_cam_no_2->setText(coach_1.at(iFullCam)->camera_type_desc);
    }
    else if(iFullCam >= 4 && iFullCam < 12){
        if(coach_1.at(iFullCam)->activeStatus==ACTIVE){
            _m10 = libvlc_media_new_location(_vlcinstance, listAllCams[iFullCam].toLocal8Bit());
            playerFullCam();
        }
        ui->label_frame_10->setText("COACH NO - "+QString::number(coach_1.at(iFullCam)->csno)+" COACH TYPE - "+coach_1.at(iFullCam)->car_type_desc);
        ui->label_cam_no_2->setText(coach_1.at(iFullCam)->camera_type_desc+" CAM NO - " + QString::number(coach_1.at(iFullCam)->sno));
    }
    //coach 2
    else if(iFullCam >= 12 && iFullCam < 20)
    {
        if(coach_2.at(iFullCam-12)->activeStatus==ACTIVE){
            _m10 = libvlc_media_new_location(_vlcinstance, listAllCams[iFullCam].toLocal8Bit());
            playerFullCam();
        }
        ui->label_frame_10->setText("COACH NO - "+QString::number(coach_2.at(iFullCam-12)->csno)+" COACH TYPE - "+coach_2.at(iFullCam-12)->car_type_desc);
        ui->label_cam_no_2->setText(coach_2.at(iFullCam-12)->camera_type_desc+" CAM NO - " + QString::number(coach_2.at(iFullCam-12)->sno));
    }
    //coach 3
    else if(iFullCam >= 20 && iFullCam < 28)
    {
        if(coach_3.at(iFullCam-20)->activeStatus==ACTIVE){
            _m10 = libvlc_media_new_location(_vlcinstance, listAllCams[iFullCam].toLocal8Bit());
            playerFullCam();
        }
        ui->label_frame_10->setText("COACH NO - "+QString::number(coach_3.at(iFullCam-20)->csno)+" COACH TYPE - "+coach_3.at(iFullCam-20)->car_type_desc);
        ui->label_cam_no_2->setText(coach_3.at(iFullCam-20)->camera_type_desc+" CAM NO - " + QString::number(coach_3.at(iFullCam-20)->sno));
    }
    //coach 4
    else if(iFullCam >= 28 && iFullCam < 36)
    {
        if(coach_4.at(iFullCam-28)->activeStatus==ACTIVE){
            _m10 = libvlc_media_new_location(_vlcinstance, listAllCams[iFullCam].toLocal8Bit());
            playerFullCam();
        }
        ui->label_frame_10->setText("COACH NO - "+QString::number(coach_4.at(iFullCam-28)->csno)+" COACH TYPE - "+coach_4.at(iFullCam-28)->car_type_desc);
        ui->label_cam_no_2->setText(coach_4.at(iFullCam-28)->camera_type_desc+" CAM NO - " + QString::number(coach_4.at(iFullCam-28)->sno));
    }
    //coach 5
    else if(iFullCam >= 36 && iFullCam < 44)
    {
        if(coach_5.at(iFullCam-36)->activeStatus==ACTIVE){
            _m10 = libvlc_media_new_location(_vlcinstance, listAllCams[iFullCam].toLocal8Bit());
            playerFullCam();
        }
        ui->label_frame_10->setText("COACH NO - "+QString::number(coach_5.at(iFullCam-36)->csno)+" COACH TYPE - "+coach_5.at(iFullCam-36)->car_type_desc);
        ui->label_cam_no_2->setText(coach_5.at(iFullCam-36)->camera_type_desc+" CAM NO - " + QString::number(coach_5.at(iFullCam-36)->sno));
    }
    //coach 6
    else if(iFullCam >= 44 && iFullCam < 52)
    {
        if(coach_6.at(iFullCam-44)->activeStatus==ACTIVE){
            _m10 = libvlc_media_new_location(_vlcinstance, listAllCams[iFullCam].toLocal8Bit());
            playerFullCam();
        }
        ui->label_frame_10->setText("COACH NO - "+QString::number(coach_6.at(iFullCam-44)->csno)+" COACH TYPE - "+coach_6.at(iFullCam-44)->car_type_desc);
        ui->label_cam_no_2->setText(coach_6.at(iFullCam-44)->camera_type_desc+" CAM NO - " + QString::number(coach_6.at(iFullCam-44)->sno));
    }
    //coach 7
    else if(iFullCam >= 52 && iFullCam < 60)
    {
        if(coach_7.at(iFullCam-52)->activeStatus==ACTIVE){
            _m10 = libvlc_media_new_location(_vlcinstance, listAllCams[iFullCam].toLocal8Bit());
            playerFullCam();
        }
        ui->label_frame_10->setText("COACH NO - "+QString::number(coach_7.at(iFullCam-52)->csno)+" COACH TYPE - "+coach_7.at(iFullCam-52)->car_type_desc);
        ui->label_cam_no_2->setText(coach_7.at(iFullCam-52)->camera_type_desc+" CAM NO - " + QString::number(coach_7.at(iFullCam-52)->sno));
    }
    //coach 8
    else if(iFullCam >= 60 && iFullCam < 68)
    {
        if(coach_8.at(iFullCam-60)->activeStatus==ACTIVE){
            _m10 = libvlc_media_new_location(_vlcinstance, listAllCams[iFullCam].toLocal8Bit());
            playerFullCam();
        }
        ui->label_frame_10->setText("COACH NO - "+QString::number(coach_8.at(iFullCam-60)->csno)+" COACH TYPE - "+coach_8.at(iFullCam-60)->car_type_desc);
        ui->label_cam_no_2->setText(coach_8.at(iFullCam-60)->camera_type_desc+" CAM NO - " + QString::number(coach_8.at(iFullCam-60)->sno));
    }
    //coach 9
    else if(iFullCam >= 68 && iFullCam < 76)
    {
        if(coach_9.at(iFullCam-68)->activeStatus==ACTIVE){
            _m10 = libvlc_media_new_location(_vlcinstance, listAllCams[iFullCam].toLocal8Bit());
            playerFullCam();
        }
        ui->label_frame_10->setText("COACH NO - "+QString::number(coach_9.at(iFullCam-68)->csno)+" COACH TYPE - "+coach_9.at(iFullCam-68)->car_type_desc);
        ui->label_cam_no_2->setText(coach_9.at(iFullCam-68)->camera_type_desc+" CAM NO - " + QString::number(coach_9.at(iFullCam-68)->sno));
    }
    //coach 10
    else if(iFullCam >= 76 && iFullCam < 84)
    {
        if(coach_10.at(iFullCam-76)->activeStatus==ACTIVE){
            _m10 = libvlc_media_new_location(_vlcinstance, listAllCams[iFullCam].toLocal8Bit());
            playerFullCam();
        }
        ui->label_frame_10->setText("COACH NO - "+QString::number(coach_10.at(iFullCam-76)->csno)+" COACH TYPE - "+coach_10.at(iFullCam-76)->car_type_desc);
        ui->label_cam_no_2->setText(coach_10.at(iFullCam-76)->camera_type_desc+" CAM NO - " + QString::number(coach_10.at(iFullCam-76)->sno));
    }
    //coach 11
    else if(iFullCam >= 84 && iFullCam < 92)
    {
        if(coach_11.at(iFullCam-84)->activeStatus==ACTIVE){
            _m10 = libvlc_media_new_location(_vlcinstance, listAllCams[iFullCam].toLocal8Bit());
            playerFullCam();
        }
        ui->label_frame_10->setText("COACH NO - "+QString::number(coach_11.at(iFullCam-84)->csno)+" COACH TYPE - "+coach_11.at(iFullCam-84)->car_type_desc);
        ui->label_cam_no_2->setText(coach_11.at(iFullCam-84)->camera_type_desc+" CAM NO - " + QString::number(coach_11.at(iFullCam-84)->sno));
    }
    //coach 12
    else if(iFullCam >= 92 && iFullCam < 96)
    {
        if(coach_12.at(iFullCam-92)->activeStatus==ACTIVE){
            _m10 = libvlc_media_new_location(_vlcinstance, listAllCams[iFullCam].toLocal8Bit());
            playerFullCam();
        }
        ui->label_frame_10->setText("COACH NO - "+QString::number(coach_12.at(iFullCam-92)->csno)+" COACH TYPE - "+coach_12.at(iFullCam-92)->car_type_desc);
        ui->label_cam_no_2->setText(coach_12.at(iFullCam-92)->camera_type_desc);
    }
    else if(iFullCam >= 96 && iFullCam < 104){
        if(coach_12.at(iFullCam-92)->activeStatus==ACTIVE){
            _m10 = libvlc_media_new_location(_vlcinstance, listAllCams[iFullCam].toLocal8Bit());
            playerFullCam();
        }
        ui->label_frame_10->setText("COACH NO - "+QString::number(coach_12.at(iFullCam-92)->csno)+" COACH TYPE - "+coach_12.at(iFullCam-92)->car_type_desc);
        ui->label_cam_no_2->setText(coach_12.at(iFullCam-92)->camera_type_desc+" CAM NO - " + QString::number(coach_12.at(iFullCam-92)->sno));
    }
    else{
        //Stopping player
        libvlc_media_player_stop (_mp10);
    }
}

//Button on the full camera view page
void MainWindow::on_pushButton_left_full_clicked()
{
    //timer to keep the window active
    timeractive.start();

    if(iFullCam==0)
    {
        iFullCam=listAllCams.length(); // 10
    }

    iFullCam--;

    libvlc_media_player_stop (_mp10);
    libvlc_media_player_stop (_mp12);

    iterateFullCamView();

}

void MainWindow::on_pushButton_right_full_clicked()
{
    //timer to keep the window active
    timeractive.start();

    if(iFullCam==listAllCams.length()-1) //9
    {
        iFullCam=-1;
    }

    iFullCam++;

    libvlc_media_player_stop (_mp10);
    libvlc_media_player_stop (_mp12);

    iterateFullCamView();

}
//=======================================================================

//=======================================================================

//Buttons for opening IPCAM stream in full view
//pushButton_frame_1 - pushButton_frame_5 for page_camView_1
//pushButton_frame_6 - pushButton_frame_9 for page_camViewMosaic
//pushButton_frame_10 for page_camViewFull

void MainWindow::renameEtbAudio()
{
    //Renaming audio recording of etb received from DDC
    QString renameEtbAudio = "mv "+pathToEtbArchives+"/rec_* "+pathToEtbArchives+recordedFileNameEtb+"_etb_audio.wav";

    system(qPrintable(renameEtbAudio));

    QString mvEtbAudio = "mv "+pathToEtbArchives+recordedFileNameEtb+"_etb_audio.wav " +pathToEtbArchives+date_text_recording+"_recordings/";

    system(qPrintable(mvEtbAudio));
}

void MainWindow::recPlayer(){
    libvlc_media_player_set_media (_mp12, _m12);
    int windid12 = ui->frame_12->winId();
    libvlc_media_player_set_xwindow (_mp12, windid12 );
    libvlc_media_player_play (_mp12);
    _isPlaying=true;
}

void MainWindow::contRecording()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    const char* url_etb =  "rtsp://192.168.1.223/video1.sdp";

    _m12 = libvlc_media_new_location(_vlcinstance, url_etb);

    recPlayer();

    ui->label_cam_no->setText("ETB CAM NO - 1");
    ui->label_cam_ip->setText(url_etb);

    recordString = url_etb;

    camNoFileName = "ETB_CAM_1";

}

void MainWindow::on_pushButton_frame_1_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam].toLocal8Bit());
    recPlayer();

    ui->label_cam_ip->setText(listExternalCams[iExtCam]);


    recordString = listExternalCams[iExtCam];

    if(iExtCam == 0){
        camNoFileName = "COACH_"+QString::number(coach_1.at(0)->csno)+"_"+coach_1.at(0)->car_type_desc+"_"+coach_1.at(0)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_1.at(0)->csno)+"_"+coach_1.at(0)->car_type_desc+"_"+coach_1.at(0)->camera_type_desc);
    }
    else if(iExtCam == 1){
        camNoFileName = "COACH_"+QString::number(coach_1.at(1)->csno)+"_"+coach_1.at(1)->car_type_desc+"_"+coach_1.at(1)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_1.at(1)->csno)+"_"+coach_1.at(1)->car_type_desc+"_"+coach_1.at(1)->camera_type_desc);
    }
    else if(iExtCam == 2){
        camNoFileName = "COACH_"+QString::number(coach_1.at(2)->csno)+"_"+coach_1.at(2)->car_type_desc+"_"+coach_1.at(2)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_1.at(2)->csno)+"_"+coach_1.at(2)->car_type_desc+"_"+coach_1.at(2)->camera_type_desc);
    }
    else if(iExtCam == 3){
        camNoFileName = "COACH_"+QString::number(coach_1.at(3)->csno)+"_"+coach_1.at(3)->car_type_desc+"_"+coach_1.at(43)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_1.at(3)->csno)+"_"+coach_1.at(3)->car_type_desc+"_"+coach_1.at(3)->camera_type_desc);
    }
    else if(iExtCam == 4){
        camNoFileName = "COACH_"+QString::number(coach_12.at(0)->csno)+"_"+coach_12.at(0)->car_type_desc+"_"+coach_12.at(0)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(0)->csno)+"_"+coach_12.at(0)->car_type_desc+"_"+coach_12.at(0)->camera_type_desc);
    }
    else if(iExtCam == 5){
        camNoFileName = "COACH_"+QString::number(coach_12.at(1)->csno)+"_"+coach_12.at(1)->car_type_desc+"_"+coach_12.at(1)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(1)->csno)+"_"+coach_12.at(1)->car_type_desc+"_"+coach_12.at(1)->camera_type_desc);
    }
}


void MainWindow::on_pushButton_frame_2_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+1].toLocal8Bit());
    recPlayer();

    ui->label_cam_ip->setText(listExternalCams[iExtCam+1]);

    recordString = listExternalCams[iExtCam+1];

    if(iExtCam == 0){
        camNoFileName = "COACH_"+QString::number(coach_1.at(1)->csno)+"_"+coach_1.at(1)->car_type_desc+"_"+coach_1.at(1)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_1.at(1)->csno)+"_"+coach_1.at(1)->car_type_desc+"_"+coach_1.at(1)->camera_type_desc);
    }
    else if(iExtCam == 1){
        camNoFileName = "COACH_"+QString::number(coach_1.at(2)->csno)+"_"+coach_1.at(2)->car_type_desc+"_"+coach_1.at(2)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_1.at(2)->csno)+"_"+coach_1.at(2)->car_type_desc+"_"+coach_1.at(2)->camera_type_desc);
    }
    else if(iExtCam == 2){
        camNoFileName = "COACH_"+QString::number(coach_1.at(3)->csno)+"_"+coach_1.at(3)->car_type_desc+"_"+coach_1.at(3)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_1.at(3)->csno)+"_"+coach_1.at(3)->car_type_desc+"_"+coach_1.at(3)->camera_type_desc);
    }
    else if(iExtCam == 3){
        camNoFileName = "COACH_"+QString::number(coach_12.at(0)->csno)+"_"+coach_12.at(0)->car_type_desc+"_"+coach_12.at(0)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(0)->csno)+"_"+coach_12.at(0)->car_type_desc+"_"+coach_12.at(0)->camera_type_desc);
    }
    else if(iExtCam == 4){
        camNoFileName = "COACH_"+QString::number(coach_12.at(1)->csno)+"_"+coach_12.at(1)->car_type_desc+"_"+coach_12.at(1)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(1)->csno)+"_"+coach_12.at(1)->car_type_desc+"_"+coach_12.at(1)->camera_type_desc);
    }
    else if(iExtCam == 5){
        camNoFileName = "COACH_"+QString::number(coach_12.at(2)->csno)+"_"+coach_12.at(2)->car_type_desc+"_"+coach_12.at(2)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(2)->csno)+"_"+coach_12.at(2)->car_type_desc+"_"+coach_12.at(2)->camera_type_desc);
    }
}


void MainWindow::on_pushButton_frame_3_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+2].toLocal8Bit());
    recPlayer();

    ui->label_cam_ip->setText(listExternalCams[iExtCam+2]);

    recordString = listExternalCams[iExtCam+2];

    if(iExtCam == 0){
        camNoFileName = "COACH_"+QString::number(coach_1.at(2)->csno)+"_"+coach_1.at(2)->car_type_desc+"_"+coach_1.at(2)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_1.at(2)->csno)+"_"+coach_1.at(2)->car_type_desc+"_"+coach_1.at(2)->camera_type_desc);
    }
    else if(iExtCam == 1){
        camNoFileName = "COACH_"+QString::number(coach_1.at(3)->csno)+"_"+coach_1.at(3)->car_type_desc+"_"+coach_1.at(3)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_1.at(3)->csno)+"_"+coach_1.at(3)->car_type_desc+"_"+coach_1.at(3)->camera_type_desc);
    }
    else if(iExtCam == 2){
        camNoFileName = "COACH_"+QString::number(coach_12.at(0)->csno)+"_"+coach_12.at(0)->car_type_desc+"_"+coach_12.at(0)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(0)->csno)+"_"+coach_12.at(0)->car_type_desc+"_"+coach_12.at(0)->camera_type_desc);
    }
    else if(iExtCam == 3){
        camNoFileName = "COACH_"+QString::number(coach_12.at(1)->csno)+"_"+coach_12.at(1)->car_type_desc+"_"+coach_12.at(1)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(1)->csno)+"_"+coach_12.at(1)->car_type_desc+"_"+coach_12.at(1)->camera_type_desc);
    }
    else if(iExtCam == 4){
        camNoFileName = "COACH_"+QString::number(coach_12.at(2)->csno)+"_"+coach_12.at(2)->car_type_desc+"_"+coach_12.at(2)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(2)->csno)+"_"+coach_12.at(2)->car_type_desc+"_"+coach_12.at(2)->camera_type_desc);
    }
    else if(iExtCam == 5){
        camNoFileName = "COACH_"+QString::number(coach_12.at(3)->csno)+"_"+coach_12.at(3)->car_type_desc+"_"+coach_12.at(3)->camera_type_desc;
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(3)->csno)+"_"+coach_12.at(3)->car_type_desc+"_"+coach_12.at(3)->camera_type_desc);
    }
}


void MainWindow::on_pushButton_frame_4_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam].toLocal8Bit());
    recPlayer();

    ui->label_cam_ip->setText(listSaloonCams[iSaloonCam]);

    recordString = listSaloonCams[iSaloonCam];

    if(iSaloonCam<8){
        for(int i = 0; i < 8 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_1.at(i+4)->csno)+"_"+coach_1.at(i+4)->car_type_desc+"_"
                                          +coach_1.at(i+4)->camera_type_desc+"_"+QString::number(coach_1.at(i+4)->sno));
                camNoFileName = "COACH_"+QString::number(coach_1.at(i+4)->csno)+"_"+coach_1.at(i+4)->car_type_desc+"_"
                        +coach_1.at(i+4)->camera_type_desc+"_"+QString::number(coach_1.at(i+4)->sno);
            }
        }
    }
    else if(iSaloonCam>7 && iSaloonCam <16){
        for(int i = 8; i < 16 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_2.at(i-8)->csno)+"_"+coach_2.at(i-8)->car_type_desc+"_"
                                          +coach_2.at(i-8)->camera_type_desc+"_"+QString::number(coach_2.at(i-8)->sno));
                camNoFileName = "COACH_"+QString::number(coach_2.at(i-8)->csno)+"_"+coach_2.at(i-8)->car_type_desc+"_"
                        +coach_2.at(i-8)->camera_type_desc+"_"+QString::number(coach_2.at(i-8)->sno);
            }
        }
    }
    else if(iSaloonCam>15 && iSaloonCam <24){
        for(int i = 16; i < 24 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_3.at(i-16)->csno)+"_"+coach_3.at(i-16)->car_type_desc+"_"+
                                          coach_3.at(i-16)->camera_type_desc+"_"+QString::number(coach_3.at(i-16)->sno));
                camNoFileName = "COACH_"+QString::number(coach_3.at(i-16)->csno)+"_"+coach_3.at(i-16)->car_type_desc+"_"+
                        coach_3.at(i-16)->camera_type_desc+"_"+QString::number(coach_3.at(i-16)->sno);
            }
        }
    }
    else if(iSaloonCam>23 && iSaloonCam <32){
        for(int i = 24; i < 32 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_4.at(i-24)->csno)+"_"+coach_4.at(i-24)->car_type_desc+"_"+
                                          coach_4.at(i-24)->camera_type_desc+"_"+QString::number(coach_4.at(i-24)->sno));
                camNoFileName = "COACH_"+QString::number(coach_4.at(i-24)->csno)+"_"+coach_4.at(i-24)->car_type_desc+"_"+
                        coach_4.at(i-24)->camera_type_desc+"_"+QString::number(coach_4.at(i-24)->sno);
            }
        }
    }
    else if(iSaloonCam>31 && iSaloonCam <40){
        for(int i = 32; i < 40 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number( coach_5.at(i-32)->csno)+"_"+coach_5.at(i-32)->car_type_desc+"_"+
                                          coach_5.at(i-32)->camera_type_desc+"_"+QString::number(coach_5.at(i-32)->sno));
                camNoFileName = "COACH_"+QString::number( coach_5.at(i-32)->csno)+"_"+coach_5.at(i-24)->car_type_desc+"_"+
                        coach_5.at(i-32)->camera_type_desc+"_"+QString::number(coach_5.at(i-32)->sno);
            }
        }
    }
    else if(iSaloonCam>39 && iSaloonCam <48){
        for(int i = 40; i < 48 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_6.at(i-40)->csno)+"_"+coach_6.at(i-40)->car_type_desc+"_"+
                                          coach_6.at(i-40)->camera_type_desc+"_"+QString::number(coach_6.at(i-40)->sno));
                camNoFileName ="COACH_"+QString::number(coach_6.at(i-40)->csno)+"_"+ coach_6.at(i-40)->car_type_desc+"_"+
                        coach_6.at(i-40)->camera_type_desc+"_"+QString::number(coach_6.at(i-40)->sno);
            }
        }
    }
    else if(iSaloonCam>47 && iSaloonCam <56){
        for(int i = 48; i < 56 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_7.at(i-48)->csno)+"_"+coach_7.at(i-48)->car_type_desc+"_"+
                                          coach_7.at(i-48)->camera_type_desc+"_"+QString::number(coach_7.at(i-48)->sno));
                camNoFileName = "COACH_"+QString::number(coach_7.at(i-48)->csno)+"_"+coach_7.at(i-48)->car_type_desc+"_"+
                        coach_7.at(i-48)->camera_type_desc+"_"+QString::number(coach_7.at(i-48)->sno);
            }
        }
    }
    else if(iSaloonCam>55 && iSaloonCam <64){
        for(int i = 56; i < 64 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_8.at(i-56)->csno)+"_"+coach_8.at(i-56)->car_type_desc+"_"+
                                          coach_8.at(i-56)->camera_type_desc+"_"+QString::number(coach_8.at(i-56)->sno));
                camNoFileName = "COACH_"+QString::number(coach_8.at(i-56)->csno)+"_"+coach_8.at(i-56)->car_type_desc+"_"+
                        coach_8.at(i-56)->camera_type_desc+"_"+QString::number(coach_8.at(i-56)->sno);
            }
        }
    }
    else if(iSaloonCam>63 && iSaloonCam <72){
        for(int i = 64; i < 72 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_9.at(i-64)->csno)+"_"+coach_9.at(i-64)->car_type_desc+"_"+
                                          coach_9.at(i-64)->camera_type_desc+"_"+QString::number(coach_9.at(i-64)->sno));
                camNoFileName = "COACH_"+QString::number(coach_9.at(i-64)->csno)+"_"+coach_9.at(i-64)->car_type_desc+"_"+
                        coach_9.at(i-64)->camera_type_desc+"_"+QString::number(coach_9.at(i-64)->sno);
            }
        }
    }
    else if(iSaloonCam>71 && iSaloonCam <80){
        for(int i = 72; i < 80 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_10.at(i-72)->csno)+"_"+coach_10.at(i-72)->car_type_desc+"_"+
                                          coach_10.at(i-72)->camera_type_desc+"_"+QString::number(coach_10.at(i-72)->sno));
                camNoFileName = "COACH_"+QString::number(coach_10.at(i-72)->csno)+"_"+coach_10.at(i-72)->car_type_desc+"_"+
                        coach_10.at(i-72)->camera_type_desc+"_"+QString::number(coach_10.at(i-72)->sno);
            }
        }
    }
    else if(iSaloonCam>79 && iSaloonCam <88){
        for(int i = 80; i < 88 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_11.at(i-80)->csno)+"_"+coach_11.at(i-80)->car_type_desc+"_"+coach_11.at(i-80)->camera_type_desc+"_"+QString::number(coach_11.at(i-80)->sno));
                camNoFileName = "COACH_"+QString::number(coach_11.at(i-80)->csno)+"_"+coach_11.at(i-80)->car_type_desc+"_"+coach_11.at(i-80)->camera_type_desc+"_"+QString::number(coach_11.at(i-80)->sno);
            }
        }
    }
    else if(iSaloonCam>87 && iSaloonCam <96){
        for(int i = 88; i < 96 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(i-84)->csno)+"_"+coach_12.at(i-84)->car_type_desc+"_"+
                                          coach_12.at(i-84)->camera_type_desc+"_"+QString::number(coach_12.at(i-84)->sno));
                camNoFileName = "COACH_"+QString::number(coach_12.at(i-84)->csno)+"_"+coach_12.at(i-84)->car_type_desc+"_"+
                        coach_12.at(i-84)->camera_type_desc+"_"+QString::number(coach_12.at(i-84)->sno);
            }
        }
    }
}


void MainWindow::on_pushButton_frame_5_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1].toLocal8Bit());
    recPlayer();

    ui->label_cam_ip->setText(listSaloonCams[iSaloonCam+1]);

    recordString = listSaloonCams[iSaloonCam+1];
    //C1
    if(iSaloonCam<8){
        for(int i = 0; i < 7 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_1.at(i+5)->csno)+"_"+coach_1.at(i+5)->car_type_desc+"_"+
                                          coach_1.at(i+5)->camera_type_desc+"_"+QString::number(coach_1.at(i+5)->sno));
                camNoFileName = "COACH_"+QString::number(coach_1.at(i+5)->csno)+"_"+coach_1.at(i+5)->car_type_desc+"_"+
                        coach_1.at(i+5)->camera_type_desc+"_"+QString::number(coach_1.at(i+5)->sno);
            }
        }
        if(iSaloonCam == 7){
            ui->label_cam_no->setText("COACH_"+QString::number(coach_2.at(0)->csno)+"_"+coach_2.at(0)->car_type_desc+"_"+
                                      coach_2.at(0)->camera_type_desc+"_"+QString::number(coach_2.at(0)->sno));
            camNoFileName = "COACH_"+QString::number(coach_2.at(0)->csno)+"_"+coach_2.at(0)->car_type_desc+"_"+
                    coach_2.at(0)->camera_type_desc+"_"+QString::number(coach_2.at(0)->sno);
        }
    }
    //C2
    else if(iSaloonCam>7 && iSaloonCam <16){
        for(int i = 8; i < 15 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_2.at(i-7)->csno)+"_"+coach_2.at(i-7)->car_type_desc+"_"+
                                          coach_2.at(i-7)->camera_type_desc+"_"+QString::number(coach_2.at(i-7)->sno));
                camNoFileName = "COACH_"+QString::number(coach_2.at(i-7)->csno)+"_"+coach_2.at(i-7)->car_type_desc+"_"
                        +coach_2.at(i-7)->camera_type_desc+"_"+QString::number(coach_2.at(i-7)->sno);
            }
        }
        if(iSaloonCam == 15){
            ui->label_cam_no->setText("COACH_"+QString::number(coach_3.at(0)->csno)+"_"+coach_3.at(0)->car_type_desc+"_"
                                      +coach_3.at(0)->camera_type_desc+"_"+QString::number(coach_3.at(0)->sno));
            camNoFileName = "COACH_"+QString::number(coach_3.at(0)->csno)+"_"+coach_3.at(0)->car_type_desc+"_"
                    +coach_3.at(0)->camera_type_desc+"_"+QString::number(coach_3.at(0)->sno);
        }
    }
    //C3
    else if(iSaloonCam>15 && iSaloonCam <24){
        for(int i = 16; i < 23 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_3.at(i-15)->csno)+"_"+coach_3.at(i-15)->car_type_desc+"_"+
                                          coach_3.at(i-15)->camera_type_desc+"_"+QString::number(coach_3.at(i-15)->sno));
                camNoFileName ="COACH_"+QString::number(coach_3.at(i-15)->csno)+"_"+ coach_3.at(i-15)->car_type_desc+"_"+
                        coach_3.at(i-15)->camera_type_desc+"_"+QString::number(coach_3.at(i-15)->sno);
            }
        }
        if(iSaloonCam == 23){
            ui->label_cam_no->setText("COACH_"+QString::number(coach_4.at(0)->csno)+"_"+coach_4.at(0)->car_type_desc+"_"+
                                      coach_4.at(0)->camera_type_desc+"_"+QString::number(coach_4.at(0)->sno));
            camNoFileName = "COACH_"+QString::number(coach_4.at(0)->csno)+"_"+coach_4.at(0)->car_type_desc+"_"+
                    coach_4.at(0)->camera_type_desc+"_"+QString::number(coach_4.at(0)->sno);
        }
    }
    //C4
    else if(iSaloonCam>23 && iSaloonCam <32){
        for(int i = 24; i < 31 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_4.at(i-23)->csno)+"_"+coach_4.at(i-23)->car_type_desc+"_"+
                                          coach_4.at(i-23)->camera_type_desc+"_"+QString::number(coach_4.at(i-23)->sno));
                camNoFileName = "COACH_"+QString::number(coach_4.at(i-23)->csno)+"_"+coach_4.at(i-23)->car_type_desc+"_"+
                        coach_4.at(i-23)->camera_type_desc+"_"+QString::number(coach_4.at(i-23)->sno);
            }
        }
        if(iSaloonCam == 31){
            ui->label_cam_no->setText("COACH_"+QString::number(coach_5.at(0)->csno)+"_"+coach_5.at(0)->car_type_desc+"_"+
                                      coach_5.at(0)->camera_type_desc+"_"+QString::number(coach_5.at(0)->sno));
            camNoFileName = "COACH_"+QString::number(coach_5.at(0)->csno)+"_"+coach_5.at(0)->car_type_desc+"_"+
                    coach_5.at(0)->camera_type_desc+"_"+QString::number(coach_5.at(0)->sno);
        }
    }
    //C5
    else if(iSaloonCam>31 && iSaloonCam <40){
        for(int i = 32; i < 39 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_5.at(i-31)->csno)+"_"+coach_5.at(i-31)->car_type_desc+"_"+
                                          coach_5.at(i-31)->camera_type_desc+"_"+QString::number(coach_5.at(i-31)->sno));
                camNoFileName = "COACH_"+QString::number(coach_5.at(i-31)->csno)+"_"+coach_5.at(i-31)->car_type_desc+"_"+
                        coach_5.at(i-31)->camera_type_desc+"_"+QString::number(coach_5.at(i-31)->sno);
            }
        }
        if(iSaloonCam == 39){
            ui->label_cam_no->setText("COACH_"+QString::number(coach_6.at(0)->csno)+"_"+coach_6.at(0)->car_type_desc+"_"+
                                      coach_6.at(0)->camera_type_desc+"_"+QString::number(coach_6.at(0)->sno));
            camNoFileName = "COACH_"+QString::number(coach_6.at(0)->csno)+"_"+coach_6.at(0)->car_type_desc+"_"+
                    coach_6.at(0)->camera_type_desc+"_"+QString::number(coach_6.at(0)->sno);
        }

    }
    //C6
    else if(iSaloonCam>39 && iSaloonCam <48){
        for(int i = 40; i < 47 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_6.at(i-39)->csno)+"_"+coach_6.at(i-39)->car_type_desc+"_"+
                                          coach_6.at(i-39)->camera_type_desc+"_"+QString::number(coach_6.at(i-39)->sno));
                camNoFileName = "COACH_"+QString::number(coach_6.at(i-39)->csno)+"_"+coach_6.at(i-39)->car_type_desc+"_"+
                        coach_6.at(i-39)->camera_type_desc+"_"+QString::number(coach_6.at(i-39)->sno);
            }
        }
        if(iSaloonCam == 47){
            ui->label_cam_no->setText("COACH_"+QString::number(coach_7.at(0)->csno)+"_"+coach_7.at(0)->car_type_desc+"_"+
                                      coach_7.at(0)->camera_type_desc+"_"+QString::number(coach_7.at(0)->sno));
            camNoFileName = "COACH_"+QString::number(coach_7.at(0)->csno)+"_"+coach_7.at(0)->car_type_desc+"_"+
                    coach_7.at(0)->camera_type_desc+"_"+QString::number(coach_7.at(0)->sno);
        }
    }
    //C7
    else if(iSaloonCam>47 && iSaloonCam <56){
        for(int i = 48; i < 55 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_7.at(i-47)->csno)+"_"+coach_7.at(i-47)->car_type_desc+"_"+
                                          coach_7.at(i-47)->camera_type_desc+"_"+QString::number(coach_7.at(i-47)->sno));
                camNoFileName = "COACH_"+QString::number(coach_7.at(i-47)->csno)+"_"+coach_7.at(i-47)->car_type_desc+"_"+
                        coach_7.at(i-47)->camera_type_desc+"_"+QString::number(coach_7.at(i-47)->sno);
            }
        }
        if(iSaloonCam == 55){
            ui->label_cam_no->setText("COACH_"+QString::number(coach_8.at(0)->csno)+"_"+coach_8.at(0)->car_type_desc+"_"+
                                      coach_8.at(0)->camera_type_desc+"_"+QString::number(coach_8.at(0)->sno));
            camNoFileName = "COACH_"+QString::number(coach_8.at(0)->csno)+"_"+coach_8.at(0)->car_type_desc+"_"+
                    coach_8.at(0)->camera_type_desc+"_"+QString::number(coach_8.at(0)->sno);
        }
    }
    //C8
    else if(iSaloonCam>55 && iSaloonCam <64){
        for(int i = 56; i < 63 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_8.at(i-55)->csno)+"_"+coach_8.at(i-55)->car_type_desc+"_"+
                                          coach_8.at(i-55)->camera_type_desc+"_"+QString::number(coach_8.at(i-55)->sno));
                camNoFileName = "COACH_"+QString::number(coach_8.at(i-55)->csno)+"_"+coach_8.at(i-55)->car_type_desc+"_"+
                        coach_8.at(i-55)->camera_type_desc+"_"+QString::number(coach_8.at(i-55)->sno);
            }
        }
        if(iSaloonCam == 63){
            ui->label_cam_no->setText("COACH_"+QString::number(coach_9.at(0)->csno)+"_"+coach_9.at(0)->car_type_desc+"_"+
                                      coach_9.at(0)->camera_type_desc+"_"+QString::number(coach_9.at(0)->sno));
            camNoFileName = "COACH_"+QString::number(coach_9.at(0)->csno)+"_"+coach_9.at(0)->car_type_desc+"_"+
                    coach_9.at(0)->camera_type_desc+"_"+QString::number(coach_9.at(0)->sno);
        }
    }
    //C9
    else if(iSaloonCam>63 && iSaloonCam <72){
        for(int i = 64; i < 71 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_9.at(i-63)->csno)+"_"+coach_9.at(i-63)->car_type_desc+"_"+
                                          coach_9.at(i-63)->camera_type_desc+"_"+QString::number(coach_9.at(i-63)->sno));
                camNoFileName = "COACH_"+QString::number(coach_9.at(i-63)->csno)+"_"+coach_9.at(i-63)->car_type_desc+"_"+
                        coach_9.at(i-63)->camera_type_desc+"_"+QString::number(coach_9.at(i-63)->sno);
            }
        }
        if(iSaloonCam == 71){
            ui->label_cam_no->setText("COACH_"+QString::number(coach_10.at(0)->csno)+"_"+coach_10.at(0)->car_type_desc+"_"+
                                      coach_10.at(0)->camera_type_desc+"_"+QString::number(coach_10.at(0)->sno));
            camNoFileName = "COACH_"+QString::number(coach_10.at(0)->csno)+"_"+coach_10.at(0)->car_type_desc+"_"+
                    coach_10.at(0)->camera_type_desc+"_"+QString::number(coach_10.at(0)->sno);
        }
    }
    //C10
    else if(iSaloonCam>71 && iSaloonCam <80){
        for(int i = 72; i < 79 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_10.at(i-71)->csno)+"_"+coach_10.at(i-71)->car_type_desc+"_"+
                                          coach_10.at(i-71)->camera_type_desc+"_"+QString::number(coach_10.at(i-71)->sno));
                camNoFileName = "COACH_"+QString::number(coach_10.at(i-71)->csno)+"_"+coach_10.at(i-71)->car_type_desc+"_"+
                        coach_10.at(i-71)->camera_type_desc+"_"+QString::number(coach_10.at(i-71)->sno);
            }
        }
        if(iSaloonCam == 79){
            ui->label_cam_no->setText("COACH_"+QString::number(coach_11.at(0)->csno)+"_"+coach_11.at(0)->car_type_desc+"_"+
                                      coach_11.at(0)->camera_type_desc+"_"+QString::number(coach_11.at(0)->sno));
            camNoFileName = "COACH_"+QString::number(coach_11.at(0)->csno)+"_"+coach_11.at(0)->car_type_desc+"_"+
                    coach_11.at(0)->camera_type_desc+"_"+QString::number(coach_11.at(0)->sno);
        }
    }
    //C11
    else if(iSaloonCam>79 && iSaloonCam <88){
        for(int i = 80; i < 87 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_11.at(i-79)->csno)+"_"+coach_11.at(i-79)->car_type_desc+"_"+
                                          coach_11.at(i-79)->camera_type_desc+"_"+QString::number(coach_11.at(i-79)->sno));
                camNoFileName = "COACH_"+QString::number(coach_11.at(i-79)->csno)+"_"+coach_11.at(i-79)->car_type_desc+"_"+
                        coach_11.at(i-79)->camera_type_desc+"_"+QString::number(coach_11.at(i-79)->sno);
            }
        }
        if(iSaloonCam == 87){
            ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(4)->csno)+"_"+coach_12.at(4)->car_type_desc+"_"+
                                      coach_12.at(4)->camera_type_desc+"_"+QString::number(coach_12.at(0)->sno));
            camNoFileName = "COACH_"+QString::number(coach_12.at(4)->csno)+"_"+coach_12.at(4)->car_type_desc+"_"+
                    coach_12.at(4)->camera_type_desc+"_"+QString::number(coach_12.at(0)->sno);
        }
    }
    else if(iSaloonCam>87 && iSaloonCam <96){
        for(int i = 88; i < 95 ; i++){
            if(iSaloonCam == i){
                ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(i-83)->csno)+"_"+coach_12.at(i-83)->car_type_desc+"_"+
                                          coach_12.at(i-83)->camera_type_desc+"_"+QString::number(coach_12.at(i-83)->sno));
                camNoFileName = "COACH_"+QString::number(coach_12.at(i-83)->csno)+"_"+coach_12.at(i-83)->car_type_desc+"_"+
                        coach_12.at(i-83)->camera_type_desc+"_"+QString::number(coach_12.at(i-83)->sno);
            }
        }
    }
}


void MainWindow::on_pushButton_frame_6_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam].toLocal8Bit());
    recPlayer();

    ui->label_cam_no->setText("SALOON CAM NO - " + QString::number(iMosiacCam+1));
    ui->label_cam_ip->setText(listSaloonCams[iMosiacCam]);

    recordString = listSaloonCams[iMosiacCam];


    //Coach 1
    if(iMosiacCam >= 0 && iMosiacCam < 8) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_1.at(iMosiacCam+4)->csno)+"_"+coach_1.at(iMosiacCam+4)->car_type_desc+"_"+coach_1.at(iMosiacCam+4)->camera_type_desc+
                                  "_"+QString::number(coach_1.at(iMosiacCam+4)->sno));
        camNoFileName = "COACH_"+QString::number(coach_1.at(iMosiacCam+4)->csno)+"_"+coach_1.at(iMosiacCam+4)->car_type_desc+"_"+coach_1.at(iMosiacCam+4)->camera_type_desc+
                "_"+QString::number(coach_1.at(iMosiacCam+4)->sno);
    }
    //Coach 2
    if(iMosiacCam >= 8 && iMosiacCam < 16) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_2.at(iMosiacCam-8)->csno)+"_"+coach_2.at(iMosiacCam-8)->car_type_desc+"_"+coach_2.at(iMosiacCam-8)->camera_type_desc+
                                  "_"+QString::number(coach_2.at(iMosiacCam-8)->sno));
        camNoFileName = "COACH_"+QString::number(coach_2.at(iMosiacCam-8)->csno)+"_"+coach_2.at(iMosiacCam-8)->car_type_desc+"_"+coach_2.at(iMosiacCam-8)->camera_type_desc+
                "_"+QString::number(coach_2.at(iMosiacCam-8)->sno);
    }
    //Coach 3
    if(iMosiacCam >= 16 && iMosiacCam < 24) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_3.at(iMosiacCam-16)->csno)+"_"+coach_3.at(iMosiacCam-16)->car_type_desc+"_"+coach_3.at(iMosiacCam-16)->camera_type_desc+
                                  "_"+QString::number(coach_3.at(iMosiacCam-16)->sno));
        camNoFileName = "COACH_"+QString::number(coach_3.at(iMosiacCam-16)->csno)+"_"+coach_3.at(iMosiacCam-16)->car_type_desc+"_"+coach_3.at(iMosiacCam-16)->camera_type_desc+
                "_"+QString::number(coach_3.at(iMosiacCam-16)->sno);
    }
    //Coach 4
    if(iMosiacCam >= 24 && iMosiacCam < 32) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_4.at(iMosiacCam-24)->csno)+"_"+coach_4.at(iMosiacCam-24)->car_type_desc+"_"+coach_4.at(iMosiacCam-24)->camera_type_desc+
                                  "_"+QString::number(coach_4.at(iMosiacCam-24)->sno));
        camNoFileName = "COACH_"+QString::number(coach_4.at(iMosiacCam-24)->csno)+"_"+coach_4.at(iMosiacCam-24)->car_type_desc+"_"+coach_4.at(iMosiacCam-24)->camera_type_desc+
                "_"+QString::number(coach_4.at(iMosiacCam-24)->sno);
    }
    //Coach 5
    if(iMosiacCam >= 32 && iMosiacCam < 40) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_5.at(iMosiacCam-32)->csno)+"_"+coach_5.at(iMosiacCam-32)->car_type_desc+"_"+coach_5.at(iMosiacCam-32)->camera_type_desc+
                                  "_"+QString::number(coach_5.at(iMosiacCam-32)->sno));
        camNoFileName = "COACH_"+QString::number(coach_5.at(iMosiacCam-32)->csno)+"_"+coach_5.at(iMosiacCam-32)->car_type_desc+"_"+coach_5.at(iMosiacCam-32)->camera_type_desc+
                "_"+QString::number(coach_5.at(iMosiacCam-32)->sno);
    }
    //Coach 6
    if(iMosiacCam >= 40 && iMosiacCam < 48) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_6.at(iMosiacCam-40)->csno)+"_"+coach_6.at(iMosiacCam-40)->car_type_desc+"_"+coach_6.at(iMosiacCam-40)->camera_type_desc+
                                  "_"+QString::number(coach_6.at(iMosiacCam-40)->sno));
        camNoFileName = "COACH_"+QString::number(coach_6.at(iMosiacCam-40)->csno)+"_"+coach_6.at(iMosiacCam-40)->car_type_desc+"_"+coach_6.at(iMosiacCam-40)->camera_type_desc+
                "_"+QString::number(coach_6.at(iMosiacCam-40)->sno);
    }
    //Coach 7
    if(iMosiacCam >= 48 && iMosiacCam < 56) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_7.at(iMosiacCam-48)->csno)+"_"+coach_7.at(iMosiacCam-48)->car_type_desc+"_"+coach_7.at(iMosiacCam-48)->camera_type_desc+
                                  "_"+QString::number(coach_7.at(iMosiacCam-48)->sno));
        camNoFileName = "COACH_"+QString::number(coach_7.at(iMosiacCam-48)->csno)+"_"+coach_7.at(iMosiacCam-48)->car_type_desc+"_"+coach_7.at(iMosiacCam-48)->camera_type_desc+
                "_"+QString::number(coach_7.at(iMosiacCam-48)->sno);
    }
    //Coach 8
    if(iMosiacCam >= 56 && iMosiacCam < 64) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_8.at(iMosiacCam-56)->csno)+"_"+coach_8.at(iMosiacCam-56)->car_type_desc+"_"+coach_8.at(iMosiacCam-56)->camera_type_desc+
                                  "_"+QString::number(coach_8.at(iMosiacCam-56)->sno));
        camNoFileName = "COACH_"+QString::number(coach_8.at(iMosiacCam-56)->csno)+"_"+coach_8.at(iMosiacCam-56)->car_type_desc+"_"+coach_8.at(iMosiacCam-56)->camera_type_desc+
                "_"+QString::number(coach_8.at(iMosiacCam-56)->sno);
    }
    //Coach 9
    if(iMosiacCam >= 64 && iMosiacCam < 72) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_9.at(iMosiacCam-64)->csno)+"_"+coach_9.at(iMosiacCam-64)->car_type_desc+"_"+coach_9.at(iMosiacCam-64)->camera_type_desc+
                                  "_"+QString::number(coach_9.at(iMosiacCam-64)->sno));
        camNoFileName = "COACH_"+QString::number(coach_9.at(iMosiacCam-64)->csno)+"_"+coach_9.at(iMosiacCam-64)->car_type_desc+"_"+coach_9.at(iMosiacCam-64)->camera_type_desc+
                "_"+QString::number(coach_9.at(iMosiacCam-64)->sno);

    }
    //Coach 10
    if(iMosiacCam >= 72 && iMosiacCam < 80) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_10.at(iMosiacCam-72)->csno)+"_"+coach_10.at(iMosiacCam-72)->car_type_desc+"_"+coach_10.at(iMosiacCam-72)->camera_type_desc+
                                  "_"+QString::number(coach_10.at(iMosiacCam-72)->sno));
        camNoFileName = "COACH_"+QString::number(coach_10.at(iMosiacCam-72)->csno)+"_"+coach_10.at(iMosiacCam-72)->car_type_desc+"_"+coach_10.at(iMosiacCam-72)->camera_type_desc+
                "_"+QString::number(coach_10.at(iMosiacCam-72)->sno);

    }
    //Coach 11
    if(iMosiacCam >= 80 && iMosiacCam < 88) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_11.at(iMosiacCam-80)->csno)+"_"+coach_11.at(iMosiacCam-80)->car_type_desc+"_"+coach_11.at(iMosiacCam-80)->camera_type_desc+
                                  "_"+QString::number(coach_11.at(iMosiacCam-80)->sno));
        camNoFileName = "COACH_"+QString::number(coach_11.at(iMosiacCam-80)->csno)+"_"+coach_11.at(iMosiacCam-80)->car_type_desc+"_"+coach_11.at(iMosiacCam-80)->camera_type_desc+
                "_"+QString::number(coach_11.at(iMosiacCam-80)->sno);
    }
    //Coach 12
    if(iMosiacCam >= 88 && iMosiacCam < 93) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(iMosiacCam-84)->csno)+"_"+coach_12.at(iMosiacCam-84)->car_type_desc+"_"+coach_12.at(iMosiacCam-84)->camera_type_desc+
                                  "_"+QString::number(coach_12.at(iMosiacCam-84)->sno));
        camNoFileName = "COACH_"+QString::number(coach_12.at(iMosiacCam-84)->csno)+"_"+coach_12.at(iMosiacCam-84)->car_type_desc+"_"+coach_12.at(iMosiacCam-84)->camera_type_desc+
                "_"+QString::number(coach_12.at(iMosiacCam-84)->sno);

    }
    else if(iMosiacCam == 93) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(iMosiacCam-84)->csno)+"_"+coach_12.at(iMosiacCam-84)->car_type_desc+"_"+coach_12.at(iMosiacCam-84)->camera_type_desc+
                                  "_"+QString::number(coach_12.at(iMosiacCam-84)->sno));
        camNoFileName = "COACH_"+QString::number(coach_12.at(iMosiacCam-84)->csno)+"_"+coach_12.at(iMosiacCam-84)->car_type_desc+"_"+coach_12.at(iMosiacCam-84)->camera_type_desc+
                "_"+QString::number(coach_12.at(iMosiacCam-84)->sno);
    }
    else if(iMosiacCam == 94) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(iMosiacCam-84)->csno)+"_"+coach_12.at(iMosiacCam-84)->car_type_desc+"_"+coach_12.at(iMosiacCam-84)->camera_type_desc+
                                  "_"+QString::number(coach_12.at(iMosiacCam-84)->sno));
        camNoFileName = "COACH_"+QString::number(coach_12.at(iMosiacCam-84)->csno)+"_"+coach_12.at(iMosiacCam-84)->car_type_desc+"_"+coach_12.at(iMosiacCam-84)->camera_type_desc+
                "_"+QString::number(coach_12.at(iMosiacCam-84)->sno);
    }
    else if(iMosiacCam == 95) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(iMosiacCam-84)->csno)+"_"+coach_12.at(iMosiacCam-84)->car_type_desc+"_"+coach_12.at(iMosiacCam-84)->camera_type_desc+
                                  "_"+QString::number(coach_12.at(iMosiacCam-84)->sno));
        camNoFileName = "COACH_"+QString::number(coach_12.at(iMosiacCam-84)->csno)+"_"+coach_12.at(iMosiacCam-84)->car_type_desc+"_"+coach_12.at(iMosiacCam-84)->camera_type_desc+
                "_"+QString::number(coach_12.at(iMosiacCam-84)->sno);
    }
}

void MainWindow::on_pushButton_frame_7_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1].toLocal8Bit());
    recPlayer();

    ui->label_cam_ip->setText(listSaloonCams[iMosiacCam+1]);

    recordString = listSaloonCams[iMosiacCam+1];

    //Coach 1
    if(iMosiacCam >= 0 && iMosiacCam < 7) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_1.at(iMosiacCam+5)->csno)+"_"+coach_1.at(iMosiacCam+5)->car_type_desc+"_"+coach_1.at(iMosiacCam+5)->camera_type_desc+
                                  "_"+QString::number(coach_1.at(iMosiacCam+5)->sno));
        camNoFileName = "COACH_"+QString::number(coach_1.at(iMosiacCam+5)->csno)+"_"+coach_1.at(iMosiacCam+5)->car_type_desc+"_"+coach_1.at(iMosiacCam+5)->camera_type_desc+
                "_"+QString::number(coach_1.at(iMosiacCam+5)->sno);
    }

    //Coach 2
    else if(iMosiacCam >= 7 && iMosiacCam < 15) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_2.at(iMosiacCam-7)->csno)+"_"+coach_2.at(iMosiacCam-7)->car_type_desc+"_"+coach_2.at(iMosiacCam-7)->camera_type_desc+
                                  "_"+QString::number(coach_2.at(iMosiacCam-7)->sno));
        camNoFileName = "COACH_"+QString::number(coach_2.at(iMosiacCam-7)->csno)+"_"+coach_2.at(iMosiacCam-7)->car_type_desc+"_"+coach_2.at(iMosiacCam-7)->camera_type_desc+
                "_"+QString::number(coach_2.at(iMosiacCam-7)->sno);
    }

    //Coach 3
    else if(iMosiacCam >= 15 && iMosiacCam < 23) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_3.at(iMosiacCam-15)->csno)+"_"+coach_3.at(iMosiacCam-15)->car_type_desc+"_"+coach_3.at(iMosiacCam-15)->camera_type_desc+
                                  "_"+QString::number(coach_3.at(iMosiacCam-15)->sno));
        camNoFileName = "COACH_"+QString::number(coach_3.at(iMosiacCam-15)->csno)+"_"+coach_3.at(iMosiacCam-15)->car_type_desc+"_"+coach_3.at(iMosiacCam-15)->camera_type_desc+
                "_"+QString::number(coach_3.at(iMosiacCam-15)->sno);
    }

    //Coach 4
    else if(iMosiacCam >= 23 && iMosiacCam < 31) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_4.at(iMosiacCam-23)->csno)+"_"+coach_4.at(iMosiacCam-23)->car_type_desc+"_"+coach_4.at(iMosiacCam-23)->camera_type_desc+
                                  "_"+QString::number(coach_4.at(iMosiacCam-23)->sno));
        camNoFileName = "COACH_"+QString::number(coach_4.at(iMosiacCam-23)->csno)+"_"+coach_4.at(iMosiacCam-23)->car_type_desc+"_"+coach_4.at(iMosiacCam-23)->camera_type_desc+
                "_"+QString::number(coach_4.at(iMosiacCam-23)->sno);

    }

    //Coach 5
    else if(iMosiacCam >= 31 && iMosiacCam < 39) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_5.at(iMosiacCam-31)->csno)+"_"+coach_5.at(iMosiacCam-31)->car_type_desc+"_"+coach_5.at(iMosiacCam-31)->camera_type_desc+
                                  "_"+QString::number(coach_5.at(iMosiacCam-31)->sno));
        camNoFileName = "COACH_"+QString::number(coach_5.at(iMosiacCam-31)->csno)+"_"+coach_5.at(iMosiacCam-31)->car_type_desc+"_"+coach_5.at(iMosiacCam-31)->camera_type_desc+
                "_"+QString::number(coach_5.at(iMosiacCam-31)->sno);
    }
    else if(iMosiacCam == 39) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_6.at(iMosiacCam-39)->csno)+"_"+coach_6.at(iMosiacCam-39)->car_type_desc+"_"+coach_6.at(iMosiacCam-39)->camera_type_desc+
                                  "_"+QString::number(coach_6.at(iMosiacCam-39)->sno));
        camNoFileName = "COACH_"+QString::number(coach_6.at(iMosiacCam-39)->csno)+"_"+coach_6.at(iMosiacCam-39)->car_type_desc+"_"+coach_6.at(iMosiacCam-39)->camera_type_desc+
                "_"+QString::number(coach_6.at(iMosiacCam-39)->sno);
    }

    //Coach 6
    else if(iMosiacCam >= 39 && iMosiacCam < 47) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_6.at(iMosiacCam-39)->csno)+"_"+coach_6.at(iMosiacCam-39)->car_type_desc+"_"+coach_6.at(iMosiacCam-39)->camera_type_desc+
                                  "_"+QString::number(coach_6.at(iMosiacCam-39)->sno));
        camNoFileName = "COACH_"+QString::number(coach_6.at(iMosiacCam-39)->csno)+"_"+coach_6.at(iMosiacCam-39)->car_type_desc+"_"+coach_6.at(iMosiacCam-39)->camera_type_desc+
                "_"+QString::number(coach_6.at(iMosiacCam-39)->sno);
    }

    //Coach 7
    else if(iMosiacCam >= 48 && iMosiacCam < 55) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_7.at(iMosiacCam-47)->csno)+"_"+coach_7.at(iMosiacCam-47)->car_type_desc+"_"+coach_7.at(iMosiacCam-47)->camera_type_desc+
                                  "_"+QString::number(coach_7.at(iMosiacCam-47)->sno));
        camNoFileName = "COACH_"+QString::number(coach_7.at(iMosiacCam-47)->csno)+"_"+coach_7.at(iMosiacCam-47)->car_type_desc+"_"+coach_7.at(iMosiacCam-47)->camera_type_desc+
                "_"+QString::number(coach_7.at(iMosiacCam-47)->sno);
    }
    else if(iMosiacCam == 55) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_8.at(iMosiacCam-55)->csno)+"_"+coach_8.at(iMosiacCam-55)->car_type_desc+"_"+coach_8.at(iMosiacCam-55)->camera_type_desc+
                                  "_"+QString::number(coach_8.at(iMosiacCam-55)->sno));
        camNoFileName = "COACH_"+QString::number(coach_8.at(iMosiacCam-55)->csno)+"_"+coach_8.at(iMosiacCam-55)->car_type_desc+"_"+coach_8.at(iMosiacCam-55)->camera_type_desc+
                "_"+QString::number(coach_8.at(iMosiacCam-55)->sno);
    }

    //Coach 8
    else if(iMosiacCam >= 55 && iMosiacCam < 63) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_8.at(iMosiacCam-55)->csno)+"_"+coach_8.at(iMosiacCam-55)->car_type_desc+"_"+coach_8.at(iMosiacCam-55)->camera_type_desc+
                                  "_"+QString::number(coach_8.at(iMosiacCam-55)->sno));
        camNoFileName = "COACH_"+QString::number(coach_8.at(iMosiacCam-55)->csno)+"_"+coach_8.at(iMosiacCam-55)->car_type_desc+"_"+coach_8.at(iMosiacCam-55)->camera_type_desc+
                "_"+QString::number(coach_8.at(iMosiacCam-55)->sno);
    }

    //Coach 9
    else if(iMosiacCam >= 63 && iMosiacCam < 71) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_9.at(iMosiacCam-63)->csno)+"_"+coach_9.at(iMosiacCam-63)->car_type_desc+"_"+coach_9.at(iMosiacCam-63)->camera_type_desc+
                                  "_"+QString::number(coach_9.at(iMosiacCam-63)->sno));
        camNoFileName = "COACH_"+QString::number(coach_9.at(iMosiacCam-63)->csno)+"_"+coach_9.at(iMosiacCam-63)->car_type_desc+"_"+coach_9.at(iMosiacCam-63)->camera_type_desc+
                "_"+QString::number(coach_9.at(iMosiacCam-63)->sno);
    }
    //Coach 10
    else if(iMosiacCam >= 71 && iMosiacCam < 79) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_10.at(iMosiacCam-71)->csno)+"_"+coach_10.at(iMosiacCam-71)->car_type_desc+"_"+coach_10.at(iMosiacCam-71)->camera_type_desc+
                                  "_"+QString::number(coach_10.at(iMosiacCam-71)->sno));
        camNoFileName = "COACH_"+QString::number(coach_10.at(iMosiacCam-71)->csno)+"_"+coach_10.at(iMosiacCam-71)->car_type_desc+"_"+coach_10.at(iMosiacCam-71)->camera_type_desc+
                "_"+QString::number(coach_10.at(iMosiacCam-71)->sno);
    }
    else if(iMosiacCam == 79) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_11.at(iMosiacCam-79)->csno)+"_"+coach_11.at(iMosiacCam-79)->car_type_desc+"_"+coach_11.at(iMosiacCam-79)->camera_type_desc+
                                  "_"+QString::number(coach_11.at(iMosiacCam-79)->sno));
        camNoFileName = "COACH_"+QString::number(coach_11.at(iMosiacCam-79)->csno)+"_"+coach_11.at(iMosiacCam-79)->car_type_desc+"_"+coach_11.at(iMosiacCam-79)->camera_type_desc+
                "_"+QString::number(coach_11.at(iMosiacCam-79)->sno);
    }

    //Coach 11
    else if(iMosiacCam >= 79 && iMosiacCam < 87) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_11.at(iMosiacCam-79)->csno)+"_"+coach_11.at(iMosiacCam-79)->car_type_desc+"_"+coach_11.at(iMosiacCam-79)->camera_type_desc+
                                  "_"+QString::number(coach_11.at(iMosiacCam-79)->sno));
        camNoFileName = "COACH_"+QString::number(coach_11.at(iMosiacCam-79)->csno)+"_"+coach_11.at(iMosiacCam-79)->car_type_desc+"_"+coach_11.at(iMosiacCam-79)->camera_type_desc+
                "_"+QString::number(coach_11.at(iMosiacCam-79)->sno);
    }
    else if(iMosiacCam == 87) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(iMosiacCam-87)->csno)+"_"+coach_12.at(iMosiacCam-87)->car_type_desc+"_"+coach_12.at(iMosiacCam-87)->camera_type_desc+
                                  "_"+QString::number(coach_12.at(iMosiacCam-87)->sno));
        camNoFileName = "COACH_"+QString::number(coach_12.at(iMosiacCam-87)->csno)+"_"+coach_12.at(iMosiacCam-87)->car_type_desc+"_"+coach_12.at(iMosiacCam-87)->camera_type_desc+
                "_"+QString::number(coach_12.at(iMosiacCam-87)->sno);
    }

    //Coach 12
    else if(iMosiacCam >= 87 && iMosiacCam < 93) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(iMosiacCam-83)->csno)+"_"+coach_12.at(iMosiacCam-83)->car_type_desc+"_"+coach_12.at(iMosiacCam-83)->camera_type_desc+
                                  "_"+QString::number(coach_12.at(iMosiacCam-83)->sno));
        camNoFileName = "COACH_"+QString::number(coach_12.at(iMosiacCam-83)->csno)+"_"+coach_12.at(iMosiacCam-83)->car_type_desc+"_"+coach_12.at(iMosiacCam-83)->camera_type_desc+
                "_"+QString::number(coach_12.at(iMosiacCam-83)->sno);
    }
}

void MainWindow::on_pushButton_frame_8_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2].toLocal8Bit());
    recPlayer();

    ui->label_cam_ip->setText(listSaloonCams[iMosiacCam+2]);

    recordString = listSaloonCams[iMosiacCam+2];

    //Coach 1
    if(iMosiacCam >= 0 && iMosiacCam < 6) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_1.at(iMosiacCam+6)->csno)+"_"+coach_1.at(iMosiacCam+6)->car_type_desc+"_"+coach_1.at(iMosiacCam+6)->camera_type_desc+
                                  "_"+QString::number(coach_1.at(iMosiacCam+6)->sno));
        camNoFileName = "COACH_"+QString::number(coach_1.at(iMosiacCam+6)->csno)+"_"+coach_1.at(iMosiacCam+6)->car_type_desc+"_"+coach_1.at(iMosiacCam+6)->camera_type_desc+
                "_"+QString::number(coach_1.at(iMosiacCam+6)->sno);
    }
    //Coach 2
    else if(iMosiacCam > 5 && iMosiacCam < 14) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_2.at(iMosiacCam-6)->csno)+"_"+coach_2.at(iMosiacCam-6)->car_type_desc+"_"+coach_2.at(iMosiacCam-6)->camera_type_desc+
                                  "_"+QString::number(coach_2.at(iMosiacCam-6)->sno));
        camNoFileName = "COACH_"+QString::number(coach_2.at(iMosiacCam-6)->csno)+"_"+coach_2.at(iMosiacCam-6)->car_type_desc+"_"+coach_2.at(iMosiacCam-6)->camera_type_desc+
                "_"+QString::number(coach_2.at(iMosiacCam-6)->sno);
    }
    //COACH 3
    else if(iMosiacCam > 13 && iMosiacCam < 22) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_3.at(iMosiacCam-14)->csno)+"_"+coach_3.at(iMosiacCam-14)->car_type_desc+"_"+coach_3.at(iMosiacCam-14)->camera_type_desc+
                                  "_"+QString::number(coach_3.at(iMosiacCam-14)->sno));
        camNoFileName = "COACH_"+QString::number(coach_3.at(iMosiacCam-14)->csno)+"_"+coach_3.at(iMosiacCam-14)->car_type_desc+"_"+coach_3.at(iMosiacCam-14)->camera_type_desc+
                "_"+QString::number(coach_3.at(iMosiacCam-14)->sno);
    }
    //Coach 4
    else if(iMosiacCam > 21 && iMosiacCam < 30) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_4.at(iMosiacCam-22)->csno)+"_"+coach_4.at(iMosiacCam-22)->car_type_desc+"_"+coach_4.at(iMosiacCam-22)->camera_type_desc+
                                  "_"+QString::number(coach_4.at(iMosiacCam-22)->sno));
        camNoFileName = "COACH_"+QString::number(coach_4.at(iMosiacCam-22)->csno)+"_"+coach_4.at(iMosiacCam-22)->car_type_desc+"_"+coach_4.at(iMosiacCam-22)->camera_type_desc+
                "_"+QString::number(coach_4.at(iMosiacCam-22)->sno);
    }
    //Coach 5
    else if(iMosiacCam > 29 && iMosiacCam < 38) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_5.at(iMosiacCam-30)->csno)+"_"+coach_5.at(iMosiacCam-30)->car_type_desc+"_"+coach_5.at(iMosiacCam-30)->camera_type_desc+
                                  "_"+QString::number(coach_5.at(iMosiacCam-30)->sno));
        camNoFileName = "COACH_"+QString::number(coach_5.at(iMosiacCam-30)->csno)+"_"+coach_5.at(iMosiacCam-30)->car_type_desc+"_"+coach_5.at(iMosiacCam-30)->camera_type_desc+
                "_"+QString::number(coach_5.at(iMosiacCam-30)->sno);
    }
    //Coach 6
    else if(iMosiacCam > 37 && iMosiacCam < 46) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_6.at(iMosiacCam-38)->csno)+"_"+coach_6.at(iMosiacCam-38)->car_type_desc+"_"+coach_6.at(iMosiacCam-38)->camera_type_desc+
                                  "_"+QString::number(coach_6.at(iMosiacCam-38)->sno));
        camNoFileName = "COACH_"+QString::number(coach_6.at(iMosiacCam-38)->csno)+"_"+coach_6.at(iMosiacCam-38)->car_type_desc+"_"+coach_6.at(iMosiacCam-38)->camera_type_desc+
                "_"+QString::number(coach_6.at(iMosiacCam-38)->sno);
    }
    //Coach 7
    else if(iMosiacCam > 45 && iMosiacCam < 54) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_7.at(iMosiacCam-46)->csno)+"_"+coach_7.at(iMosiacCam-46)->car_type_desc+"_"+coach_7.at(iMosiacCam-46)->camera_type_desc+
                                  "_"+QString::number(coach_7.at(iMosiacCam-46)->sno));
        camNoFileName = "COACH_"+QString::number(coach_7.at(iMosiacCam-46)->csno)+"_"+coach_7.at(iMosiacCam-46)->car_type_desc+"_"+coach_7.at(iMosiacCam-46)->camera_type_desc+
                "_"+QString::number(coach_7.at(iMosiacCam-46)->sno);
    }
    //COACH 8
    else if(iMosiacCam > 53 && iMosiacCam < 62) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_8.at(iMosiacCam-54)->csno)+"_"+coach_8.at(iMosiacCam-54)->car_type_desc+"_"+coach_8.at(iMosiacCam-54)->camera_type_desc+
                                  "_"+QString::number(coach_8.at(iMosiacCam-54)->sno));
        camNoFileName = "COACH_"+QString::number(coach_8.at(iMosiacCam-54)->csno)+"_"+coach_8.at(iMosiacCam-54)->car_type_desc+"_"+coach_8.at(iMosiacCam-54)->camera_type_desc+
                "_"+QString::number(coach_8.at(iMosiacCam-54)->sno);
    }
    //COACH 9
    else if(iMosiacCam > 61 && iMosiacCam < 70) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_9.at(iMosiacCam-62)->csno)+"_"+coach_9.at(iMosiacCam-62)->car_type_desc+"_"+coach_9.at(iMosiacCam-62)->camera_type_desc+
                                  "_"+QString::number(coach_9.at(iMosiacCam-62)->sno));
        camNoFileName = "COACH_"+QString::number(coach_9.at(iMosiacCam-62)->csno)+"_"+coach_9.at(iMosiacCam-62)->car_type_desc+"_"+coach_9.at(iMosiacCam-62)->camera_type_desc+
                "_"+QString::number(coach_9.at(iMosiacCam-62)->sno);
    }
    //COACH 10
    else if(iMosiacCam > 69 && iMosiacCam < 78) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_10.at(iMosiacCam-70)->csno)+"_"+coach_10.at(iMosiacCam-70)->car_type_desc+"_"+coach_10.at(iMosiacCam-70)->camera_type_desc+
                                  "_"+QString::number(coach_10.at(iMosiacCam-70)->sno));
        camNoFileName = "COACH_"+QString::number(coach_10.at(iMosiacCam-70)->csno)+"_"+coach_10.at(iMosiacCam-70)->car_type_desc+"_"+coach_10.at(iMosiacCam-70)->camera_type_desc+
                "_"+QString::number(coach_10.at(iMosiacCam-70)->sno);
    }
    //COACH 11
    else if(iMosiacCam > 77 && iMosiacCam < 86) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_11.at(iMosiacCam-78)->csno)+"_"+coach_11.at(iMosiacCam-78)->car_type_desc+"_"+coach_11.at(iMosiacCam-78)->camera_type_desc+
                                  "_"+QString::number(coach_11.at(iMosiacCam-78)->sno));
        camNoFileName = "COACH_"+QString::number(coach_11.at(iMosiacCam-78)->csno)+"_"+coach_11.at(iMosiacCam-78)->car_type_desc+"_"+coach_11.at(iMosiacCam-78)->camera_type_desc+
                "_"+QString::number(coach_11.at(iMosiacCam-78)->sno);
    }
    //COACH 12
    else if(iMosiacCam > 85 && iMosiacCam < 93) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(iMosiacCam-82)->csno)+"_"+coach_12.at(iMosiacCam-82)->car_type_desc+"_"+coach_12.at(iMosiacCam-82)->camera_type_desc+
                                  "_"+QString::number(coach_12.at(iMosiacCam-82)->sno));
        camNoFileName = "COACH_"+QString::number(coach_12.at(iMosiacCam-82)->csno)+"_"+coach_12.at(iMosiacCam-82)->car_type_desc+"_"+coach_12.at(iMosiacCam-82)->camera_type_desc+
                "_"+QString::number(coach_12.at(iMosiacCam-82)->sno);
    }
}

void MainWindow::on_pushButton_frame_9_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3].toLocal8Bit());
    recPlayer();

    ui->label_cam_ip->setText(listSaloonCams[iMosiacCam+3]);

    recordString = listSaloonCams[iMosiacCam+3];

    //Coach 1
    if(iMosiacCam >= 0 && iMosiacCam < 5) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_1.at(iMosiacCam+7)->csno)+"_"+coach_1.at(iMosiacCam+7)->car_type_desc+"_"+coach_1.at(iMosiacCam+7)->camera_type_desc+
                                  "_"+QString::number(coach_1.at(iMosiacCam+7)->sno));
        camNoFileName = "COACH_"+QString::number(coach_1.at(iMosiacCam+7)->csno)+"_"+coach_1.at(iMosiacCam+7)->car_type_desc+"_"+coach_1.at(iMosiacCam+7)->camera_type_desc+
                "_"+QString::number(coach_1.at(iMosiacCam+7)->sno);
    }

    //Coach 2
    else if(iMosiacCam > 4 && iMosiacCam < 13) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_2.at(iMosiacCam-5)->csno)+"_"+coach_2.at(iMosiacCam-5)->car_type_desc+"_"+coach_2.at(iMosiacCam-5)->camera_type_desc+
                                  "_"+QString::number(coach_2.at(iMosiacCam-5)->sno));
        camNoFileName = "COACH_"+QString::number(coach_2.at(iMosiacCam-5)->csno)+"_"+coach_2.at(iMosiacCam-5)->car_type_desc+"_"+coach_2.at(iMosiacCam-5)->camera_type_desc+
                "_"+QString::number(coach_2.at(iMosiacCam-5)->sno);
    }
    //Coach 3
    else if(iMosiacCam > 12 && iMosiacCam < 21) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_3.at(iMosiacCam-13)->csno)+"_"+coach_3.at(iMosiacCam-13)->car_type_desc+"_"+coach_3.at(iMosiacCam-13)->camera_type_desc+
                                  "_"+QString::number(coach_3.at(iMosiacCam-13)->sno));
        camNoFileName = "COACH_"+QString::number(coach_3.at(iMosiacCam-13)->csno)+"_"+coach_3.at(iMosiacCam-13)->car_type_desc+"_"+coach_3.at(iMosiacCam-13)->camera_type_desc+
                "_"+QString::number(coach_3.at(iMosiacCam-13)->sno);
    }
    //Coach 4
    else if(iMosiacCam > 20 && iMosiacCam < 29) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_4.at(iMosiacCam-21)->csno)+"_"+coach_4.at(iMosiacCam-21)->car_type_desc+"_"+coach_4.at(iMosiacCam-21)->camera_type_desc+
                                  "_"+QString::number(coach_4.at(iMosiacCam-21)->sno));
        camNoFileName = "COACH_"+QString::number(coach_4.at(iMosiacCam-21)->csno)+"_"+coach_4.at(iMosiacCam-21)->car_type_desc+"_"+coach_4.at(iMosiacCam-21)->camera_type_desc+
                "_"+QString::number(coach_4.at(iMosiacCam-21)->sno);
    }
    //Coach 5
    else if(iMosiacCam > 28 && iMosiacCam < 37) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_5.at(iMosiacCam-29)->csno)+"_"+coach_5.at(iMosiacCam-29)->car_type_desc+"_"+coach_5.at(iMosiacCam-29)->camera_type_desc+
                                  "_"+QString::number(coach_5.at(iMosiacCam-29)->sno));
        camNoFileName = "COACH_"+QString::number(coach_5.at(iMosiacCam-29)->csno)+"_"+coach_5.at(iMosiacCam-29)->car_type_desc+"_"+coach_5.at(iMosiacCam-29)->camera_type_desc+
                "_"+QString::number(coach_5.at(iMosiacCam-29)->sno);
    }
    //Coach 6
    else if(iMosiacCam > 36 && iMosiacCam < 45) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_6.at(iMosiacCam-37)->csno)+"_"+coach_6.at(iMosiacCam-37)->car_type_desc+"_"+coach_6.at(iMosiacCam-37)->camera_type_desc+
                                  "_"+QString::number(coach_6.at(iMosiacCam-37)->sno));
        camNoFileName = "COACH_"+QString::number(coach_6.at(iMosiacCam-37)->csno)+"_"+coach_6.at(iMosiacCam-37)->car_type_desc+"_"+coach_6.at(iMosiacCam-37)->camera_type_desc+
                "_"+QString::number(coach_6.at(iMosiacCam-37)->sno);
    }
    //Coach 7
    else if(iMosiacCam > 44 && iMosiacCam < 53) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_7.at(iMosiacCam-45)->csno)+"_"+coach_7.at(iMosiacCam-45)->car_type_desc+"_"+coach_7.at(iMosiacCam-45)->camera_type_desc+
                                  "_"+QString::number(coach_7.at(iMosiacCam-45)->sno));
        camNoFileName = "COACH_"+QString::number(coach_7.at(iMosiacCam-45)->csno)+"_"+coach_7.at(iMosiacCam-45)->car_type_desc+"_"+coach_7.at(iMosiacCam-45)->camera_type_desc+
                "_"+QString::number(coach_7.at(iMosiacCam-45)->sno);
    }
    //Coach 8
    else if(iMosiacCam > 52 && iMosiacCam < 61) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_8.at(iMosiacCam-53)->csno)+"_"+coach_8.at(iMosiacCam-53)->car_type_desc+"_"+coach_8.at(iMosiacCam-53)->camera_type_desc+
                                  "_"+QString::number(coach_8.at(iMosiacCam-53)->sno));
        camNoFileName = "COACH_"+QString::number(coach_8.at(iMosiacCam-53)->csno)+"_"+coach_8.at(iMosiacCam-53)->car_type_desc+"_"+coach_8.at(iMosiacCam-53)->camera_type_desc+
                "_"+QString::number(coach_8.at(iMosiacCam-53)->sno);
    }
    //COACH 9
    else if(iMosiacCam > 60 && iMosiacCam < 69) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_9.at(iMosiacCam-61)->csno)+"_"+coach_9.at(iMosiacCam-61)->car_type_desc+"_"+coach_9.at(iMosiacCam-61)->camera_type_desc+
                                  "_"+QString::number(coach_9.at(iMosiacCam-61)->sno));
        camNoFileName = "COACH_"+QString::number(coach_9.at(iMosiacCam-61)->csno)+"_"+coach_9.at(iMosiacCam-61)->car_type_desc+"_"+coach_9.at(iMosiacCam-61)->camera_type_desc+
                "_"+QString::number(coach_9.at(iMosiacCam-61)->sno);
    }
    //Coach 10
    else if(iMosiacCam > 68 && iMosiacCam < 77) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_10.at(iMosiacCam-69)->csno)+"_"+coach_10.at(iMosiacCam-69)->car_type_desc+"_"+coach_10.at(iMosiacCam-69)->camera_type_desc+
                                  "_"+QString::number(coach_10.at(iMosiacCam-69)->sno));
        camNoFileName = "COACH_"+QString::number(coach_10.at(iMosiacCam-69)->csno)+"_"+coach_10.at(iMosiacCam-69)->car_type_desc+"_"+coach_10.at(iMosiacCam-69)->camera_type_desc+
                "_"+QString::number(coach_10.at(iMosiacCam-69)->sno);
    }
    //Coach 11
    else if(iMosiacCam > 76 && iMosiacCam < 85) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_11.at(iMosiacCam-77)->csno)+"_"+coach_11.at(iMosiacCam-77)->car_type_desc+"_"+coach_11.at(iMosiacCam-77)->camera_type_desc+
                                  "_"+QString::number(coach_11.at(iMosiacCam-77)->sno));
        camNoFileName = "COACH_"+QString::number(coach_11.at(iMosiacCam-77)->csno)+"_"+coach_11.at(iMosiacCam-77)->car_type_desc+"_"+coach_11.at(iMosiacCam-77)->camera_type_desc+
                "_"+QString::number(coach_11.at(iMosiacCam-77)->sno);
    }
    //Coach 12
    else if(iMosiacCam > 84 && iMosiacCam < 93) {
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(iMosiacCam-81)->csno)+"_"+coach_12.at(iMosiacCam-81)->car_type_desc+"_"+coach_12.at(iMosiacCam-81)->camera_type_desc+
                                  "_"+QString::number(coach_12.at(iMosiacCam-81)->sno));
        camNoFileName = "COACH_"+QString::number(coach_12.at(iMosiacCam-81)->csno)+"_"+coach_12.at(iMosiacCam-81)->car_type_desc+"_"+coach_12.at(iMosiacCam-81)->camera_type_desc+
                "_"+QString::number(coach_12.at(iMosiacCam-81)->sno);
    }
}

void MainWindow::on_pushButton_frame_10_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listAllCams[iFullCam].toLocal8Bit());
    recPlayer();

    ui->label_cam_no->setText("CAM NO - " + QString::number(iFullCam+1));
    ui->label_cam_ip->setText(listAllCams[iFullCam]);

    recordString = listAllCams[iFullCam];

    //COACH 1
    if(iFullCam >=0 && iFullCam < 4){
        ui->label_cam_no->setText("COACH_"+QString::number(coach_1.at(iFullCam)->csno)+"_"+coach_1.at(iFullCam)->car_type_desc+"_"+coach_1.at(iFullCam)->camera_type_desc);
        camNoFileName = "COACH_"+QString::number(coach_1.at(iFullCam)->csno)+"_"+coach_1.at(iFullCam)->car_type_desc+"_"+coach_1.at(iFullCam)->camera_type_desc;
    }
    else if(iFullCam >=4 && iFullCam < 12){
        ui->label_cam_no->setText("COACH_"+QString::number(coach_1.at(iFullCam)->csno)+"_"+coach_1.at(iFullCam)->car_type_desc+"_"+coach_1.at(iFullCam)->camera_type_desc
                                  +"_"+QString::number(coach_1.at(iFullCam)->sno));
        camNoFileName = "COACH_"+QString::number(coach_1.at(iFullCam)->csno)+"_"+coach_1.at(iFullCam)->car_type_desc+"_"+coach_1.at(iFullCam)->camera_type_desc
                +"_"+QString::number(coach_1.at(iFullCam)->sno);
    }

    //COACH 2
    else if(iFullCam >=12 && iFullCam < 20){
        ui->label_cam_no->setText("COACH_"+QString::number(coach_2.at(iFullCam-12)->csno)+"_"+coach_2.at(iFullCam-12)->car_type_desc+"_"+coach_2.at(iFullCam-12)->camera_type_desc+
                                  "_"+QString::number(coach_2.at(iFullCam-12)->sno));
        camNoFileName = "COACH_"+QString::number(coach_2.at(iFullCam-12)->csno)+"_"+coach_2.at(iFullCam-12)->car_type_desc+"_"+coach_2.at(iFullCam-12)->camera_type_desc+
                "_"+QString::number(coach_2.at(iFullCam-12)->sno);
    }

    //COACH 3
    else if(iFullCam >=20 && iFullCam < 28){
        ui->label_cam_no->setText("COACH_"+QString::number(coach_3.at(iFullCam-20)->csno)+"_"+coach_3.at(iFullCam-20)->car_type_desc+"_"+coach_3.at(iFullCam-20)->camera_type_desc+
                                  "_"+QString::number(coach_3.at(iFullCam-20)->sno));
        camNoFileName = "COACH_"+QString::number(coach_3.at(iFullCam-20)->csno)+"_"+coach_3.at(iFullCam-20)->car_type_desc+"_"+coach_3.at(iFullCam-20)->camera_type_desc+
                "_"+QString::number(coach_3.at(iFullCam-20)->sno);
    }

    //COACH 4
    else if(iFullCam >=28 && iFullCam < 36){
        ui->label_cam_no->setText("COACH_"+QString::number(coach_4.at(iFullCam-28)->csno)+"_"+coach_4.at(iFullCam-28)->car_type_desc+"_"+coach_4.at(iFullCam-28)->camera_type_desc+
                                  "_"+QString::number(coach_4.at(iFullCam-28)->sno));
        camNoFileName = "COACH_"+QString::number(coach_4.at(iFullCam-28)->csno)+"_"+coach_4.at(iFullCam-28)->car_type_desc+"_"+coach_4.at(iFullCam-28)->camera_type_desc+
                "_"+QString::number(coach_4.at(iFullCam-28)->sno);
    }

    //COACH 5
    else if(iFullCam >=36 && iFullCam < 44){
        ui->label_cam_no->setText("COACH_"+QString::number(coach_5.at(iFullCam-36)->csno)+"_"+coach_5.at(iFullCam-36)->car_type_desc+"_"+coach_5.at(iFullCam-36)->camera_type_desc+
                                  "_"+QString::number(coach_5.at(iFullCam-36)->sno));
        camNoFileName = "COACH_"+QString::number(coach_5.at(iFullCam-36)->csno)+"_"+coach_5.at(iFullCam-36)->car_type_desc+"_"+coach_5.at(iFullCam-36)->camera_type_desc+
                "_"+QString::number(coach_5.at(iFullCam-36)->sno);
    }

    //COACH 6
    else if(iFullCam >=44 && iFullCam < 52){
        ui->label_cam_no->setText("COACH_"+QString::number(coach_6.at(iFullCam-44)->csno)+"_"+coach_6.at(iFullCam-44)->car_type_desc+"_"+coach_6.at(iFullCam-44)->camera_type_desc+
                                  "_"+QString::number(coach_6.at(iFullCam-44)->sno));
        camNoFileName = "COACH_"+QString::number(coach_6.at(iFullCam-44)->csno)+"_"+coach_6.at(iFullCam-44)->car_type_desc+"_"+coach_6.at(iFullCam-44)->camera_type_desc+
                "_"+QString::number(coach_6.at(iFullCam-44)->sno);
    }

    //COACH 7
    else if(iFullCam >=52 && iFullCam < 60){
        ui->label_cam_no->setText("COACH_"+QString::number(coach_7.at(iFullCam-52)->csno)+"_"+coach_7.at(iFullCam-52)->car_type_desc+"_"+coach_7.at(iFullCam-52)->camera_type_desc+
                                  "_"+QString::number(coach_7.at(iFullCam-52)->sno));
        camNoFileName = "COACH_"+QString::number(coach_7.at(iFullCam-52)->csno)+"_"+coach_7.at(iFullCam-52)->car_type_desc+"_"+coach_7.at(iFullCam-52)->camera_type_desc+
                "_"+QString::number(coach_7.at(iFullCam-52)->sno);
    }

    //COACH 8
    else if(iFullCam >=60 && iFullCam < 68){
        ui->label_cam_no->setText("COACH_"+QString::number(coach_8.at(iFullCam-60)->csno)+"_"+coach_8.at(iFullCam-60)->car_type_desc+"_"+coach_8.at(iFullCam-60)->camera_type_desc+
                                  "_"+QString::number(coach_8.at(iFullCam-60)->sno));
        camNoFileName = "COACH_"+QString::number(coach_8.at(iFullCam-60)->csno)+"_"+coach_8.at(iFullCam-60)->car_type_desc+"_"+coach_8.at(iFullCam-60)->camera_type_desc+
                "_"+QString::number(coach_8.at(iFullCam-60)->sno);
    }

    //COACH 9
    else if(iFullCam >=68 && iFullCam < 76){
        ui->label_cam_no->setText("COACH_"+QString::number(coach_9.at(iFullCam-68)->csno)+"_"+coach_9.at(iFullCam-68)->car_type_desc+"_"+coach_9.at(iFullCam-68)->camera_type_desc+
                                  "_"+QString::number(coach_9.at(iFullCam-68)->sno));
        camNoFileName = "COACH_"+QString::number(coach_9.at(iFullCam-68)->csno)+"_"+coach_9.at(iFullCam-68)->car_type_desc+"_"+coach_9.at(iFullCam-68)->camera_type_desc+
                "_"+QString::number(coach_9.at(iFullCam-68)->sno);
    }

    //COACH 10
    else if(iFullCam >=76 && iFullCam < 84){
        ui->label_cam_no->setText("COACH_"+QString::number(coach_10.at(iFullCam-76)->csno)+"_"+coach_10.at(iFullCam-76)->car_type_desc+"_"+coach_10.at(iFullCam-76)->camera_type_desc+
                                  "_"+QString::number(coach_10.at(iFullCam-76)->sno));
        camNoFileName = "COACH_"+QString::number(coach_10.at(iFullCam-76)->csno)+"_"+coach_10.at(iFullCam-76)->car_type_desc+"_"+coach_10.at(iFullCam-76)->camera_type_desc+
                "_"+QString::number(coach_10.at(iFullCam-76)->sno);
    }

    //COACH 11
    else if(iFullCam >=84 && iFullCam < 92){
        ui->label_cam_no->setText("COACH_"+QString::number(coach_11.at(iFullCam-84)->csno)+"_"+coach_11.at(iFullCam-84)->car_type_desc+"_"+coach_11.at(iFullCam-84)->camera_type_desc+
                                  "_"+QString::number(coach_11.at(iFullCam-84)->sno));
        camNoFileName = "COACH_"+QString::number(coach_11.at(iFullCam-84)->csno)+"_"+coach_11.at(iFullCam-84)->car_type_desc+"_"+coach_11.at(iFullCam-84)->camera_type_desc+
                "_"+QString::number(coach_11.at(iFullCam-84)->sno);
    }

    //COACH 12
    else if(iFullCam >=92 && iFullCam < 96){
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(iFullCam-92)->csno)+"_"+coach_12.at(iFullCam-92)->car_type_desc+"_"+coach_12.at(iFullCam-92)->camera_type_desc);
        camNoFileName = "COACH_"+QString::number(coach_12.at(iFullCam-92)->csno)+"_"+coach_12.at(iFullCam-92)->car_type_desc+"_"+coach_12.at(iFullCam-92)->camera_type_desc;
    }
    else if(iFullCam >=96 && iFullCam < 104){
        ui->label_cam_no->setText("COACH_"+QString::number(coach_12.at(iFullCam-92)->csno)+"_"+coach_12.at(iFullCam-92)->car_type_desc+"_"+coach_12.at(iFullCam-92)->camera_type_desc
                                  +"_"+QString::number(coach_12.at(iFullCam-92)->sno));
        camNoFileName = "COACH_"+QString::number(coach_12.at(iFullCam-92)->csno)+"_"+coach_12.at(iFullCam-92)->car_type_desc+"_"+coach_12.at(iFullCam-92)->camera_type_desc
                +"_"+QString::number(coach_12.at(iFullCam-92)->sno);
    }
}


//Buttons on 6th page of stackWidget_Dynamic
void MainWindow::on_pushButton_record_clicked()
{
    //Initialising the timer for recording
    //    recordTimeCtr++;
    //    ui->label_timer->setText(QString::number(recordTimeCtr/60)+":"+QString::number(recordTimeCtr%60));

    recordedFileName = date_text_recording + "_" +time_text_recording;

    QString forRecordingStream = "gst-launch-1.0 -ev  rtspsrc location=" + recordString + " ! application/x-rtp, media=video, encoding-name=H264 ! queue ! rtph264depay "
                                                                                          "! h264parse ! matroskamux ! filesink location="+pathToVidArchives+date_text_recording+"_recordings/"+camNoFileName+"_"+recordedFileName+".mp4 &";
    //    QString forRecordingStream = "cvlc -vvv "+ recordString + " --sout=\"#transcode{vcodec=mp4v,vfilter=canvas{width=800,height=600}}:std{access=file,mux=mp4,dst=/home/hmi/VidArchives/24.02.2023_recordings/123.mp4}\" &";
    system(qPrintable(forRecordingStream));

    ui->label_recordingIcon->setStyleSheet("image: url(:/new/icons/recordIcon.png);");
    ui->label_recording_status->setText("RECORDING STARTED");
    ui->pushButton_record->setStyleSheet("background-color: rgb(138, 226, 52);");
    ui->pushButton_stop->setStyleSheet("background-color: rgb(211, 215, 207);");

}


void MainWindow::on_pushButton_stop_clicked()
{
    //Stopping gstreamer recording

    system("ps -A | grep gst | awk '{ printf $1 }' | xargs kill -2 $1");

    recordedFileName = "";

    ui->label_recordingIcon->setStyleSheet("");
    ui->label_recording_status->setText("RECORDING STOPPED");
    ui->pushButton_record->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->pushButton_stop->setStyleSheet("background-color: rgb(239, 41, 41);");

    //Stopping and Resetting the timer for recording
    //    recordTimer->stop();
    //    recordTimeCtr = 0;
    //    ui->label_timer->setText("");
}

//Screenshot
void MainWindow::openscreenshotdialog()
{
    Screenshot *screenshot = new Screenshot();
    screenshot->setModal(true);
    screenshot->setWindowFlag(Qt::FramelessWindowHint);
    screenshot->exec();
    timeractive.start();
}


void MainWindow::on_pushButton_screenshot_clicked()
{
    //    system("gnome-screenshot -i");
    openscreenshotdialog();
    //timer to keep the window active
    timeractive.start();
}


void MainWindow::on_pushButton_car1_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 0;
        }
    }

}

void MainWindow::on_pushButton_car2_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 1;
        }
    }
}

void MainWindow::on_pushButton_car3_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 2;
        }
    }
}

void MainWindow::on_pushButton_car4_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 3;
        }
    }
}

void MainWindow::on_pushButton_car5_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 4;
        }
    }
}

void MainWindow::on_pushButton_car6_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 5;
        }
    }
}

void MainWindow::on_pushButton_car7_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 6;
        }
    }
}

void MainWindow::on_pushButton_car8_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 7;
        }
    }
}

void MainWindow::on_pushButton_car9_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 8;
        }
    }
}

void MainWindow::on_pushButton_car10_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 9;
        }
    }
}

void MainWindow::on_pushButton_car11_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 10;
        }
    }
}

void MainWindow::on_pushButton_car12_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 11;
        }
    }
}


void MainWindow::on_pushButton_carM1_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 0;
        }
    }
}

void MainWindow::on_pushButton_carM2_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 1;
        }
    }
}

void MainWindow::on_pushButton_carM3_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 2;
        }
    }
}

void MainWindow::on_pushButton_carM4_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 3;
        }
    }
}

void MainWindow::on_pushButton_carM5_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 4;
        }
    }
}

void MainWindow::on_pushButton_carM6_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 5;
        }
    }
}

void MainWindow::on_pushButton_carM7_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 6;
        }
    }
}

void MainWindow::on_pushButton_carM8_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 7;
        }
    }
}

void MainWindow::on_pushButton_carM9_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 8;
        }
    }
}

void MainWindow::on_pushButton_carM10_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 9;
        }
    }
}

void MainWindow::on_pushButton_carM11_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 10;
        }
    }
}

void MainWindow::on_pushButton_carM12_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 11;
        }
    }
}


void MainWindow::on_pushButton_carF1_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 0;
        }
    }
}

void MainWindow::on_pushButton_carF2_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 1;
        }
    }
}

void MainWindow::on_pushButton_carF3_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 2;
        }
    }
}

void MainWindow::on_pushButton_carF4_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 3;
        }
    }
}

void MainWindow::on_pushButton_carF5_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 4;
        }
    }
}

void MainWindow::on_pushButton_carF6_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 5;
        }
    }
}

void MainWindow::on_pushButton_carF7_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 6;
        }
    }
}

void MainWindow::on_pushButton_carF8_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 7;
        }
    }
}

void MainWindow::on_pushButton_carF9_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 8;
        }
    }
}

void MainWindow::on_pushButton_carF10_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 9;
        }
    }
}

void MainWindow::on_pushButton_carF11_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 10;
        }
    }
}

void MainWindow::on_pushButton_carF12_clicked()
{
    if (loginfail == 1 || timeractive.elapsed() >= 1000){
        openlogindialog();
        if(success == 1){
            DeviceWindow *devicewindow = new DeviceWindow();
            devicewindow->setWindowFlag(Qt::FramelessWindowHint);
            //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
            QObject::connect(devicewindow, SIGNAL(homebuttonPressedDevice()), this, SLOT(openHomePage()));
            //This connect logic connects the return button press with the first page ( index 0) of stackWidget in the Device Status window
            QObject::connect(devicewindow, SIGNAL(returnbuttonPressedDevice()), this, SLOT(openMenuPagereturn()));
            devicewindow->showFullScreen();
            //timer to keep the window active
            timeractive.start();
            iterate_button_pressed = 11;
        }
    }
}

