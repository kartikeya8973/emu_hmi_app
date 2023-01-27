#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QDialog>
#include <QMediaPlayer>
#include <QVideoWidget>

#include <vlc/vlc.h>
namespace Ui {
class videoplayer;
}

class videoplayer : public QDialog
{
    Q_OBJECT

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

public:
    explicit videoplayer(QWidget *parent = nullptr);
    ~videoplayer();

    //Updating time of the video
    void updateDurationInfo(qint64 currentInfo);


private slots:
    void on_pushButton_close_clicked();

    void on_pushButton_play_clicked();

    void on_pushButton_pause_clicked();

    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void seek(int seconds);

    //For setting time
//    void SetTimeLabel();

private:
    Ui::videoplayer *ui;
    QMediaPlayer *player;
    QVideoWidget *vw ;
//    QSlider *slider;

    //Duration of the video
    qint64 m_duration;
};

#endif // VIDEOPLAYER_H
