#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "customle.h"
#include <QtSql>
#include <QFileSystemModel>

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

    //open rename window
    void openrenamedialog();

private slots:
    void on_pushButton_home_button_clicked();

    void on_pushButton_return_clicked();

    void on_pushButton_passwordSettings_clicked();

    void on_pushButton_externalStorage_clicked();

    void on_treeView_pressed(const QModelIndex &index);

    void on_pushButton_play_clicked();

    void on_pushButton_upload_clicked();

    void on_pushButton_rename_clicked();

    void on_pushButton_delete_clicked();

    void on_treeView_2_pressed(const QModelIndex &index);

    void on_pushButton_copy_clicked();

    void on_pushButton_move_clicked();

    void on_pushButton_delete_2_clicked();

    void on_pushButton_copy_move_clicked();

    void on_pushButton_updatePass_2_clicked();

private:
    Ui::SettingsWindow *ui;
    QTimer *timer;

    //This model will display the directories
    QFileSystemModel *dirmodel;

    //This model will display the directories to which we can copy/move files
    QFileSystemModel *model2;

    //For adding new driver password
    customle *le;

    //For adding new maintenance password
    customle *le2;
};

#endif // SETTINGSWINDOW_H
