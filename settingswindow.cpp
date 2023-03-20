#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include "renamewindow.h"

extern QSqlDatabase passdb;
extern QElapsedTimer timeractive;

extern QString new_name;

//counter for return button for settings window
int returncounter_settings;

//Global variable for absolute filepath
QString fileabspath = "";

//Global variable for filepath
QString filepath = "";

//Global variable for filename
QString filename = "";

//Global variable for filepath of folder
QString usbpath_settings = "";

//Global variable for filename selected from folder
QString usbfilename_settings = "";

//for videoplayer - tell that settings window is accessing it
int settingvidplayer;

//for rename dialog - tells that settings window is accessing it
int renameSet;

SettingsWindow::SettingsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(statusDateTimeSettings()));
    timer->start(100); //timer to start the data and time on the status bar
    //Adding data and time widgets to the status bar
    ui->statusbar->addPermanentWidget(ui->label_Date);
    ui->statusbar->addPermanentWidget(ui->label_Time);


    //For Adding new password in Settings
    le = new customle(this);
    le->setStyleSheet("background-color: rgb(114, 159, 207); color: rgb(0, 0, 0); font-size: 22px");
    ui->horizontalLayout_Password->addWidget(le);

    //Path of the root directory
        QString SourcePath = "/media/hmi/";
//    QString SourcePath = "/media/csemi";

    dirmodel = new QFileSystemModel(this);
    //Displays only directories
    //    dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dirmodel->setRootPath(SourcePath);
    ui->treeView->setModel(dirmodel);
    ui->treeView->setRootIndex(dirmodel->setRootPath(SourcePath));
    ui->treeView->setColumnWidth(0,600);
    ui->treeView->setColumnWidth(1,140);
    ui->treeView->setColumnWidth(2,100);
    ui->treeView->setColumnWidth(3,300);

    //Disable the buttons except home and return button
    ui->pushButton_play->setEnabled(false);
    ui->pushButton_upload->setEnabled(false);
    ui->pushButton_rename->setEnabled(false);
    ui->pushButton_delete->setEnabled(false);
    ui->pushButton_copy_move->setEnabled(false);

    model2 = new QFileSystemModel(this);
    ui->treeView_2->setModel(model2);
    ui->treeView_2->setRootIndex(model2->setRootPath("/home/hmi"));
//    ui->treeView_2->setRootIndex(model2->setRootPath("/home/csemi/"));
    ui->treeView_2->setColumnWidth(0,600);
    ui->treeView_2->setColumnWidth(1,140);
    ui->treeView_2->setColumnWidth(2,100);
    ui->treeView_2->setColumnWidth(3,300);
}


SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::statusDateTimeSettings()
{
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString time_text = time.toString("hh : mm : ss");
    QString date_text = date.toString("dd-MM-yyyy");
    ui->label_Time->setText(time_text);
    ui->label_Date->setText(date_text);
}

//Function for opening the rename dialog
void SettingsWindow::openrenamedialog(){

    renamewindow *renamedialog = new renamewindow(this);
    renamedialog->setModal(true);
    renamedialog->setWindowFlag(Qt::FramelessWindowHint);

    renamedialog->exec();
    timeractive.elapsed();
}

void SettingsWindow::on_pushButton_home_button_clicked()
{
    emit homebuttonPressedSettings();
    //close this window
    close();

    //timer to keep the window active
    timeractive.start();
}

void SettingsWindow::on_pushButton_return_clicked()
{
    //timer to keep the window active
    timeractive.start();

    //Going to the main page on NVR window
    if(returncounter_settings == 2)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->label_heading->setText(" EXTERNAL STORAGE");
        returncounter_settings --;
    }

    else if (returncounter_settings == 1){
        ui->stackedWidget->setCurrentIndex(0);
        ui->label_heading->setText(" SETTINGS");
        returncounter_settings --;
    }
    //returns to menu page of the Mainwindow
    else if(returncounter_settings == 0){
        emit returnbuttonPressedSettings();
        close();
    }
}

void SettingsWindow::on_pushButton_passwordSettings_clicked()
{

    //Switch to the first page
    ui->stackedWidget->setCurrentIndex(1);

    ui->label_heading->setText(" PASSWORD SETTINGS");

    returncounter_settings = 1; //signifies we are going to page 2 of stackwidget
}


