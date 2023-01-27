#include "etbarchiveswindow.h"
#include "ui_etbarchiveswindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "audioplayer.h"
#include <QDateTime>
#include <QRect>
#include <QGuiApplication>

//Global variable for filepath of audio files (this path is taken on pressing event)
QString filepathmp3 = "";

ETBArchivesWindow::ETBArchivesWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ETBArchivesWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(statusDateTimeETB()));
    timer->start(100); //timer to start the data and time on the status bar
    //Adding data and time widgets to the status bar
    ui->statusBar->addPermanentWidget(ui->label_Date);
    ui->statusBar->addPermanentWidget(ui->label_Time);
    ui->statusBar->setGeometry(0,778,1280,22);


    //Path of the root directory
    QString SourcePath = "/home/hmi/Music/";
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

ETBArchivesWindow::~ETBArchivesWindow()
{
    delete ui;
}

void ETBArchivesWindow::statusDateTimeETB()
{
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString time_text = time.toString("hh : mm : ss");
    QString date_text = date.toString("dd-MM-yyyy");
    ui->label_Time->setText(time_text);
    ui->label_Date->setText(date_text);
}

//void ETBArchivesWindow::on_pushButton_home_button_clicked()
//{
//    emit homebuttonPressedETB();
//    //Close the window
//    close();
//}


void ETBArchivesWindow::on_treeView_pressed(const QModelIndex &index)
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


void ETBArchivesWindow::on_pushButton_play_clicked()
{
    audioplayer *audioplayer = new class audioplayer(this);
    audioplayer->setModal(true);
    audioplayer->setWindowFlag(Qt::FramelessWindowHint);
    audioplayer->setGeometry(50,50,1180,700);
    audioplayer->exec();

}

