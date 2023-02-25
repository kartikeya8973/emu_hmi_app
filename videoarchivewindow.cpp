#include "videoarchivewindow.h"
#include "ui_videoarchivewindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "videoplayer.h"
#include <QDateTime>
#include <QRect>
#include <QGuiApplication>

extern QElapsedTimer timeractive;

//Global variable for filepath of video files (this path is taken on pressing event)
QString filepathmp4 = "";

VideoArchiveWindow::VideoArchiveWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VideoArchiveWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(statusDateTimeVid()));
    timer->start(100); //timer to start the data and time on the status bar
    //Adding data and time widgets to the status bar
    ui->statusbar->addPermanentWidget(ui->label_Date);
    ui->statusbar->addPermanentWidget(ui->label_Time);

    //Path of the root directory
    QString SourcePath = "/home/hmi/VidArchives/";
//    QString SourcePath = "/home/csemi/VidArchives/";
    dirmodel = new QFileSystemModel(this);
    //Displays only directories
//    dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dirmodel->setRootPath(SourcePath);
    ui->treeView->setModel(dirmodel);
    ui->treeView->setRootIndex(dirmodel->setRootPath(SourcePath));
    ui->treeView->setColumnWidth(0,400);
    ui->treeView->setColumnWidth(1,200);
    ui->treeView->setColumnWidth(2,200);
//    ui->treeView->setAlternatingRowColors(true);

//    filemodel = new QFileSystemModel(this);
//    filemodel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
//    filemodel->setRootPath(SourcePath);
//    ui->listView->setModel(filemodel);

    //Disable the buttons except home and return button
    ui->pushButton_play->setEnabled(false);
    ui->pushButton_upload->setEnabled(false);
    ui->pushButton_rename->setEnabled(false);
    ui->pushButton_delete->setEnabled(false);
    ui->pushButton_copy->setEnabled(false);
    ui->pushButton_move->setEnabled(false);
}

VideoArchiveWindow::~VideoArchiveWindow()
{
    delete ui;
}

void VideoArchiveWindow::statusDateTimeVid()
{
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString time_text = time.toString("hh : mm : ss");
    QString date_text = date.toString("dd-MM-yyyy");
    ui->label_Time->setText(time_text);
    ui->label_Date->setText(date_text);
}

void VideoArchiveWindow::on_treeView_pressed(const QModelIndex &index)
{
    //Enable the buttons on press
    filepathmp4 = dirmodel->fileInfo(index).absoluteFilePath();
    qDebug("%s", qUtf8Printable(filepathmp4));
    ui->pushButton_play->setEnabled(true);
    ui->pushButton_upload->setEnabled(true);
    ui->pushButton_rename->setEnabled(true);
    ui->pushButton_delete->setEnabled(true);
    ui->pushButton_copy->setEnabled(true);
    ui->pushButton_move->setEnabled(true);

    QByteArray ba = filepathmp4.toLocal8Bit();
    const char *file = ba.data();

    qDebug() << file;
}


//buttons in the etb window (8 buttons - home ,return ,play, upload, rename, delete, copy, move)

void VideoArchiveWindow::on_pushButton_home_button_clicked()
{
    emit homebuttonPressedVid();
    //close this window
    close();

    //timer to keep the window active
    timeractive.start();
}

//return to menu page
void VideoArchiveWindow::on_pushButton_return_clicked()
{
    emit returnbuttonPressedVid();
    //close this window
    close();

    //timer to keep the window active
    timeractive.start();
}

//Dialog window for playing videos in file system
void VideoArchiveWindow::on_pushButton_play_clicked()
{
    videoplayer *videoplayer = new class videoplayer(this);
    videoplayer->setModal(true);
    videoplayer->setWindowFlag(Qt::FramelessWindowHint);
    videoplayer->setGeometry(50,50,924,686);
    videoplayer->exec();
//    QString command="vlc " + filepathmp4;
//    system(command.toStdString().c_str());

    //timer to keep the window active
    timeractive.start();
}

void VideoArchiveWindow::on_pushButton_upload_clicked()
{
    //timer to keep the window active
    timeractive.start();
}

void VideoArchiveWindow::on_pushButton_rename_clicked()
{
    //timer to keep the window active
    timeractive.start();
}

void VideoArchiveWindow::on_pushButton_delete_clicked()
{
    //timer to keep the window active
    timeractive.start();
}

void VideoArchiveWindow::on_pushButton_copy_clicked()
{
    //timer to keep the window active
    timeractive.start();
}

void VideoArchiveWindow::on_pushButton_move_clicked()
{
    //timer to keep the window active
    timeractive.start();
}