void SettingsWindow::on_pushButton_updatePass_clicked()
{
    if(!passdb.isOpen()){
        qDebug() << "Failed to Open Database";
        return;
    }

    QString Login_Pass;
    Login_Pass=le->text();

    //Change the password

    if(Login_Pass.size()==6)
    {

        QString Query = "UPDATE Password SET Pass='"+Login_Pass+"' WHERE ID=1";

        QSqlQuery qry;

        if(!qry.exec(Query)){
            qDebug() << "failed to update data in the table";
        }
        else{
            qDebug () << "Successfully updated data in table";
        }
        ui->label_message->setText("Password upgraded successfully");
        ui->label_message->setStyleSheet("QLabel {background-color: rgb(114, 159, 207); color : green; }");
        QTimer::singleShot(3000, this, [this] () { ui->label_message->setText(""); });
    }

    else
    {
        ui->label_message->setText("Please enter 6 digits as password");
        ui->label_message->setStyleSheet("QLabel {background-color: rgb(114, 159, 207); color : red; }");
        QTimer::singleShot(3000, this, [this] () { ui->label_message->setText(""); });
    }
}


void SettingsWindow::on_pushButton_externalStorage_clicked()
{
    //Switch to the first page
    ui->stackedWidget->setCurrentIndex(2);

    ui->label_heading->setText(" EXTERNAL STORAGE");

    returncounter_settings = 1; //signifies we are going to page 2 of stackwidget
}


void SettingsWindow::on_treeView_pressed(const QModelIndex &index)
{
    //Enable the buttons on press
    fileabspath = dirmodel->fileInfo(index).absoluteFilePath();
    qDebug("%s", qUtf8Printable(fileabspath));
    filepath = dirmodel->fileInfo(index).path();
    filename = dirmodel->fileInfo(index).fileName();

    ui->pushButton_play->setEnabled(true);
    ui->pushButton_upload->setEnabled(true);
    ui->pushButton_rename->setEnabled(true);
    ui->pushButton_delete->setEnabled(true);
    ui->pushButton_copy_move->setEnabled(true);

    QByteArray ba = fileabspath.toLocal8Bit();
    const char *file = ba.data();

    qDebug() << file;
}


void SettingsWindow::on_pushButton_play_clicked()
{
    settingvidplayer = 1;
    videoplayer *videoplayer = new class videoplayer(this);
    videoplayer->setModal(true);
    videoplayer->setWindowFlag(Qt::FramelessWindowHint);
    videoplayer->setGeometry(50,50,924,686);
    videoplayer->exec();

    //timer to keep the window active
    timeractive.start();
}


void SettingsWindow::on_pushButton_upload_clicked()
{
    //timer to keep the window active
    timeractive.start();
}


void SettingsWindow::on_pushButton_rename_clicked()
{
    //timer to keep the window active
    timeractive.start();

    openrenamedialog();

    QString newName_settings = filepath +"/"+  new_name;
    QString rename_file = " mv " + fileabspath + " " + newName_settings  ;

    qDebug() << rename_file;

    system(qPrintable(rename_file));

    renameSet = 1;
}


void SettingsWindow::on_pushButton_delete_clicked()
{
    //timer to keep the window active
    timeractive.start();

    QString delete_file = " rm " + fileabspath;

    system(qPrintable(delete_file));
}

void SettingsWindow::on_pushButton_copy_move_clicked()
{
    //timer to keep the window active
    timeractive.start();

    //Opens the Directory view page
    ui->stackedWidget->setCurrentIndex(3);
    returncounter_settings = 2;

    ui->label_heading->setText(" DIRECTORIES");
    ui->label_status->setText("Select the targer directory");
}

void SettingsWindow::on_treeView_2_pressed(const QModelIndex &index)
{
    //timer to keep the window active
    timeractive.start();

    usbpath_settings = model2->fileInfo(index).absoluteFilePath();
    usbfilename_settings = model2->fileInfo(index).fileName();

    ui->label_status->setText(usbpath_settings);
}


void SettingsWindow::on_pushButton_copy_clicked()
{
    //timer to keep the window active
    timeractive.start();

    QString copy_file_to_usb = " cp " + fileabspath + " " + usbpath_settings;

    qDebug() << copy_file_to_usb;

    system(qPrintable(copy_file_to_usb));

    ui->label_status->setText(filename + " copied to " + usbpath_settings );
    QTimer::singleShot(5000, this, [this] () { ui->label_status->setText(""); });
}


void SettingsWindow::on_pushButton_move_clicked()
{
    //timer to keep the window active
    timeractive.start();

    QString move_file_to_usb = " mv " + fileabspath + " " + usbpath_settings;

    qDebug() << move_file_to_usb;

    system(qPrintable(move_file_to_usb));

    ui->label_status->setText(filename + " moved to " + usbpath_settings );
    QTimer::singleShot(5000, this, [this] () { ui->label_status->setText(""); });
}


void SettingsWindow::on_pushButton_delete_2_clicked()
{
    //timer to keep the window active
    timeractive.start();

    QString delete_file_on_usb = " rm " + usbpath_settings;

    qDebug() << delete_file_on_usb;

    system(qPrintable(delete_file_on_usb));

    ui->label_status->setText( usbfilename_settings + " deleted ");
    QTimer::singleShot(5000, this, [this] () { ui->label_status->setText(""); });
}




