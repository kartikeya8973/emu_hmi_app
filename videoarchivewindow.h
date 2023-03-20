#ifndef VIDEOARCHIVEWINDOW_H
#define VIDEOARCHIVEWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtCore>
#include <QtGui>
#include <QFileSystemModel>
#include "videoplayer.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>

namespace Ui {
class VideoArchiveWindow;
}

class VideoArchiveWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VideoArchiveWindow(QWidget *parent = nullptr);
    ~VideoArchiveWindow();

signals:
    // signal for home button(present in the info window) press
    void homebuttonPressedVid();

    // signal for return button(present in the ETB window) press
    void returnbuttonPressedVid();


public slots:
    void statusDateTimeVid();

    //open rename window
    void openrenamedialog();

private slots:
    void on_pushButton_home_button_clicked();

    void on_treeView_pressed(const QModelIndex &index);

    void on_pushButton_play_clicked();

    void on_pushButton_return_clicked();

    void on_pushButton_upload_clicked();

    void on_pushButton_rename_clicked();

    void on_pushButton_delete_clicked();

    void on_treeView_2_pressed(const QModelIndex &index);

    void on_pushButton_copy_move_clicked();

    void on_pushButton_copy_clicked();

    void on_pushButton_move_clicked();

    void on_pushButton_delete_2_clicked();

private:
    Ui::VideoArchiveWindow *ui;
    QTimer *timer;
    //This model will display the directories
    QFileSystemModel *dirmodel;

    //For USB copy page
    QFileSystemModel *model2;

    //For screenshot page
    QFileSystemModel *model3;




    //String to save path of file selected
//    QString filepath = "";

    QMediaPlayer *player;
    QVideoWidget *vw ;

};

#endif // VIDEOARCHIVEWINDOW_H
