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

//Global variable for filename of audio files (this path is taken on pressing event)
QString filenamemp3 = "";

//Global variable for filepath of USB
QString usbpath_mp3 = "";

//Global variable for filename selected from USB
QString usbfilename_mp3 = "";

//counter for return button for etb (audio) archives window
int returncounter_etb;

etbarchivewindow::etbarchivewindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::etbarchivewindow)
{
    ui->setupUi(this);
     returncounter_etb = 0;
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
    ui->treeView->setColumnWidth(0,500);
    ui->treeView->setColumnWidth(1,120);
    ui->treeView->setColumnWidth(2,120);
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
    ui->pushButton_copy_move->setEnabled(false);

    model2 = new QFileSystemModel(this);
    ui->treeView_2->setModel(model2);
    ui->treeView_2->setRootIndex(model2->setRootPath("/media/hmi/"));
//    ui->treeView_2->setRootIndex(model2->setRootPath("/media/csemi/"));
    ui->treeView_2->setColumnWidth(0,500);
    ui->treeView_2->setColumnWidth(1,120);
    ui->treeView_2->setColumnWidth(2,120);

    ui->label_status->setWordWrap(true);
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
    filenamemp3 = dirmodel->fileInfo(index).fileName();
    ui->pushButton_play->setEnabled(true);
    ui->pushButton_upload->setEnabled(true);
    ui->pushButton_rename->setEnabled(true);
    ui->pushButton_delete->setEnabled(true);
    ui->pushButton_copy_move->setEnabled(true);

    QByteArray ba = filepathmp3.toLocal8Bit();
    const char *file = ba.data();

    qDebug() << file;
}

//buttons in the etb window (7 buttons - home ,return ,play, upload, rename, delete, copy/move)
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
    if (returncounter_etb == 1){
        ui->stackedWidget->setCurrentIndex(0);
        returncounter_etb --;
    }
    //returns to menu page of the Mainwindow
    else if(returncounter_etb == 0){
        emit returnbuttonPressedetb();
        close();
    }

    ui->label_heading->setText(" ETB Archives");
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

    QString delete_file = " rm " + filepathmp3;

    system(qPrintable(delete_file));
}

void etbarchivewindow::on_pushButton_copy_move_clicked()
{
    //Opens the USB copy and move page
    ui->stackedWidget->setCurrentIndex(1);
    returncounter_etb = 1;

    ui->label_heading->setText(" USB / External Storage List");
    ui->label_status->setText("Select the USB or Harddisk");
}

void etbarchivewindow::on_treeView_2_pressed(const QModelIndex &index)
{
    usbpath_mp3 = model2->fileInfo(index).absoluteFilePath();
    usbfilename_mp3 = model2->fileInfo(index).fileName();

    ui->label_status->setText(usbpath_mp3);
}

void etbarchivewindow::on_pushButton_copy_clicked()
{
    //timer to keep the window active
    timeractive.start();
    QString copy_file_to_usb = " cp " + filepathmp3 + " " + usbpath_mp3;

    qDebug() << copy_file_to_usb;

    system(qPrintable(copy_file_to_usb));

    ui->label_status->setText(filenamemp3 + " copied to " + usbpath_mp3 );
    QTimer::singleShot(5000, this, [this] () { ui->label_status->setText(""); });
}

void etbarchivewindow::on_pushButton_move_clicked()
{
    //timer to keep the window active
    timeractive.start();
    QString move_file_to_usb = " mv " + filepathmp3 + " " + usbpath_mp3;

    qDebug() << move_file_to_usb;

    system(qPrintable(move_file_to_usb));

    ui->label_status->setText(filenamemp3 + " moved to " + usbpath_mp3 );
    QTimer::singleShot(5000, this, [this] () { ui->label_status->setText(""); });
}

void etbarchivewindow::on_pushButton_delete_2_clicked()
{
    QString delete_file_on_usb = " rm " + usbpath_mp3;

    qDebug() << delete_file_on_usb;

    system(qPrintable(delete_file_on_usb));

    ui->label_status->setText( usbfilename_mp3 + " deleted ");
    QTimer::singleShot(5000, this, [this] () { ui->label_status->setText(""); });
}




