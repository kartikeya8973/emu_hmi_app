#ifndef ETBCALLWINDOW_H
#define ETBCALLWINDOW_H

#include <QMainWindow>
#include <vlc/vlc.h>

namespace Ui {
class EtbCallWindow;
}

class EtbCallWindow : public QMainWindow
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
    libvlc_media_player_t *_mplayer_etb;
    libvlc_media_t *_m_etb;

public:
    explicit EtbCallWindow(QWidget *parent = nullptr);
    ~EtbCallWindow();

public slots:

    //For getting rtsp streams
    void playEtbStream();

private:
    Ui::EtbCallWindow *ui;
};

#endif // ETBCALLWINDOW_H
