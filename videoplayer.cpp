#include "videoplayer.h"
#include "ui_videoplayer.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "videoarchivewindow.h"
#include <vlc/libvlc.h>


//using std::to_string;
//using std::string;

extern QString filepathmp4;

videoplayer::videoplayer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::videoplayer)
{
    ui->setupUi(this);
    //Using the global variable filepath defined in videoarchivewindow.cpp
    //    qDebug("%s", qUtf8Printable(filepathmp4));

    //    player = new QMediaPlayer(this);
    //    vw = new QVideoWidget(ui->label_videoplayer);
    //    player->setVideoOutput(vw);
    ////    player->setMedia(QUrl("gst-pipeline: udpsrc port=9000 caps=\"application/x-rtp, media=(string)video, clock-rate=(int)90000, payload=(int)96, encoding-name=(string)H264\" ! rtph264depay ! identity silent=0 ! avdec_h264 ! videoconvert ! xvimagesink name=\"qtvideosink\""));
    //    player->setMedia(QUrl::fromLocalFile(filepathmp4));
    ////     player->setMedia(QUrl("http://192.168.1.10:12060/live.flv?devid=0098004B9A&chl=9&st=0&audio=1"));
    //    vw->setGeometry(0,0,ui->label_videoplayer->width(),ui->label_videoplayer->height());
    //    vw->show();
    //    vw->setAspectRatioMode(Qt::IgnoreAspectRatio);
    //    player->play();

    ////    connect(player, &QMediaPlayer::durationChanged, ui->slider, &QSlider::setMaximum);
    ////    connect(player, &QMediaPlayer::positionChanged, ui->slider, &QSlider::setValue);
    ////    connect(ui->slider, &QSlider::sliderMoved,player, &QMediaPlayer::setPosition);

    //    //Setting the range of the slider
    //    ui->slider->setRange(0, player->duration() / 1000);

    //    connect(player, &QMediaPlayer::positionChanged, this, &videoplayer::positionChanged);
    //    connect(player, &QMediaPlayer::durationChanged, this, &videoplayer::durationChanged);
    //    connect(ui->slider, &QSlider::sliderMoved, this, &videoplayer::seek);

    //preparation of the vlc command
    const char * const vlc_args[] = {
        "", //be much more verbose then normal for debugging purpose
    };

#ifdef Q_WS_X11
    _videoWidget=new QX11EmbedContainer(this);
#else
    //    _videoWidget=new QFrame(this);
#endif

    ui->slider->setMaximum(POSITION_RESOLUTION);

    _isPlaying=false;
    poller=new QTimer(this);

    //create a new libvlc instance
    _vlcinstance=libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);  //tricky calculation of the char space used

    // Create a media player playing environement
    _mp = libvlc_media_player_new (_vlcinstance);

    playStream();

    //connect the two sliders to the corresponding slots (uses Qt's signal / slots technology)
    connect(poller, SIGNAL(timeout()), this, SLOT(updateInterface()));
    connect(ui->slider, SIGNAL(sliderMoved(int)), this, SLOT(changePosition(int)));
    connect(ui->slider, SIGNAL(sliderMoved(int)), this, SLOT(updateTime(int)));

    poller->start(100); //start timer to trigger every 100 ms the updateInterface slot

}


videoplayer::~videoplayer()
{
    delete ui;

    libvlc_media_player_stop (_mp);
    libvlc_media_player_release (_mp);
    libvlc_release (_vlcinstance);

}

void videoplayer::on_pushButton_close_clicked()
{
    //closes video player
    close();
    //For qmediaplayer
    //    player->stop();

    //For vlc mediaplayer
    libvlc_media_player_stop (_mp);
    _isPlaying=true;
}


void videoplayer::on_pushButton_play_clicked()
{
    //play the video
    //For qmediaplayer
    //    player->play();

    //For vlc mediaplayer
    libvlc_media_player_play (_mp);
    _isPlaying=true;
}

void videoplayer::on_pushButton_pause_clicked()
{
    //pause the video
    //For qmediaplayer
    //    player->pause();

    //For vlc mediaplayer
    libvlc_media_player_pause (_mp);
    _isPlaying=true;
}

