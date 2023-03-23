#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <QMainWindow>
#include <vlc/vlc.h>

namespace Ui {
class DefaultS;
}

class DefaultS : public QMainWindow
{
    Q_OBJECT

public:
    explicit DefaultS(QWidget *parent = nullptr);
    ~DefaultS();

#ifdef Q_WS_X11
    QX11EmbedContainer *_videoWidget;
#else
    //    QFrame *_videoWidget;
#endif
    // [20101215 JG] If KDE is used like unique desktop environment, only use QFrame *_videoWidget;
    bool _isPlaying_df;
    //libvlc_exception_t _vlcexcep; // [20101215 JG] Used for versions prior to VLC 1.2.0.
    libvlc_instance_t *_vlcinstance;
    // For Default Screens - 1st Frame
    libvlc_media_player_t *_dfSmp;
    libvlc_media_t *_dfSm;

    // For Default Screens - 2nd Frame
    libvlc_media_player_t *_dfSmp2;
    libvlc_media_t *_dfSm2;

    // For Default Screens - 3rd Frame
    libvlc_media_player_t *_dfSmp3;
    libvlc_media_t *_dfSm3;

    // For Default Screens - 4th Frame
    libvlc_media_player_t *_dfSmp4;
    libvlc_media_t *_dfSm4;

public slots:
    //open driver login dialog
    void opendriverlogindialog();

    void closeScreen();

    //For getting rtsp streams
    void playStream();

private slots:
    void on_pushButton_driverLogin_1_clicked();

    void on_pushButton_driverLogin_2_clicked();

    void on_pushButton_driverLogin_3_clicked();

    void on_pushButton_driverLogin_4_clicked();

private:
    Ui::DefaultS *ui;

    QTimer *timer_play;

};

#endif // DEFAULTS_H
