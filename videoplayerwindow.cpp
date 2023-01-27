#include "videoplayerwindow.h"
#include "ui_videoplayerwindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "videoarchivewindow.h"

extern QString filepath;

VideoPlayerWindow::VideoPlayerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VideoPlayerWindow)
{
    ui->setupUi(this);
    //Using the global variable filepath defined in videoarchivewindow.cpp
    qDebug("%s", qUtf8Printable(filepath));

    player = new QMediaPlayer(this);
    vw = new QVideoWidget(ui->label_videoplayer);
    player->setVideoOutput(vw);
//    player->setMedia(QUrl("gst-pipeline: udpsrc port=9000 caps=\"application/x-rtp, media=(string)video, clock-rate=(int)90000, payload=(int)96, encoding-name=(string)H264\" ! rtph264depay ! identity silent=0 ! avdec_h264 ! videoconvert ! xvimagesink name=\"qtvideosink\""));
    player->setMedia(QUrl::fromLocalFile(filepath));
    vw->setGeometry(0,0,ui->label_videoplayer->width(),ui->label_videoplayer->height());
    vw->show();
    vw->setAspectRatioMode(Qt::IgnoreAspectRatio);
    player->play();

    slider = new QSlider(this);
    bar = new QProgressBar(this);

    ui->statusbar->addPermanentWidget(slider);
    ui->statusbar->addPermanentWidget(bar);

    slider->setOrientation(Qt::Horizontal);
//    connect(player, &QMediaPlayer::durationChanged,slider, &QSlider::setMaximum);
//    connect(player, &QMediaPlayer::durationChanged,slider, &QSlider::setValue);

//    connect(player, &QMediaPlayer::durationChanged, bar, &QProgressBar::setMaximum);
//    connect(player, &QMediaPlayer::durationChanged, bar, &QProgressBar::setValue);



}

VideoPlayerWindow::~VideoPlayerWindow()
{
    delete ui;
}

void VideoPlayerWindow::on_slider_sliderMoved(int position)
{
    qInfo() << "position is " << position ;
    player->setPosition(position);
}



