#include "devicestatuswindow.h"
#include "ui_devicestatuswindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

DeviceStatusWindow::DeviceStatusWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DeviceStatusWindow)
{
    ui->setupUi(this);
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(statusDateTimeDeviceStatus()));
    timer->start(100); //timer to start the data and time on the status bar
    //Adding data and time widgets to the status bar
    ui->statusbar->addPermanentWidget(ui->label_Date);
    ui->statusbar->addPermanentWidget(ui->label_Time);
}

DeviceStatusWindow::~DeviceStatusWindow()
{
    delete ui;
}

void DeviceStatusWindow::statusDateTimeDeviceStatus()
{
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString time_text = time.toString("hh : mm : ss");
    QString date_text = date.toString("dd-MM-yyyy");
    ui->label_Time->setText(time_text);
    ui->label_Date->setText(date_text);
}



void DeviceStatusWindow::on_pushButton_home_button_clicked()
{
    emit homebuttonPressedDevice();
    close();
}

