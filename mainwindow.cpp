#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include "logindialog.h"
#include "infowindow.h"
#include "devicewindow.h"
#include "videoarchivewindow.h"
#include "etbarchivewindow.h"
#include "nvrwindow.h"
#include "settingswindow.h"
#include "logswindow.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QFrame>
#include <vlc/libvlc.h>
#include "pingthread.h"

extern QString filepath;

//return counter for main window
int returncounter_main;

int buttonpress;

// timer for keeping in check the active and inactive status on the application
QElapsedTimer timeractive;


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

//Cam no which is  also part of the name by recorded stream is saved in VideoArhives
QString camNoFileName = "";

//String for storing date and time for directory and file creating
QString time_text_recording = "";
QString date_text_recording = "";

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
    ui->stackedWidget_Dynamic->setCurrentIndex(2);

    //For playing udp videos in labels
    //    playVideo();

    //return counter set initially to 0
    returncounter_main = 0;

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

    //Function for getting RTSP streams using libvlc
    playStream();

    etbListner = new QTcpServer(this);
    etbListner->listen(QHostAddress::Any,1999);
    connect(etbListner,SIGNAL(newConnection()),this,SLOT(etb_connected()));

    trainStopLister = new QTcpServer(this);
    trainStopLister->listen(QHostAddress::Any,3000);
    connect(trainStopLister,SIGNAL(newConnection()),this,SLOT(train_stops_connected()));


    //    recordTimer = new QTimer(this);
    //    connect(recordTimer, SIGNAL(timeout()), this, SLOT(push_button_record()));
    //    recordTimeCtr = 0;


    //Creating VidArchives folder for current date
    system("mkdir /home/hmi/VidArchives/$(date +""%Y.%m.%d"")_recordings");

    //Creatting ETBArchives folder for current date
    system("mkdir /home/hmi/ETBArchives/$(date +""%Y.%m.%d"")_recordings");

    //Initialising thread for pinging devices
    PingThread *pingthread;
    pingthread = new PingThread();
    pingthread->setObjectName("first thread");
    pingthread->start(QThread::HighestPriority);
}


