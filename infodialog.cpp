#include "infodialog.h"
#include "ui_infodialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

infoDialog::infoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::infoDialog)
{
    ui->setupUi(this);
    QLayout *statusLayout = new QVBoxLayout();
    statusBar = new QStatusBar;
    statusLayout->addWidget(statusBar);

    label_Date = new QLabel;
    label_Time = new QLabel;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(infostatusDataTime()));
    timer->start(1000); //timer to start the data and time on the status bar
    statusBar->addWidget(label_Date);
    statusBar->addWidget(label_Time);
}

infoDialog::~infoDialog()
{
    delete ui;
}

void infoDialog::infostatusDataTime()
{
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString time_text = time.toString("hh : mm : ss");
    QString date_text = date.toString("dd-MM-yyyy");
    label_Time->setText(time_text);
    label_Date->setText(date_text);
}
