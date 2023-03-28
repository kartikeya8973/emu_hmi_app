#include "logindialog.h"
#include "ui_logindialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

extern QElapsedTimer timeractive;

//flag for menu button
int success;
//flag for reopening login window if the window is closed by user using close button
int loginfail;

//Sqlite server for storing password
extern QSqlDatabase passdb;

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    success = 0;
    loginfail = 0;

//    //Creating Database
//    passdb = QSqlDatabase::addDatabase("QSQLITE");

    //For HMI
    passdb.setDatabaseName("/home/hmi/HMI/password.db");
    //For Host PC
//        passdb.setDatabaseName("/home/csemi/qtworkspace_new/HMItemplate/password.db");

    if(!passdb.open()){
        qDebug() << "Failed to open Database";
    }
    else{
        qDebug() << "Connected to Database";
        QString query = "CREATE TABLE Password2 ("
                        "ID INTEGER PRIMARY KEY,"
                        "Pass INTERGER );";

        QSqlQuery qry;
        if(!qry.exec(query)){
            qDebug() << "Failed to Create Password Table/ Table already exists";
        }
        else{
            qDebug() << "Table Created";
            QString pass_default = "INSERT INTO Password2 (Pass) VALUES (123456);";
            QSqlQuery pass_qry;
            if(!pass_qry.exec(pass_default)){
                qDebug() << "Failed to Add Default Password in the Table/ Password already exists";
            }
            else{
                qDebug() << "Password Added";
            }
        }
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_pushButton_close_clicked()
{
    close(); //closes the login dialog window
    //opens login window again
    loginfail = 1;
}


void LoginDialog::on_pushButton_1_clicked()
{
    if(accessKey.length() < 6){
        accessKey = accessKey + "1";
        ui->label_password->setText(accessKey);
        ui->label_password->setStyleSheet("QLabel {"
                                          "border-style: solid;"
                                          "border-width: 2px;"
                                          "border-color: black; "
                                          "}");
    }
    else{
        accessKey = "";
        ui->label_password->setText("");
    }
}


void LoginDialog::on_pushButton_2_clicked()
{
    if(accessKey.length() < 6){
        accessKey = accessKey + "2";
        ui->label_password->setText(accessKey);
        ui->label_password->setStyleSheet("QLabel {"
                                          "border-style: solid;"
                                          "border-width: 2px;"
                                          "border-color: black; "
                                          "}");
    }
    else{
        accessKey = "";
        ui->label_password->setText("");
    }
}


void LoginDialog::on_pushButton_3_clicked()
{
    if(accessKey.length() < 6){
        accessKey = accessKey + "3";
        ui->label_password->setText(accessKey);
        ui->label_password->setStyleSheet("QLabel {"
                                          "border-style: solid;"
                                          "border-width: 2px;"
                                          "border-color: black; "
                                          "}");
    }
    else{
        accessKey = "";
        ui->label_password->setText("");
    }
}


void LoginDialog::on_pushButton_4_clicked()
{
    if(accessKey.length() < 6){
        accessKey = accessKey + "4";
        ui->label_password->setText(accessKey);
        ui->label_password->setStyleSheet("QLabel {"
                                          "border-style: solid;"
                                          "border-width: 2px;"
                                          "border-color: black; "
                                          "}");
    }
    else{
        accessKey = "";
        ui->label_password->setText("");
    }
}


void LoginDialog::on_pushButton_5_clicked()
{
    if(accessKey.length() < 6){
        accessKey = accessKey + "5";
        ui->label_password->setText(accessKey);
        ui->label_password->setStyleSheet("QLabel {"
                                          "border-style: solid;"
                                          "border-width: 2px;"
                                          "border-color: black; "
                                          "}");
    }
    else{
        accessKey = "";
        ui->label_password->setText("");
    }
}


void LoginDialog::on_pushButton_6_clicked()
{
    if(accessKey.length() < 6){
        accessKey = accessKey + "6";
        ui->label_password->setText(accessKey);
        ui->label_password->setStyleSheet("QLabel {"
                                          "border-style: solid;"
                                          "border-width: 2px;"
                                          "border-color: black; "
                                          "}");
    }
    else{
        accessKey = "";
        ui->label_password->setText("");
    }
}


void LoginDialog::on_pushButton_7_clicked()
{
    if(accessKey.length() < 6){
        accessKey = accessKey + "7";
        ui->label_password->setText(accessKey);
        ui->label_password->setStyleSheet("QLabel {"
                                          "border-style: solid;"
                                          "border-width: 2px;"
                                          "border-color: black; "
                                          "}");
    }
    else{
        accessKey = "";
        ui->label_password->setText("");
    }
}


void LoginDialog::on_pushButton_8_clicked()
{
    if(accessKey.length() < 6){
        accessKey = accessKey + "8";
        ui->label_password->setText(accessKey);
        ui->label_password->setStyleSheet("QLabel {"
                                          "border-style: solid;"
                                          "border-width: 2px;"
                                          "border-color: black; "
                                          "}");
    }
    else{
        accessKey = "";
        ui->label_password->setText("");
    }
}


void LoginDialog::on_pushButton_9_clicked()
{
    if(accessKey.length() < 6){
        accessKey = accessKey + "9";
        ui->label_password->setText(accessKey);
        ui->label_password->setStyleSheet("QLabel {"
                                          "border-style: solid;"
                                          "border-width: 2px;"
                                          "border-color: black; "
                                          "}");
    }
    else{
        accessKey = "";
        ui->label_password->setText("");
    }
}

void LoginDialog::on_pushButton_0_clicked()
{
    if(accessKey.length() < 6){
        accessKey = accessKey + "0";
        ui->label_password->setText(accessKey);
        ui->label_password->setStyleSheet("QLabel {"
                                          "border-style: solid;"
                                          "border-width: 2px;"
                                          "border-color: black; "
                                          "}");
    }
    else{
        accessKey = "";
        ui->label_password->setText("");
    }
}

void LoginDialog::on_pushButton_clear_clicked()
{
    accessKey = "";
    ui->label_password->setText("");
    ui->label_password->setStyleSheet("QLabel {"
                                      "border-style: solid;"
                                      "border-width: 2px;"
                                      "border-color: black; "
                                      "}");
}


void LoginDialog::on_pushButton_ok_clicked()
{
    //        if (accessKey == "123456"){
    //            emit okbuttonPressed();
    //            close();

    //            //timer for keeping window active
    //            timeractive.start();
    //            //flag for using menu button for opening menu page after succesful login
    //            success = 1;
    //        }
    QString password;
    password = ui->label_password->text();

    if(!passdb.isOpen()){
        qDebug() << "Failed to Open Database";
        return;
    }

    QSqlQuery qry;
    int count = 0;
    if(qry.exec(("SELECT * FROM Password2 WHERE Pass='"+password+"'"))){
        while(qry.next())
        {
            count++;
        }
    }

    if (count == 1 && accessKey == password && accessKey.size()==6){
        emit okbuttonPressed();
        close();

        //timer for keeping window active
        timeractive.start();
        //flag for using menu button for opening menu page after succesful login
        success = 1;
    }

    else{
        accessKey = "";
        ui->label_password->setText("ERROR");
        ui->label_password->setStyleSheet("QLabel {"
                                          "background-color : red;"
                                          "border-style: solid;"
                                          "border-width: 2px;"
                                          "border-color: black; "
                                          "}");
        QTimer::singleShot(2000, this, [this] () { ui->label_password->setText("");
            ui->label_password->setStyleSheet("QLabel {"
                                              "border-style: solid;"
                                              "border-width: 2px;"
                                              "border-color: black; "
                                              "}"); });
    }
}