MainWindow::~MainWindow()
{
    delete ui;

    /* Stop playing */
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

void MainWindow::etb_ready_read()
{
    QByteArray block = etbLocalConnection->readAll();

    block.resize(16);

    if(block.contains("ST:")) //Starting of Call to ETB
    {
        QString msg = "192.168.1.221";
        etbLocalConnection->write(msg.toLocal8Bit());
        qDebug() << msg.toLocal8Bit();

        /*Stop Stream on Different View*/
        libvlc_media_player_stop (_mp);
        _isPlaying=true;
        libvlc_media_player_stop (_mp2);
        _isPlaying=true;
        libvlc_media_player_stop (_mp3);
        _isPlaying=true;
        libvlc_media_player_stop (_mp4);
        _isPlaying=true;
        libvlc_media_player_stop (_mp5);
        _isPlaying=true;
        libvlc_media_player_stop (_mp6);
        _isPlaying=true;
        libvlc_media_player_stop (_mp7);
        _isPlaying=true;
        libvlc_media_player_stop (_mp8);
        _isPlaying=true;
        libvlc_media_player_stop (_mp9);
        _isPlaying=true;
        libvlc_media_player_stop (_mp10);
        _isPlaying=true;

        //Shifting to CCTV View when ETB call starts
        ui->stackedWidget_Dynamic->setCurrentIndex(5);

        //Starting respective CCTV feed (Hardcoded for now)
        const char* url_cctv =  "rtsp://192.168.1.221/video1.sdp";

        _m11 = libvlc_media_new_location(_vlcinstance, url_cctv);
        libvlc_media_player_set_media (_mp11, _m11);

        int windid11 = ui->frame_11->winId();
        libvlc_media_player_set_xwindow (_mp11, windid11);

        libvlc_media_player_play (_mp11);
        _isPlaying=true;
    }

    else if(block.contains("EN:")) //End of Call to ETB
    {
        //Stops CCTV feed
        libvlc_media_player_stop (_mp11);
        _isPlaying=true;

        //Returns to Default Camera View
        ui->stackedWidget_Dynamic->setCurrentIndex(2);

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
        libvlc_media_player_stop (_mp);
        _isPlaying=true;
        libvlc_media_player_stop (_mp2);
        _isPlaying=true;
        libvlc_media_player_stop (_mp3);
        _isPlaying=true;
        libvlc_media_player_stop (_mp4);
        _isPlaying=true;
        libvlc_media_player_stop (_mp5);
        _isPlaying=true;
        libvlc_media_player_stop (_mp6);
        _isPlaying=true;
        libvlc_media_player_stop (_mp7);
        _isPlaying=true;
        libvlc_media_player_stop (_mp8);
        _isPlaying=true;
        libvlc_media_player_stop (_mp9);
        _isPlaying=true;
        libvlc_media_player_stop (_mp10);
        _isPlaying=true;

        //Shifting to CCTV View when ETB call starts
        ui->stackedWidget_Dynamic->setCurrentIndex(7);

        //Starting feed of reverse view IPCAM (Hardcoded for now)
        const char* url_reverse =  "rtsp://192.168.1.224/video1.sdp";

        _m13 = libvlc_media_new_location(_vlcinstance, url_reverse);
        libvlc_media_player_set_media (_mp13, _m13);

        int windid13 = ui->frame_13->winId();
        libvlc_media_player_set_xwindow (_mp13, windid13);

        libvlc_media_player_play (_mp13);
        _isPlaying=true;
    }

    else if(block.contains("EN:")) //Ending reverse and sending HMI to default screen
    {
        //Stops reverse IPCAM feed
        libvlc_media_player_stop (_mp13);
        _isPlaying=true;

        //Returns to Default Camera View
        ui->stackedWidget_Dynamic->setCurrentIndex(2);

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

    }

}

//=======================================================================
//Menu Button
extern int success;
extern int loginfail;
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
        libvlc_media_player_stop (_mp);
        _isPlaying=true;
        libvlc_media_player_stop (_mp2);
        _isPlaying=true;
        libvlc_media_player_stop (_mp3);
        _isPlaying=true;
        libvlc_media_player_stop (_mp4);
        _isPlaying=true;
        libvlc_media_player_stop (_mp5);
        _isPlaying=true;
        libvlc_media_player_stop (_mp6);
        _isPlaying=true;
        libvlc_media_player_stop (_mp7);
        _isPlaying=true;
        libvlc_media_player_stop (_mp8);
        _isPlaying=true;
        libvlc_media_player_stop (_mp9);
        _isPlaying=true;
        libvlc_media_player_stop (_mp10);
        _isPlaying=true;
    }

    //Resetting parameters for recording page (Page 6 of stackedWidget_Dynamic)
    ui->pushButton_record->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->pushButton_stop->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->label_recording_status->setText("");
}

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
QList<const char*> listAllCams = { "rtsp://192.168.1.221/video1.sdp", "rtsp://192.168.1.222/video1.sdp","rtsp://192.168.1.223/video1.sdp",
                                   "rtsp://192.168.1.224/video1.sdp","rtsp://192.168.1.225/video1.sdp", "rtsp://192.168.1.226/video1.sdp",
                                   "rtsp://192.168.1.227/video1.sdp","rtsp://192.168.1.228/video1.sdp", "rtsp://192.168.1.229/video1.sdp",
                                   "rtsp://192.168.1.230/video1.sdp","rtsp://192.168.1.231/video1.sdp" };

QList<const char*> listExternalCams = { "rtsp://192.168.1.221/video1.sdp", "rtsp://192.168.1.224/video1.sdp", "rtsp://192.168.1.232/video1.sdp",
                                        "rtsp://192.168.1.233/video1.sdp", "rtsp://192.168.1.234/video1.sdp", "rtsp://192.168.1.235/video1.sdp" };

QList<const char*> listSaloonCams = { "rtsp://192.168.1.222/video1.sdp", "rtsp://192.168.1.223/video1.sdp", "rtsp://192.168.1.224/video1.sdp",
                                      "rtsp://192.168.1.225/video1.sdp", "rtsp://192.168.1.226/video1.sdp", "rtsp://192.168.1.227/video1.sdp",
                                      "rtsp://192.168.1.228/video1.sdp", "rtsp://192.168.1.229/video1.sdp", "rtsp://192.168.1.230/video1.sdp",
                                      "rtsp://192.168.1.231/video1.sdp", "rtsp://192.168.1.232/video1.sdp", "rtsp://192.168.1.233/video1.sdp",
                                      "rtsp://192.168.1.234/video1.sdp"};

