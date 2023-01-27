#include "devicewindow.h"
#include "ui_devicewindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

extern QElapsedTimer timeractive;

DeviceWindow::DeviceWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DeviceWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(statusDateTimeDevice()));
    timer->start(100); //timer to start the data and time on the status bar
    //Adding data and time widgets to the status bar
    ui->statusbar->addPermanentWidget(ui->label_Date);
    ui->statusbar->addPermanentWidget(ui->label_Time);
}

DeviceWindow::~DeviceWindow()
{
    delete ui;
}

void DeviceWindow::statusDateTimeDevice()
{
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString time_text = time.toString("hh : mm : ss");
    QString date_text = date.toString("dd-MM-yyyy");
    ui->label_Time->setText(time_text);
    ui->label_Date->setText(date_text);
}

void DeviceWindow::on_pushButton_home_button_clicked()
{
    emit homebuttonPressedDevice();
    //Close the window
    close();

    //timer to keep the window active
    timeractive.start();

}
void DeviceWindow::on_pushButton_return_clicked()
{
    //timer to keep the window active
    timeractive.start();
}

