#ifndef DEVICESTATUSWINDOW_H
#define DEVICESTATUSWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class DeviceStatusWindow;
}

class DeviceStatusWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DeviceStatusWindow(QWidget *parent = nullptr);
    ~DeviceStatusWindow();

signals:
    // signal for home button(present in the device status window) press
    void homebuttonPressedDevice();

public slots:
    void statusDateTimeDeviceStatus();

private slots:
    void on_pushButton_home_button_clicked();

private:
    Ui::DeviceStatusWindow *ui;
    QTimer *timer;
};

#endif // DEVICESTATUSWINDOW_H