void MainWindow::playStream(){
    /* Create a new LibVLC media descriptor */

    //    const char* url =  "rtsp://192.168.1.221/video1.sdp";
    //    const char* url2 = "rtsp://192.168.1.223/video1.sdp";
    //    const char* url3 = "rtsp://192.168.1.224/video1.sdp";
    //    const char* url4 = "rtsp://192.168.1.225/video1.sdp";
    //    const char* url5 = "rtsp://192.168.1.222/video1.sdp";
    //    const char* url6 = "rtsp://192.168.1.227/video1.sdp";
    //    const char* url7 = "rtsp://192.168.1.228/video1.sdp";
    //    const char* url8 = "rtsp://192.168.1.229/video1.sdp";
    //    const char* url9 = "rtsp://192.168.1.230/video1.sdp";
    //    const char* url10 = "rtsp://192.168.1.231/video1.sdp";

    //    _m = libvlc_media_new_path(_vlcinstance, file.toLatin1());

    //Frames for External Cameras (Front, Back and Front Left/Right and Back Left/Right Cameras
    _m = libvlc_media_new_location(_vlcinstance, listExternalCams[0]);
    libvlc_media_player_set_media (_mp, _m);

    _m2 = libvlc_media_new_location(_vlcinstance, listExternalCams[1]);
    libvlc_media_player_set_media (_mp2, _m2);

    _m3 = libvlc_media_new_location(_vlcinstance, listExternalCams[2]);
    libvlc_media_player_set_media (_mp3, _m3);

    //Frames for Saloon Cameras
    _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[0]);
    libvlc_media_player_set_media (_mp4, _m4);

    _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[1]);
    libvlc_media_player_set_media (_mp5, _m5);

    _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[0]);
    libvlc_media_player_set_media (_mp6, _m6);

    _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[1]);
    libvlc_media_player_set_media (_mp7, _m7);

    _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[2]);
    libvlc_media_player_set_media (_mp8, _m8);

    _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[3]);
    libvlc_media_player_set_media (_mp9, _m9);

    //Frame for viewing all cameras
    _m10 = libvlc_media_new_location(_vlcinstance, listAllCams[9]);
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

    /*Stop Stream on Different View*/
    libvlc_media_player_stop (_mp6);
    _isPlaying=true;
    libvlc_media_player_stop (_mp7);
    _isPlaying=true;
    libvlc_media_player_stop (_mp8);
    _isPlaying=true;
    libvlc_media_player_stop (_mp9);
    _isPlaying=true;
    libvlc_media_player_stop (_mp10);
    _isPlaying=true;
    libvlc_media_player_stop (_mp12);
    _isPlaying=true;

    //Resetting parameters for recording page (Page 6 of stackedWidget_Dynamic)
    ui->pushButton_record->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->pushButton_stop->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->label_recording_status->setText("");
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

    /*Play Stream*/
    libvlc_media_player_play (_mp6);
    _isPlaying=true;
    libvlc_media_player_play (_mp7);
    _isPlaying=true;
    libvlc_media_player_play (_mp8);
    _isPlaying=true;
    libvlc_media_player_play (_mp9);
    _isPlaying=true;

    /*Stop Stream on Different View*/
    libvlc_media_player_stop (_mp);
    _isPlaying=true;
    libvlc_media_player_stop (_mp2);
    _isPlaying=true;
    libvlc_media_player_stop (_mp3);
    _isPlaying=true;
    libvlc_media_player_stop (_mp4);
    _isPlaying=true;
    libvlc_media_player_stop (_mp5);
    _isPlaying=true;
    libvlc_media_player_stop (_mp10);
    _isPlaying=true;
    libvlc_media_player_stop (_mp12);
    _isPlaying=true;

    //Resetting parameters for recording page (Page 6 of stackedWidget_Dynamic)
    ui->pushButton_record->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->pushButton_stop->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->label_recording_status->setText("");

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

    /*Play Stream*/
    libvlc_media_player_play (_mp10);
    _isPlaying=true;

    /*Stop Stream on Different View*/
    libvlc_media_player_stop (_mp);
    _isPlaying=true;
    libvlc_media_player_stop (_mp2);
    _isPlaying=true;
    libvlc_media_player_stop (_mp3);
    _isPlaying=true;
    libvlc_media_player_stop (_mp4);
    _isPlaying=true;
    libvlc_media_player_stop (_mp5);
    _isPlaying=true;
    libvlc_media_player_stop (_mp6);
    _isPlaying=true;
    libvlc_media_player_stop (_mp7);
    _isPlaying=true;
    libvlc_media_player_stop (_mp8);
    _isPlaying=true;
    libvlc_media_player_stop (_mp9);
    _isPlaying=true;
    libvlc_media_player_stop (_mp12);
    _isPlaying=true;

    //Resetting parameters for recording page (Page 6 of stackedWidget_Dynamic)
    ui->pushButton_record->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->pushButton_stop->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->label_recording_status->setText("");

}

