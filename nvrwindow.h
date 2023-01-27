#ifndef NVRWINDOW_H
#define NVRWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class NVRWindow;
}

class NVRWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NVRWindow(QWidget *parent = nullptr);
    ~NVRWindow();

signals:
    // signal for home button(present in the NVR window) press
    void homebuttonPressedNVR();

public slots:
    void statusDateTimeNVR();

private slots:
    void on_pushButton_home_button_clicked();

    void on_pushButton_return_clicked();

private:
    Ui::NVRWindow *ui;
    QTimer *timer;
};

#endif // NVRWINDOW_H
