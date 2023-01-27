#ifndef DEVICEWINDOW_H
#define DEVICEWINDOW_H

#include <QMainWindow>
#include <QTimer>

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

public slots:
    void statusDateTimeDevice();

private slots:
    void on_pushButton_home_button_clicked();

    void on_pushButton_return_clicked();

private:
    Ui::DeviceWindow *ui;
    QTimer *timer;
};

#endif // DEVICEWINDOW_H