void MainWindow::on_pushButton_return_clicked()
{
    if(returncounter_main == 0){
        ui->stackedWidget_Dynamic->setCurrentIndex(2);
    }

    else if(returncounter_main == 1){
        ui->stackedWidget_Dynamic->setCurrentIndex(2);
        returncounter_main --;
    }

    //timer to keep the window active
    timeractive.start();
    //    player->stop();
    //    video->close();

    //    qDebug() << player->state();

    libvlc_media_player_stop (_mp12);
    _isPlaying=true;

    /*Play Stream*/
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

    //Resetting parameters for recording page (Page 6 of stackedWidget_Dynamic)
    ui->pushButton_record->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->pushButton_stop->setStyleSheet("background-color: rgb(211, 215, 207);");
    ui->label_recording_status->setText("");

}
//=======================================================================
//Function to iterate External IPCAM for 5 cam view (top 3 frames)
void MainWindow::iterateExternalCams()
{
    _m = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam]);
    libvlc_media_player_set_media (_mp, _m);

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mp, windid );

    libvlc_media_player_play (_mp);
    _isPlaying=true;

    _m2 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+1]);
    libvlc_media_player_set_media (_mp2, _m2);

    int windid2 = ui->frame_2->winId();
    libvlc_media_player_set_xwindow (_mp2, windid2 );

    libvlc_media_player_play (_mp2);
    _isPlaying=true;

    _m3 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+2]);
    libvlc_media_player_set_media (_mp3, _m3);

    int windid3 = ui->frame_3->winId();
    libvlc_media_player_set_xwindow (_mp3, windid3 );

    libvlc_media_player_play (_mp3);
    _isPlaying=true;

    //        ui->label_3->setText(QString::number(iMosiacCam));
    ui->label_frame_1->setText(listExternalCams[iExtCam]);
    ui->label_frame_2->setText(listExternalCams[iExtCam+1]);
    ui->label_frame_3->setText(listExternalCams[iExtCam+2]);
}

//Button on the 5 camera view page
void MainWindow::on_pushButton_left_up_clicked()
{
    //timer to keep the window active
    timeractive.start();

    //Stopping player
    libvlc_media_player_stop (_mp);
    _isPlaying=true;
    libvlc_media_player_stop (_mp2);
    _isPlaying=true;
    libvlc_media_player_stop (_mp3);
    _isPlaying=true;

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
    _isPlaying=true;
    libvlc_media_player_stop (_mp2);
    _isPlaying=true;
    libvlc_media_player_stop (_mp3);
    _isPlaying=true;

    if(iExtCam==listExternalCams.length() - 3) //3
    {
        iExtCam = -1;
    }

    iExtCam ++;

    iterateExternalCams();
}

//Function to iterate Saloon IPCAM for 5 cam view ( botton 2 frames)

void MainWindow::iterateSaloonCams()
{
    _m4 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam]);
    libvlc_media_player_set_media (_mp4, _m4);

    int windid4 = ui->frame_4->winId();
    libvlc_media_player_set_xwindow (_mp4, windid4 );

    libvlc_media_player_play (_mp4);
    _isPlaying=true;

    _m5 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1]);
    libvlc_media_player_set_media (_mp5, _m5);

    int windid5 = ui->frame_5->winId();
    libvlc_media_player_set_xwindow (_mp5, windid5 );

    libvlc_media_player_play (_mp5);
    _isPlaying=true;

    ui->label_frame_4->setText(listSaloonCams[iSaloonCam]);
    ui->label_frame_5->setText(listSaloonCams[iSaloonCam+1]);
}

