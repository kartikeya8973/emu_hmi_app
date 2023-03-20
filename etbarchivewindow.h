#ifndef ETBARCHIVEWINDOW_H
#define ETBARCHIVEWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtCore>
#include <QtGui>
#include <QFileSystemModel>
#include "audioplayer.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>

namespace Ui {
class etbarchivewindow;
}

class etbarchivewindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit etbarchivewindow(QWidget *parent = nullptr);
    ~etbarchivewindow();

signals:
    // signal for home button(present in the ETB window) press
    void homebuttonPressedetb();

    // signal for return button(present in the ETB window) press
    void returnbuttonPressedetb();

public slots:
    void statusDateTimeETB();

    //open rename window
    void openrenamedialog();

private slots:

    void on_pushButton_play_clicked();

    void on_pushButton_home_button_clicked();

    void on_treeView_pressed(const QModelIndex &index);

    void on_pushButton_return_clicked();

    void on_pushButton_upload_clicked();

    void on_pushButton_rename_clicked();

    void on_pushButton_delete_clicked();

    void on_pushButton_copy_clicked();

    void on_pushButton_move_clicked();

    void on_pushButton_copy_move_clicked();

    void on_treeView_2_pressed(const QModelIndex &index);

    void on_pushButton_delete_2_clicked();

private:
    Ui::etbarchivewindow *ui;
    QTimer *timer;
    //This model will display the directories
    QFileSystemModel *dirmodel;
    QMediaPlayer *player;
    QVideoWidget *vw ;

    //For USB copy page
    QFileSystemModel *model2;

};

#endif // ETBARCHIVEWINDOW_H
