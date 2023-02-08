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


extern QString filepath;

//return counter for main window
int returncounter_main;

int buttonpress;

// timer for keeping in check the active and inactive status on the application
QElapsedTimer timeractive;

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
    playVideo();

    //return counter set initially to 0
    returncounter_main = 0;

    //disable buttons
    ui->pushButton_camView_1->setEnabled(false);
    ui->pushButton_camView_2->setEnabled(false);
    ui->pushButton_camView_full->setEnabled(false);
    ui->pushButton_return->setEnabled(false);

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


    //Function for getting RTSP streams using libvlc
    playStream();

    etbListner = new QTcpServer(this);
    etbListner->listen(QHostAddress::Any,1999);
    connect(etbListner,SIGNAL(newConnection()),this,SLOT(etb_connected()));
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
    }

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

    //        player=new QMediaPlayer ();
    //        video=new QVideoWidget (ui->label_playVideo_3) ;
    //        video->show();
    //        player->setVideoOutput(video);
    //        player->setMedia(QUrl("gst-pipeline: udpsrc port=2246 caps=\"application/x-rtp, media=(string)video, clock-rate=(int)90000, payload=(int)96, encoding-name=(string)H264\" ! rtph264depay ! identity silent=0 ! avdec_h264 ! videoconvert ! xvimagesink name=\"qtvideosink\""));

    //        video->setGeometry(0,0,ui->label_playVideo_3->width(),ui->label_playVideo_3->height());
    //        video->setAspectRatioMode(Qt::IgnoreAspectRatio);
    //        player->play();

    //        player=new QMediaPlayer ();
    //        video=new QVideoWidget (ui->label_playVideo_4) ;
    //        video->show();
    //        player->setVideoOutput(video);
    //        player->setMedia(QUrl("gst-pipeline: udpsrc port=2247 caps=\"application/x-rtp, media=(string)video, clock-rate=(int)90000, payload=(int)96, encoding-name=(string)H264\" ! rtph264depay ! identity silent=0 ! avdec_h264 ! videoconvert ! xvimagesink name=\"qtvideosink\""));

    //        video->setGeometry(0,0,ui->label_playVideo_4->width(),ui->label_playVideo_4->height());
    //        video->setAspectRatioMode(Qt::IgnoreAspectRatio);
    //        player->play();

    //        player=new QMediaPlayer ();
    //        video=new QVideoWidget (ui->label_playVideo_5) ;
    //        video->show();
    //        player->setVideoOutput(video);
    //        player->setMedia(QUrl("gst-pipeline: udpsrc port=2248 caps=\"application/x-rtp, media=(string)video, clock-rate=(int)90000, payload=(int)96, encoding-name=(string)H264\" ! rtph264depay ! identity silent=0 ! avdec_h264 ! videoconvert ! xvimagesink name=\"qtvideosink\""));

    //        video->setGeometry(0,0,ui->label_playVideo_5->width(),ui->label_playVideo_5->height());
    //        video->setAspectRatioMode(Qt::IgnoreAspectRatio);
    //        player->play();

    //    player=new QMediaPlayer ();
    //    video=new QVideoWidget (ui->label_playVideoMosaic) ;
    //    video->show();
    //    player->setVideoOutput(video);
    //    player->setMedia(QUrl("gst-pipeline: udpsrc port=2249 caps=\"application/x-rtp, media=(string)video, clock-rate=(int)90000, payload=(int)96, encoding-name=(string)H264\" ! rtph264depay ! identity silent=0 ! avdec_h264 ! videoconvert ! xvimagesink name=\"qtvideosink\""));

    //    video->setGeometry(0,0,ui->label_playVideoMosaic->width(),ui->label_playVideoMosaic->height());
    //    video->setAspectRatioMode(Qt::IgnoreAspectRatio);
    //    player->play();

    //    player=new QMediaPlayer ();
    //    video=new QVideoWidget (ui->label_playVideoMosaic_2) ;
    //    video->show();
    //    player->setVideoOutput(video);
    //    player->setMedia(QUrl("gst-pipeline: udpsrc port=2250 caps=\"application/x-rtp, media=(string)video, clock-rate=(int)90000, payload=(int)96, encoding-name=(string)H264\" ! rtph264depay ! identity silent=0 ! avdec_h264 ! videoconvert ! xvimagesink name=\"qtvideosink\""));

    //    video->setGeometry(0,0,ui->label_playVideoMosaic_2->width(),ui->label_playVideoMosaic_2->height());
    //    video->setAspectRatioMode(Qt::IgnoreAspectRatio);
    //    player->play();

    //    player=new QMediaPlayer ();
    //    video=new QVideoWidget (ui->label_playVideoMosaic_3) ;
    //    video->show();
    //    player->setVideoOutput(video);
    //    player->setMedia(QUrl("gst-pipeline: udpsrc port=2251 caps=\"application/x-rtp, media=(string)video, clock-rate=(int)90000, payload=(int)96, encoding-name=(string)H264\" ! rtph264depay ! identity silent=0 ! avdec_h264 ! videoconvert ! xvimagesink name=\"qtvideosink\""));

    //    video->setGeometry(0,0,ui->label_playVideoMosaic_3->width(),ui->label_playVideoMosaic_3->height());
    //    video->setAspectRatioMode(Qt::IgnoreAspectRatio);
    //    player->play();

    //    player=new QMediaPlayer ();
    //    video=new QVideoWidget (ui->label_playVideoMosaic_4) ;
    //    video->show();
    //    player->setVideoOutput(video);
    //    player->setMedia(QUrl("gst-pipeline: udpsrc port=2252 caps=\"application/x-rtp, media=(string)video, clock-rate=(int)90000, payload=(int)96, encoding-name=(string)H264\" ! rtph264depay ! identity silent=0 ! avdec_h264 ! videoconvert ! xvimagesink name=\"qtvideosink\""));

    //    video->setGeometry(0,0,ui->label_playVideoMosaic_4->width(),ui->label_playVideoMosaic_4->height());
    //    video->setAspectRatioMode(Qt::IgnoreAspectRatio);
    //    player->play();


    //    player=new QMediaPlayer ();
    //    video=new QVideoWidget (ui->label_playVideo_full) ;
    //    video->show();
    //    player->setVideoOutput(video);
    //    player->setMedia(QUrl("gst-pipeline: udpsrc port=2253 caps=\"application/x-rtp, media=(string)video, clock-rate=(int)90000, payload=(int)96, encoding-name=(string)H264\" ! rtph264depay ! identity silent=0 ! avdec_h264 ! videoconvert ! xvimagesink name=\"qtvideosink\""));
    //    video->setGeometry(0,0,ui->label_playVideo_full->width(),ui->label_playVideo_full->height());
    //    video->setAspectRatioMode(Qt::IgnoreAspectRatio);
    //    player->play();
}

