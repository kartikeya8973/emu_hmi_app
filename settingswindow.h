#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "customle.h"
#include <QtSql>

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
    // signal for home button(present in the settings window) press
    void homebuttonPressedSettings();

    // signal for return button(present in the settings window) press
    void returnbuttonPressedSettings();

public slots:
    void statusDateTimeSettings();

    void on_pushButton_updatePass_clicked();

private slots:
    void on_pushButton_home_button_clicked();

    void on_pushButton_return_clicked();

    void on_pushButton_passwordSettings_clicked();


private:
    Ui::SettingsWindow *ui;
    QTimer *timer;

    //For adding new password
    customle *le;
};

#endif // SETTINGSWINDOW_H
