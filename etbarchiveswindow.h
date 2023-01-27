#ifndef ETBARCHIVESWINDOW_H
#define ETBARCHIVESWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtCore>
#include <QtGui>
#include <QFileSystemModel>
#include "audioplayer.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>

namespace Ui {
class ETBArchivesWindow;
}

class ETBArchivesWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ETBArchivesWindow(QWidget *parent = nullptr);
    ~ETBArchivesWindow();

signals:
    // signal for home button(present in the ETB window) press
//    void homebuttonPressedETB();

public slots:
    void statusDateTimeETB();

private slots:
    void on_pushButton_home_button_clicked();

    void on_treeView_pressed(const QModelIndex &index);

    void on_pushButton_play_clicked();

private:
    Ui::ETBArchivesWindow *ui;
    QTimer *timer;
    //This model will display the directories
    QFileSystemModel *dirmodel;
    QMediaPlayer *player;
    QVideoWidget *vw ;
};

#endif // ETBARCHIVESWINDOW_H
