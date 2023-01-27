#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

extern QElapsedTimer timeractive;

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
}