//=======================================================================

//=======================================================================

//=======================================================================


void MainWindow::playStream(){
    /* Create a new LibVLC media descriptor */

    const char* url =  "rtsp://192.168.1.221/video1.sdp";
    const char* url2 = "rtsp://192.168.1.223/video1.sdp";
    const char* url3 = "rtsp://192.168.1.224/video1.sdp";
    const char* url4 = "rtsp://192.168.1.225/video1.sdp";
    const char* url5 = "rtsp://192.168.1.222/video1.sdp";
    const char* url6 = "rtsp://192.168.1.227/video1.sdp";
    const char* url7 = "rtsp://192.168.1.228/video1.sdp";
    const char* url8 = "rtsp://192.168.1.229/video1.sdp";
    const char* url9 = "rtsp://192.168.1.230/video1.sdp";
    const char* url10 = "rtsp://192.168.1.231/video1.sdp";


    //    _m = libvlc_media_new_path(_vlcinstance, file.toLatin1());
    _m = libvlc_media_new_location(_vlcinstance, url);
    libvlc_media_player_set_media (_mp, _m);

    _m2 = libvlc_media_new_location(_vlcinstance, url2);
    libvlc_media_player_set_media (_mp2, _m2);

    _m3 = libvlc_media_new_location(_vlcinstance, url3);
    libvlc_media_player_set_media (_mp3, _m3);

    _m4 = libvlc_media_new_location(_vlcinstance, url4);
    libvlc_media_player_set_media (_mp4, _m4);

    _m5 = libvlc_media_new_location(_vlcinstance, url5);
    libvlc_media_player_set_media (_mp5, _m5);

    _m6 = libvlc_media_new_location(_vlcinstance, url6);
    libvlc_media_player_set_media (_mp6, _m6);

    _m7 = libvlc_media_new_location(_vlcinstance, url7);
    libvlc_media_player_set_media (_mp7, _m7);

    _m8 = libvlc_media_new_location(_vlcinstance, url8);
    libvlc_media_player_set_media (_mp8, _m8);

    _m9 = libvlc_media_new_location(_vlcinstance, url9);
    libvlc_media_player_set_media (_mp9, _m9);

    _m10 = libvlc_media_new_location(_vlcinstance, url10);
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
}

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

}

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
    //timeractive.start();
    //    player->stop();
    //    video->close();

    //    qDebug() << player->state();

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
}
//=======================================================================
//Button on the 5 camera view page
void MainWindow::on_pushButton_left_up_clicked()
{
    //timer to keep the window active
    //timeractive.start();
}
void MainWindow::on_pushButton_left_down_clicked()
{
    //timer to keep the window active
    //timeractive.start();
}
void MainWindow::on_pushButton_right_up_clicked()
{
    //timer to keep the window active
    //timeractive.start();
}
void MainWindow::on_pushButton_right_down_clicked()
{
    //timer to keep the window active
    //timeractive.start();
}
//=======================================================================

//=======================================================================
//Button on the 4 camera view page
void MainWindow::on_pushButton_left_clicked()
{
    //timer to keep the window active
    //timeractive.start();
}
void MainWindow::on_pushButton_right_clicked()
{
    //timer to keep the window active
    //timeractive.start();
}
//=======================================================================

//=======================================================================
//Button on the full camera view page
void MainWindow::on_pushButton_left_full_clicked()
{
    //timer to keep the window active
    //timeractive.start();
}
void MainWindow::on_pushButton_right_full_clicked()
{
    //timer to keep the window active
    //timeractive.start();
}
//=======================================================================

//=======================================================================















