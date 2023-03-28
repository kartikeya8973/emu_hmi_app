#ifndef DEVICEWINDOW_H
#define DEVICEWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QThread>
#include <vlc/vlc.h>


namespace Ui {
class DeviceWindow;
}

class DeviceWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DeviceWindow(QWidget *parent = nullptr);
    ~DeviceWindow();

#ifdef Q_WS_X11
    QX11EmbedContainer *_videoWidget;
#else
    //    QFrame *_videoWidget;
#endif
    // [20101215 JG] If KDE is used like unique desktop environment, only use QFrame *_videoWidget;
    bool _isPlaying;
    //libvlc_exception_t _vlcexcep; // [20101215 JG] Used for versions prior to VLC 1.2.0.
    libvlc_instance_t *_vlcinstance_deviceplayer;
    // For Camview1 - 1st Frame
    libvlc_media_player_t *_mp_deviceplayer;
    libvlc_media_t *_m_deviceplayer;

signals:
    // signal for home button(present in the device window) press
    void homebuttonPressedDevice();

    // signal for return button(present in the NVR window) press
    void returnbuttonPressedDevice();

public slots:
    void statusDateTimeDevice();

    //Function to get the status of all the IPCAMs
    void camStatus();

    //Function to get the status of all the NVRs
    void nvrStatus();

    //open screenshot window
    void openscreenshotdialog();

private slots:
    void on_pushButton_home_button_clicked();

    void on_pushButton_return_clicked();

    void on_pushButton_camStatus_clicked();

    void on_pushButton_nvrStatus_clicked();

    void on_pushButton_sendDeviceStatus_clicked();

    void on_pushButton_cam1_clicked();

    void on_pushButton_cam2_clicked();

    void on_pushButton_cam5_clicked();

    void on_pushButton_cam3_clicked();

    void on_pushButton_cam4_clicked();

    void on_pushButton_cam6_clicked();

    void on_pushButton_cam7_clicked();

    void on_pushButton_cam8_clicked();

    void on_pushButton_cam9_clicked();

    void on_pushButton_cam10_clicked();

    void on_pushButton_cam11_clicked();

    void on_pushButton_cam12_clicked();

    void on_pushButton_cam13_clicked();

    void on_pushButton_cam14_clicked();

    void on_pushButton_record_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_screenshot_clicked();

private:
    Ui::DeviceWindow *ui;
    QTimer *timer;

};

#endif // DEVICEWINDOW_H
