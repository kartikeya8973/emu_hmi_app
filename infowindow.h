#ifndef INFOWINDOW_H
#define INFOWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class InfoWindow;
}

class InfoWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit InfoWindow(QWidget *parent = nullptr);
    ~InfoWindow();

signals:
    // signal for home button(present in the info window) press
    void homebuttonPressedInfo();
    // signal for return button(present in the info window) press
    void returnbuttonPressedInfo();

public slots:
    void statusDateTimeInfo();

private slots:
    void on_pushButton_home_button_clicked();

    void on_pushButton_return_clicked();

private:
    Ui::InfoWindow *ui;
    QTimer *timer;
};

#endif // INFOWINDOW_H