void MainWindow::on_pushButton_left_down_clicked()
{
    //timer to keep the window active
    timeractive.start();

    //Stopping player
    libvlc_media_player_stop (_mp4);
    _isPlaying=true;
    libvlc_media_player_stop (_mp5);
    _isPlaying=true;

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

    //Stopping player
    libvlc_media_player_stop (_mp4);
    _isPlaying=true;
    libvlc_media_player_stop (_mp5);
    _isPlaying=true;

    if(iSaloonCam==listSaloonCams.length() - 2) //10
    {
        iSaloonCam = -1;
    }

    iSaloonCam ++;

    iterateSaloonCams();
}
//=======================================================================

//=======================================================================
//Function to iterate cameras in the Mosiac Cam View
void MainWindow::iterateMosiacCamView()
{
    _m6 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam]);
    libvlc_media_player_set_media (_mp6, _m6);

    int windid6 = ui->frame_6->winId();
    libvlc_media_player_set_xwindow (_mp6, windid6 );

    libvlc_media_player_play (_mp6);
    _isPlaying=true;

    _m7 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1]);
    libvlc_media_player_set_media (_mp7, _m7);

    int windid7 = ui->frame_7->winId();
    libvlc_media_player_set_xwindow (_mp7, windid7 );

    libvlc_media_player_play (_mp7);
    _isPlaying=true;

    _m8 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2]);
    libvlc_media_player_set_media (_mp8, _m8);

    int windid8 = ui->frame_8->winId();
    libvlc_media_player_set_xwindow (_mp8, windid8 );

    libvlc_media_player_play (_mp8);
    _isPlaying=true;

    _m9 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3]);
    libvlc_media_player_set_media (_mp9, _m9);

    int windid9 = ui->frame_9->winId();
    libvlc_media_player_set_xwindow (_mp9, windid9 );

    libvlc_media_player_play (_mp9);
    _isPlaying=true;

    //        ui->label_3->setText(QString::number(iMosiacCam));
    ui->label_frame_6->setText(listSaloonCams[iMosiacCam]);
    ui->label_frame_7->setText(listSaloonCams[iMosiacCam+1]);
    ui->label_frame_8->setText(listSaloonCams[iMosiacCam+2]);
    ui->label_frame_9->setText(listSaloonCams[iMosiacCam+3]);

}

//Button on the 4 camera view page
void MainWindow::on_pushButton_left_clicked()
{
    //timer to keep the window active
    timeractive.start();

    //Stopping player
    libvlc_media_player_stop (_mp6);
    _isPlaying=true;
    libvlc_media_player_stop (_mp7);
    _isPlaying=true;
    libvlc_media_player_stop (_mp8);
    _isPlaying=true;
    libvlc_media_player_stop (_mp9);
    _isPlaying=true;

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
    libvlc_media_player_stop (_mp6);
    _isPlaying=true;
    libvlc_media_player_stop (_mp7);
    _isPlaying=true;
    libvlc_media_player_stop (_mp8);
    _isPlaying=true;
    libvlc_media_player_stop (_mp9);
    _isPlaying=true;

    if(iMosiacCam==listSaloonCams.length() - 4) //8
    {
        iMosiacCam = -1;
    }

    iMosiacCam ++;

    iterateMosiacCamView();

}
//=======================================================================

//=======================================================================
//Function for iterating Full Cam View
void MainWindow::iterateFullCamView()
{
    if (iFullCam<=10)
    {
        _m10 = libvlc_media_new_location(_vlcinstance, listAllCams[iFullCam]);
        libvlc_media_player_set_media (_mp10, _m10);

        int windid10 = ui->frame_10->winId();
        libvlc_media_player_set_xwindow (_mp10, windid10 );

        libvlc_media_player_play (_mp10);
        _isPlaying=true;
    }

    ui->label_frame_10->setText(listAllCams[iFullCam]);
    ui->label_cam_no_2->setText("CAM NO - " + QString::number(iFullCam+1));


}

