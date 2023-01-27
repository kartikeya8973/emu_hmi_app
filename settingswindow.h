#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

signals:
    // signal for home button(present in the info window) press
    void homebuttonPressedSettings();

public slots:
    void statusDateTimeSettings();

private slots:
    void on_pushButton_home_button_clicked();

    void on_pushButton_return_clicked();

private:
    Ui::SettingsWindow *ui;
    QTimer *timer;
};

#endif // SETTINGSWINDOW_H
