#ifndef NVRWINDOW_H
#define NVRWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QTableView>
#include "qjsontablemodel.h"
#include <QFileSystemModel>
#include "customle.h"


namespace Ui {
class NVRWindow;
}

class NVRWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NVRWindow(QWidget *parent = nullptr);
    ~NVRWindow();

    //Function to download record list from NVR
    void downloadJson();

    //Function to download video stream from NVR
    void downloadStream();

    //Function to read json of record list and display in tableview
    void readJson();

    //opens local video archives
    void openvidArvives();

    //Fucntion to get status of HDD
    void getHddStatus();

    //Funtion to set HDD status to labels
    void setHddStatus();

    //Function to add IPCAM to NVR
    void addIPCAM();

signals:
    // signal for home button(present in the NVR window) press
    void homebuttonPressedNVR();

    // signal for return button(present in the NVR window) press
    void returnbuttonPressedNVR();

public slots:
    void statusDateTimeNVR();

    // for record list
    void replyList (QNetworkReply *replyList);

    // for video stream
    void replyStream (QNetworkReply *replyStream);

    // for hdd status
    void replyHdd (QNetworkReply *replyHdd);

    //Function to get the status of all the NVRs
    void nvrStatus();

private slots:
    void on_pushButton_home_button_clicked();

    void on_pushButton_return_clicked();

    void on_pushButton_streamList_clicked();

    void on_pushButton_downloadReloadList_clicked();

    void on_pushButton_downloadVideo_clicked();

    void on_pushButton_viewList_clicked();

    void on_pushButton_diagnostics_clicked();

    void on_pushButton_videoList_clicked();

    void on_pushButton_openDownloadedList_clicked();

    void on_tableView_streamList_pressed(const QModelIndex &index);

    void on_pushButton_NVR_IP_clicked();

    void on_pushButton_IPCAM_IP_clicked();

    void on_pushButton_Add_New_IPCAM_clicked();

    void on_pushButton_CHANNEL_NO_clicked();

    void on_pushButton_add_IPCAM_clicked();

private:
    Ui::NVRWindow *ui;
    QTimer *timer;

    // for record list
    QNetworkAccessManager *managerList;

    // for video stream
    QNetworkAccessManager *managerStream;

    // for hdd status
    QNetworkAccessManager *managerHdd;

    // for adding IPCAM to NVR
    QNetworkAccessManager *managerIpcam;

    // for Table View
    QJsonTableModel *streamList;

    //For Download Video on NVR list
    QFileSystemModel *model;

    //For USB copy page
    QFileSystemModel *model2;

    //For adding IP address of NVR
    customle *le;

    //For adding IP address of IPCAM
    customle *le2;

    //For adding channel no
    customle *le3;
};

#endif // NVRWINDOW_H
