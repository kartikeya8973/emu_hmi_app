#include "infowindow.h"
#include "ui_infowindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

extern QElapsedTimer timeractive;

InfoWindow::InfoWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::InfoWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(statusDateTimeInfo()));
    timer->start(100); //timer to start the data and time on the status bar
    //Adding data and time widgets to the status bar
    ui->statusbar->addPermanentWidget(ui->label_Date);
    ui->statusbar->addPermanentWidget(ui->label_Time);

    ui->tabWidget->setCurrentIndex(0);
}

InfoWindow::~InfoWindow()
{
    delete ui;
}

void InfoWindow::statusDateTimeInfo()
{
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString time_text = time.toString("hh : mm : ss");
    QString date_text = date.toString("dd-MM-yyyy");
    ui->label_Time->setText(time_text);
    ui->label_Date->setText(date_text);
}

void InfoWindow::on_pushButton_home_button_clicked()
{
    emit homebuttonPressedInfo();
    //Closes the info window
    close();

    //timer to keep the window active
    timeractive.start();
}

//return to menu page
void InfoWindow::on_pushButton_return_clicked()
{
    emit returnbuttonPressedInfo();
    close();

    //timer to keep the window active
    timeractive.start();
}