//Button on the full camera view page
void MainWindow::on_pushButton_left_full_clicked()
{
    //timer to keep the window active
    timeractive.start();

    //Stopping player
    libvlc_media_player_stop (_mp10);
    _isPlaying=true;

    if(iFullCam==0)
    {
        iFullCam=listAllCams.length(); // 10
    }

    iFullCam--;

    iterateFullCamView();

}

void MainWindow::on_pushButton_right_full_clicked()
{
    //timer to keep the window active
    timeractive.start();

    //Stopping player
    libvlc_media_player_stop (_mp10);
    _isPlaying=true;

    if(iFullCam==listAllCams.length() - 1) //9
    {
        iFullCam=-1;
    }

    iFullCam++;

    iterateFullCamView();

}
//=======================================================================

//=======================================================================

//Buttons for opening IPCAM stream in full view
//pushButton_frame_1 - pushButton_frame_5 for page_camView_1
//pushButton_frame_6 - pushButton_frame_9 for page_camViewMosaic
//pushButton_frame_10 for page_camViewFull

void MainWindow::on_pushButton_frame_1_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam]);
    libvlc_media_player_set_media (_mp12, _m12);

    int windid12 = ui->frame_12->winId();
    libvlc_media_player_set_xwindow (_mp12, windid12 );

    libvlc_media_player_play (_mp12);
    _isPlaying=true;

    ui->label_cam_no->setText("EXTERNAL CAM NO - " + QString::number(iExtCam+1));
    ui->label_cam_ip->setText(listExternalCams[iExtCam]);


    recordString = listExternalCams[iExtCam];

    camNoFileName = "EX_CAM_" +QString::number(iExtCam+1);

}


void MainWindow::on_pushButton_frame_2_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+1]);
    libvlc_media_player_set_media (_mp12, _m12);

    int windid12 = ui->frame_12->winId();
    libvlc_media_player_set_xwindow (_mp12, windid12 );

    libvlc_media_player_play (_mp12);
    _isPlaying=true;

    ui->label_cam_no->setText("EXTERNAL CAM NO - " + QString::number(iExtCam+2));
    ui->label_cam_ip->setText(listExternalCams[iExtCam+1]);

    recordString = listExternalCams[iExtCam+1];

    camNoFileName = "EX_CAM_" +QString::number(iExtCam+2);
}


void MainWindow::on_pushButton_frame_3_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listExternalCams[iExtCam+2]);
    libvlc_media_player_set_media (_mp12, _m12);

    int windid12 = ui->frame_12->winId();
    libvlc_media_player_set_xwindow (_mp12, windid12 );

    libvlc_media_player_play (_mp12);
    _isPlaying=true;

    ui->label_cam_no->setText("EXTERNAL CAM NO - " + QString::number(iExtCam+3));
    ui->label_cam_ip->setText(listExternalCams[iExtCam+2]);

    recordString = listExternalCams[iExtCam+2];

    camNoFileName = "EX_CAM_" +QString::number(iExtCam+3);
}


void MainWindow::on_pushButton_frame_4_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam]);
    libvlc_media_player_set_media (_mp12, _m12);

    int windid12 = ui->frame_12->winId();
    libvlc_media_player_set_xwindow (_mp12, windid12 );

    libvlc_media_player_play (_mp12);
    _isPlaying=true;

    ui->label_cam_no->setText("SALOON CAM NO - " + QString::number(iSaloonCam+1));
    ui->label_cam_ip->setText(listSaloonCams[iSaloonCam]);

    recordString = listSaloonCams[iSaloonCam];

    camNoFileName = "SA_CAM_" +QString::number(iSaloonCam+1);
}


void MainWindow::on_pushButton_frame_5_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iSaloonCam+1]);
    libvlc_media_player_set_media (_mp12, _m12);

    int windid12 = ui->frame_12->winId();
    libvlc_media_player_set_xwindow (_mp12, windid12 );

    libvlc_media_player_play (_mp12);
    _isPlaying=true;

    ui->label_cam_no->setText("SALOON CAM NO - " + QString::number(iSaloonCam+2));
    ui->label_cam_ip->setText(listSaloonCams[iSaloonCam+1]);

    recordString = listSaloonCams[iSaloonCam+1];

    camNoFileName = "SA_CAM_" +QString::number(iSaloonCam+2);
}


