#include "audioplayer.h"
#include "ui_audioplayer.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "etbarchiveswindow.h"

extern QString filepathmp3;

audioplayer::audioplayer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::audioplayer)
{
    ui->setupUi(this);
    //Using the global variable filepath defined in videoarchivewindow.cpp
    //    qDebug("%s", qUtf8Printable(filepathmp4));


    player = new QMediaPlayer(this);
    vw = new QVideoWidget(ui->label_videoplayer);
    player->setVideoOutput(vw);
    //    player->setMedia(QUrl("gst-pipeline: udpsrc port=9000 caps=\"application/x-rtp, media=(string)video, clock-rate=(int)90000, payload=(int)96, encoding-name=(string)H264\" ! rtph264depay ! identity silent=0 ! avdec_h264 ! videoconvert ! xvimagesink name=\"qtvideosink\""));
    player->setMedia(QUrl::fromLocalFile(filepathmp3));
    vw->setGeometry(0,0,ui->label_videoplayer->width(),ui->label_videoplayer->height());
    vw->show();
    vw->setAspectRatioMode(Qt::IgnoreAspectRatio);
    player->play();

    //    connect(player, &QMediaPlayer::durationChanged, ui->slider, &QSlider::setMaximum);
    //    connect(player, &QMediaPlayer::positionChanged, ui->slider, &QSlider::setValue);
    //    connect(ui->slider, &QSlider::sliderMoved,player, &QMediaPlayer::setPosition);

    //Setting the range of the slider
    ui->slider->setRange(0, player->duration() / 1000);

    connect(player, &QMediaPlayer::positionChanged, this, &audioplayer::positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &audioplayer::durationChanged);
    connect(ui->slider, &QSlider::sliderMoved, this, &audioplayer::seek);

}

audioplayer::~audioplayer()
{
    delete ui;
}

void audioplayer::on_pushButton_play_clicked()
{
    //play the audio
    player->play();
}


void audioplayer::on_pushButton_pause_clicked()
{
    //pause the audio
    player->pause();
}

void audioplayer::on_pushButton_close_clicked()
{
    //closes audio player
    close();
    player->stop();
}

//class used for updating position with the player
void audioplayer::positionChanged(qint64 progress)
{
    if (!ui->slider->isSliderDown())
        ui->slider->setValue(progress / 1000);

    updateDurationInfo(progress / 1000);
}
//class used for updating duration with the player
void audioplayer::durationChanged(qint64 duration)
{
    m_duration = duration / 1000;
    ui->slider->setMaximum(m_duration);
}
//Seeks the value when the slider is manually moved
void audioplayer::seek(int seconds)
{
    player->setPosition(seconds * 1000);
}

//Updates the time duration in the time label
void audioplayer::updateDurationInfo(qint64 currentInfo)
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



