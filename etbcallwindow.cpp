#include "etbcallwindow.h"
#include "ui_etbcallwindow.h"
#include "mainwindow.h"

EtbCallWindow::EtbCallWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EtbCallWindow)
{
    ui->setupUi(this);
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
    _mplayer_etb = libvlc_media_player_new (_vlcinstance);

    playEtbStream();
}

EtbCallWindow::~EtbCallWindow()
{
    delete ui;

    /* Stop playing */
    libvlc_media_player_stop (_mplayer_etb);
    /* Free the media_player */
    libvlc_media_player_release (_mplayer_etb);

}

void EtbCallWindow::playEtbStream(){
    //Starting respective CCTV feed (Hardcoded for now)
    const char* url_cctv =  "rtsp://192.168.1.221/video1.sdp";

    _m_etb = libvlc_media_new_location(_vlcinstance, url_cctv);
    libvlc_media_player_set_media (_mplayer_etb, _m_etb);

    int windid = ui->frame->winId();
    libvlc_media_player_set_xwindow (_mplayer_etb, windid);

    libvlc_media_player_play (_mplayer_etb);
    _isPlaying=true;

}