void MainWindow::on_pushButton_frame_6_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam]);
    libvlc_media_player_set_media (_mp12, _m12);

    int windid12 = ui->frame_12->winId();
    libvlc_media_player_set_xwindow (_mp12, windid12 );

    libvlc_media_player_play (_mp12);
    _isPlaying=true;

    ui->label_cam_no->setText("SALOON CAM NO - " + QString::number(iMosiacCam+1));
    ui->label_cam_ip->setText(listSaloonCams[iMosiacCam]);

    recordString = listSaloonCams[iMosiacCam];

    camNoFileName = "SA_CAM_" +QString::number(iMosiacCam+1);
}


void MainWindow::on_pushButton_frame_7_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+1]);
    libvlc_media_player_set_media (_mp12, _m12);

    int windid12 = ui->frame_12->winId();
    libvlc_media_player_set_xwindow (_mp12, windid12 );

    libvlc_media_player_play (_mp12);
    _isPlaying=true;

    ui->label_cam_no->setText("SALOON CAM NO - " + QString::number(iMosiacCam+2));
    ui->label_cam_ip->setText(listSaloonCams[iMosiacCam+1]);

    recordString = listSaloonCams[iMosiacCam+1];

    camNoFileName = "SA_CAM_" +QString::number(iMosiacCam+2);
}


void MainWindow::on_pushButton_frame_8_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+2]);
    libvlc_media_player_set_media (_mp12, _m12);

    int windid12 = ui->frame_12->winId();
    libvlc_media_player_set_xwindow (_mp12, windid12 );

    libvlc_media_player_play (_mp12);
    _isPlaying=true;

    ui->label_cam_no->setText("SALOON CAM NO - " + QString::number(iMosiacCam+3));
    ui->label_cam_ip->setText(listSaloonCams[iMosiacCam+2]);

    recordString = listSaloonCams[iMosiacCam+2];

    camNoFileName = "SA_CAM_" +QString::number(iMosiacCam+3);
}


void MainWindow::on_pushButton_frame_9_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listSaloonCams[iMosiacCam+3]);
    libvlc_media_player_set_media (_mp12, _m12);

    int windid12 = ui->frame_12->winId();
    libvlc_media_player_set_xwindow (_mp12, windid12 );

    libvlc_media_player_play (_mp12);
    _isPlaying=true;

    ui->label_cam_no->setText("SALOON CAM NO - " + QString::number(iMosiacCam+4));
    ui->label_cam_ip->setText(listSaloonCams[iMosiacCam+3]);

    recordString = listSaloonCams[iMosiacCam+3];

    camNoFileName = "SA_CAM_" +QString::number(iMosiacCam+4);
}

void MainWindow::on_pushButton_frame_10_clicked()
{
    ui->stackedWidget_Dynamic->setCurrentIndex(6);
    returncounter_main = 1;

    _m12 = libvlc_media_new_location(_vlcinstance, listAllCams[iFullCam]);
    libvlc_media_player_set_media (_mp12, _m12);

    int windid12 = ui->frame_12->winId();
    libvlc_media_player_set_xwindow (_mp12, windid12 );

    libvlc_media_player_play (_mp12);
    _isPlaying=true;

    ui->label_cam_no->setText("CAM NO - " + QString::number(iFullCam+1));
    ui->label_cam_ip->setText(listAllCams[iFullCam]);

    recordString = listAllCams[iFullCam];

    camNoFileName = "CAM_" +QString::number(iFullCam+1);
}


//Buttons on 6th page of stackWidget_Dynamic
void MainWindow::on_pushButton_record_clicked()
{
    //Initialising the timer for recording
    //    recordTimeCtr++;
    //    ui->label_timer->setText(QString::number(recordTimeCtr/60)+":"+QString::number(recordTimeCtr%60));

    recordedFileName = date_text_recording + "_" +time_text_recording;

    QString forRecordingStream = "gst-launch-1.0 -ev  rtspsrc location=" + recordString + " ! application/x-rtp, media=video, encoding-name=H264 ! queue ! rtph264depay "
                                                                                          "! h264parse ! matroskamux ! filesink location=/home/hmi/VidArchives/"+date_text_recording+"_recordings/"+camNoFileName+"_"+recordedFileName+".mp4 &";
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



