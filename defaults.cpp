#include "defaults.h"
#include "ui_defaults.h"
#include "driverlogin.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

// timer for keeping in check the active and inactive status on the application
extern QElapsedTimer timeractive;

int buttonpress_dri;

int camVariable;

//When application starts
int launch_app = 0;
DefaultS::DefaultS(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DefaultS)
{
    ui->setupUi(this);

    //preparation of the vlc command
    const char * const vlc_args[] = {
        "--verbose=2", //be much more verbose then normal for debugging purpose
    };

#ifdef Q_WS_X11
    _videoWidget=new QX11EmbedContainer(this);
#else
    //    _videoWidget=new QFrame(this);
#endif

    _isPlaying_df=false;

    //create a new libvlc instance
    _vlcinstance=libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);  //tricky calculation of the char space used

    // Create a media player playing environement
    _dfSmp = libvlc_media_player_new (_vlcinstance);
    _dfSmp2 = libvlc_media_player_new (_vlcinstance);
    _dfSmp3 = libvlc_media_player_new (_vlcinstance);
    _dfSmp4 = libvlc_media_player_new (_vlcinstance);

    //    playStream();

    if(launch_app == 0)
    {
        playStream();
        launch_app++;
    }

    timer_play = new QTimer(this);
    timer_play->setInterval(60000); //camera rotate every 60 seconds
    connect(timer_play, SIGNAL(timeout()), this, SLOT(playStream()));
    timer_play->start(); // call playStream() every 60 seconds
}

DefaultS::~DefaultS()
{
    delete ui;

    /* Stop playing */
    libvlc_media_player_stop (_dfSmp);
    libvlc_media_player_stop (_dfSmp2);
    libvlc_media_player_stop (_dfSmp3);
    libvlc_media_player_stop (_dfSmp4);

    /* Free the media_player */
    libvlc_media_player_release (_dfSmp);
    libvlc_media_player_release (_dfSmp2);
    libvlc_media_player_release (_dfSmp3);
    libvlc_media_player_release (_dfSmp4);

    libvlc_release (_vlcinstance);
}

//Function for opening the driver login dialog

void DefaultS::opendriverlogindialog(){

    DriverLogin *driverDialog = new DriverLogin(this);
    driverDialog->setModal(true);
    driverDialog->setWindowFlag(Qt::FramelessWindowHint);
    //This connect logic connects the ok button press (with right password) with the menu page of the stack widget
    QObject::connect(driverDialog, SIGNAL(okbuttonPressed_driver()), this, SLOT(closeScreen()));
    driverDialog->exec();
    timeractive.elapsed();
}

extern int success_driver;
extern int loginfail_driver;

void DefaultS::on_pushButton_driverLogin_1_clicked()
{
    //For the first
    buttonpress_dri++;
    if (buttonpress_dri == 1){
        opendriverlogindialog();
    }

    /*this opens the driver login window again
    if window remains inactive for a minute or if driver login window is closed*/
    else if (loginfail_driver == 1 || timeractive.elapsed() >= 1000){
        opendriverlogindialog();
    }
    else if(success_driver == 1){
        closeScreen();
    }
}


void DefaultS::on_pushButton_driverLogin_2_clicked()
{
    //For the first
    buttonpress_dri++;
    if (buttonpress_dri == 1){
        opendriverlogindialog();
    }

    /*this opens the driver login window again
    if window remains inactive for a minute or if driver login window is closed*/
    else if (loginfail_driver == 1 || timeractive.elapsed() >= 1000){
        opendriverlogindialog();
    }
    else if(success_driver == 1){
        closeScreen();
    }
}


void DefaultS::on_pushButton_driverLogin_3_clicked()
{
    //For the first
    buttonpress_dri++;
    if (buttonpress_dri == 1){
        opendriverlogindialog();
    }

    /*this opens the driver login window again
    if window remains inactive for a minute or if driver login window is closed*/
    else if (loginfail_driver == 1 || timeractive.elapsed() >= 1000){
        opendriverlogindialog();
    }
    else if(success_driver == 1){
        closeScreen();
    }
}


void DefaultS::on_pushButton_driverLogin_4_clicked()
{
    //For the first
    buttonpress_dri++;
    if (buttonpress_dri == 1){
        opendriverlogindialog();
    }

    /*this opens the driver login window again
    if window remains inactive for a minute or if driver login window is closed*/
    else if (loginfail_driver == 1 || timeractive.elapsed() >= 1000){
        opendriverlogindialog();
    }
    else if(success_driver == 1){
        closeScreen();
    }
}

void DefaultS::closeScreen(){

    close();
}


QList<const char*> listAllCamsDefScreen = { "rtsp://192.168.1.221/video1.sdp", "rtsp://192.168.1.222/video1.sdp","rtsp://192.168.1.223/video1.sdp",
                                            "rtsp://192.168.1.224/video1.sdp","rtsp://192.168.1.225/video1.sdp", "rtsp://192.168.1.226/video1.sdp",
                                            "rtsp://192.168.1.227/video1.sdp","rtsp://192.168.1.228/video1.sdp", "rtsp://192.168.1.229/video1.sdp",
                                            "rtsp://192.168.1.230/video1.sdp","rtsp://192.168.1.231/video1.sdp" };

void DefaultS::playStream(){

    libvlc_media_player_stop (_dfSmp);
    _isPlaying_df=true;
    libvlc_media_player_stop (_dfSmp2);
    _isPlaying_df=true;
    libvlc_media_player_stop (_dfSmp3);
    _isPlaying_df=true;
    libvlc_media_player_stop (_dfSmp4);
    _isPlaying_df=true;


    _dfSm = libvlc_media_new_location(_vlcinstance, listAllCamsDefScreen[camVariable]);
    libvlc_media_player_set_media (_dfSmp, _dfSm);

    _dfSm2 = libvlc_media_new_location(_vlcinstance, listAllCamsDefScreen[camVariable+1]);
    libvlc_media_player_set_media (_dfSmp2, _dfSm2);

    _dfSm3 = libvlc_media_new_location(_vlcinstance, listAllCamsDefScreen[camVariable+2]);
    libvlc_media_player_set_media (_dfSmp3, _dfSm3);

    _dfSm4 = libvlc_media_new_location(_vlcinstance, listAllCamsDefScreen[camVariable+3]);
    libvlc_media_player_set_media (_dfSmp4, _dfSm4);

    camVariable ++ ;

    if(camVariable == 7/*listAllCamsDefScreen.length()-4*/){
        camVariable = 0;
    }

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_dfSmp, windid );

    int windid2 = ui->frame_2->winId();
    libvlc_media_player_set_xwindow (_dfSmp2, windid2 );

    int windid3 = ui->frame_3->winId();
    libvlc_media_player_set_xwindow (_dfSmp3, windid3 );

    int windid4 = ui->frame_4->winId();
    libvlc_media_player_set_xwindow (_dfSmp4, windid4 );

    libvlc_media_player_play (_dfSmp);
    _isPlaying_df=true;
    libvlc_media_player_play (_dfSmp2);
    _isPlaying_df=true;
    libvlc_media_player_play (_dfSmp3);
    _isPlaying_df=true;
    libvlc_media_player_play (_dfSmp4);
    _isPlaying_df=true;
}
