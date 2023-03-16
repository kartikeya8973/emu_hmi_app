#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <logindialog.h>

extern QSqlDatabase passdb;
extern QElapsedTimer timeractive;

//counter for return button for settings window
int returncounter_settings;


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
    if (returncounter_settings == 1){
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

