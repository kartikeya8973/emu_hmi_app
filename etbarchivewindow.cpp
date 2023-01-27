#include "etbarchivewindow.h"
#include "ui_etbarchivewindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "audioplayer.h"
#include <QDateTime>
#include <QRect>
#include <QGuiApplication>

extern QElapsedTimer timeractive;

//Global variable for filepath of audio files (this path is taken on pressing event)
QString filepathmp3 = "";

etbarchivewindow::etbarchivewindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::etbarchivewindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(statusDateTimeETB()));
    timer->start(100); //timer to start the data and time on the status bar
    //Adding data and time widgets to the status bar
    ui->statusbar->addPermanentWidget(ui->label_Date);
    ui->statusbar->addPermanentWidget(ui->label_Time);

    //Path of the root directory
    QString SourcePath = "/home/hmi/ETBArchives/";
//    QString SourcePath = "/home/csemi/EtbArchives/";
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

etbarchivewindow::~etbarchivewindow()
{
    delete ui;
}

void etbarchivewindow::statusDateTimeETB()
{
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString time_text = time.toString("hh : mm : ss");
    QString date_text = date.toString("dd-MM-yyyy");
    ui->label_Time->setText(time_text);
    ui->label_Date->setText(date_text);
}


void etbarchivewindow::on_treeView_pressed(const QModelIndex &index)
{
    //Enable the buttons on press
    filepathmp3 = dirmodel->fileInfo(index).absoluteFilePath();
//    qDebug("%s", qUtf8Printable(filepath));
    ui->pushButton_play->setEnabled(true);
    ui->pushButton_upload->setEnabled(true);
    ui->pushButton_rename->setEnabled(true);
    ui->pushButton_delete->setEnabled(true);
    ui->pushButton_copy->setEnabled(true);
    ui->pushButton_move->setEnabled(true);
}

//buttons in the etb window (8 buttons - home ,return ,play, upload, rename, delete, copy, move)
void etbarchivewindow::on_pushButton_home_button_clicked()
{
    emit homebuttonPressedetb();
    //Close the window
    close();
    //timer to keep the window active
    timeractive.start();
}

//return to menu page
void etbarchivewindow::on_pushButton_return_clicked()
{
    emit returnbuttonPressedetb();
    //Close the window
    close();
    //timer to keep the window active
    timeractive.start();
}

void etbarchivewindow::on_pushButton_play_clicked()
{
    audioplayer *audioplayer = new class audioplayer(this);
    audioplayer->setModal(true);
    audioplayer->setWindowFlag(Qt::FramelessWindowHint);
    audioplayer->setGeometry(50,50,924,686);
    audioplayer->exec();
//    QString command="vlc " + filepathmp3;
//    system(command.toStdString().c_str());

    //timer to keep the window active
    timeractive.start();
}

void etbarchivewindow::on_pushButton_upload_clicked()
{
    //timer to keep the window active
    timeractive.start();
}

void etbarchivewindow::on_pushButton_rename_clicked()
{
    //timer to keep the window active
    timeractive.start();
}

void etbarchivewindow::on_pushButton_delete_clicked()
{
    //timer to keep the window active
    timeractive.start();
}

void etbarchivewindow::on_pushButton_copy_clicked()
{
    //timer to keep the window active
    timeractive.start();
}

void etbarchivewindow::on_pushButton_move_clicked()
{
    //timer to keep the window active
    timeractive.start();
}