//Functions used when using qmediaplayer
//###########################################################################
//class used for updating position with the player
void videoplayer::positionChanged(qint64 progress)
{
    if (!ui->slider->isSliderDown())
        ui->slider->setValue(progress / 1000);

    updateDurationInfo(progress / 1000);
}
//class used for updating duration with the player
void videoplayer::durationChanged(qint64 duration)
{
    m_duration = duration / 1000;
    ui->slider->setMaximum(m_duration);
}
//Seeks the value when the slider is manually moved
void videoplayer::seek(int seconds)
{
    player->setPosition(seconds * 1000);
}

//Updates the time duration in the time label
void videoplayer::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || m_duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
                          currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60,
                        m_duration % 60, (m_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    ui->label_time->setText(tStr);
}
//###########################################################################


//Functions used when using vlc mediaplayer
//###########################################################################
void videoplayer::playStream(){
    /* Create a new LibVLC media descriptor */

    QByteArray filePath = filepathmp4.toLocal8Bit();
    const char* file = filePath.data();

    //    _m = libvlc_media_new_path(_vlcinstance, file.toLatin1());
    _m = libvlc_media_new_path(_vlcinstance, file);
    libvlc_media_player_set_media (_mp, _m);

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

#endif

    libvlc_media_player_play (_mp);
    _isPlaying=true;

}

void videoplayer::changePosition(int newPosition)
{
    //libvlc_exception_clear(&_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.
    // It's possible that the vlc doesn't play anything
    // so check before
    libvlc_media_t *curMedia = libvlc_media_player_get_media (_mp);
    //libvlc_media_t *curMedia = libvlc_media_player_get_media (_mp, &_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.
    //libvlc_exception_clear(&_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.
    if (curMedia == NULL)
        return;

    float pos=(float)(newPosition)/(float)POSITION_RESOLUTION;
    libvlc_media_player_set_position (_mp, pos);
    //libvlc_media_player_set_position (_mp, pos, &_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.
    //raise(&_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.

    //Calling function to update time of the video when slider in moved manually
    updateTime(newPosition);

}

void videoplayer::updateInterface()
{
    if(!_isPlaying)
        return;

    // It's possible that the vlc doesn't play anything
    // so check before
    libvlc_media_t *curMedia = libvlc_media_player_get_media (_mp);
    //libvlc_media_t *curMedia = libvlc_media_player_get_media (_mp, &_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.
    //libvlc_exception_clear(&_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.
    if (curMedia == NULL)
        return;

    float pos=libvlc_media_player_get_position (_mp);
    //float pos=libvlc_media_player_get_position (_mp, &_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.
    int siderPos=(int)(pos*(float)(POSITION_RESOLUTION));
    ui->slider->setValue(siderPos);
    //    int volume=libvlc_audio_get_volume (_mp);
    //int volume=libvlc_audio_get_volume (_vlcinstance,&_vlcexcep); // [20101215 JG] Used for versions prior to VLC 1.2.0.
    //    _volumeSlider->setValue(volume);

    //    int time = libvlc_media_player_get_time(_mp);
    //    QString timeStr = QString::number(time/1000);
    //    ui->label_time->setText(timeStr);

    //For updating the time stamp when slider in not moved manually
    int position = libvlc_media_player_get_position (_mp);
    updateTime(position);

}

void videoplayer::updateTime(int currentTime)
{
    QString tStr;

    //Storing current time in seconds
    currentTime = libvlc_media_player_get_time(_mp)/1000;

    //Storing total time in seconds
    int totolTime = libvlc_media_player_get_length(_mp)/1000;

    //Conversion of seconds to hh:mm:ss format
    if (currentTime || totolTime) {
        QTime cTime((currentTime / 3600) % 60, (currentTime / 60) % 60,
                    currentTime % 60, (currentTime * 1000) % 1000);
        QTime tTime((totolTime / 3600) % 60, (totolTime / 60) % 60,
                    totolTime % 60, (totolTime * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";

        tStr = cTime.toString(format) + " / " + tTime.toString(format);
    }

    ui->label_time->setText(tStr);
}
//###########################################################################

void videoplayer::on_pushButton_screenshot_clicked()
{
    system("gnome-screenshot -i");
}

