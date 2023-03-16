#ifndef DEVICEWINDOW_H
#define DEVICEWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QThread>

namespace Ui {
class DeviceWindow;
}

class DeviceWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DeviceWindow(QWidget *parent = nullptr);
    ~DeviceWindow();

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

private slots:
    void on_pushButton_home_button_clicked();

    void on_pushButton_return_clicked();

    void on_pushButton_camStatus_clicked();

    void on_pushButton_nvrStatus_clicked();

private:
    Ui::DeviceWindow *ui;
    QTimer *timer;

};

#endif // DEVICEWINDOW_H
