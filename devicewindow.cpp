#include "devicewindow.h"
#include "ui_devicewindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

extern QElapsedTimer timeractive;


//counter for return button for NVR window
int returncounter_deviceStatus;

extern int ping_for_cam1;
extern int ping_for_cam2;
extern int ping_for_cam3;
extern int ping_for_cam4;
extern int ping_for_cam5;
extern int ping_for_cam6;
extern int ping_for_cam7;
extern int ping_for_cam8;
extern int ping_for_cam9;
extern int ping_for_cam10;
extern int ping_for_cam11;
extern int ping_for_cam12;

extern int ping_for_nvr1;
extern int ping_for_nvr2;

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

    //Going to the main page on NVR window
    if (returncounter_deviceStatus == 1){
        ui->stackedWidget->setCurrentIndex(0);
        ui->label_heading->setText(" DEVICE STATUS");
        returncounter_deviceStatus --;
    }
    //returns to menu page of the Mainwindow
    else if(returncounter_deviceStatus == 0){
        emit returnbuttonPressedDevice();
        close();
    }
}

void DeviceWindow::on_pushButton_nvrStatus_clicked()
{
    //Switch to the second page (Camera Status)
    ui->stackedWidget->setCurrentIndex(1);

    ui->label_heading->setText(" NVR STATUS");

    returncounter_deviceStatus = 1; //signifies we are going to page 2 of stackwidget

    nvrStatus();
}

void DeviceWindow::on_pushButton_camStatus_clicked()
{
    //Switch to the second page (Camera Status)
    ui->stackedWidget->setCurrentIndex(2);

    ui->label_heading->setText(" CAMERA STATUS");

    returncounter_deviceStatus = 1; //signifies we are going to page 3 of stackwidget

    camStatus();
}

void DeviceWindow::nvrStatus()
{
    //NVR1
    if ( ping_for_nvr1 == 0) //Pinging NVR to check if it active
    {
        ui->label_StatusNvr1->setText("IP : 192.168.1.2");
        ui->label_StatusNvr1->setStyleSheet("QLabel { color : green; }");
        ui->label_StatusIconNvr1->setStyleSheet("image: url(:/new/icons/nvr_green.png);");
    }
    else
    {
        ui->label_StatusNvr1->setText("NVR 1 is OFFLINE");
        ui->label_StatusNvr1->setStyleSheet("QLabel { color : red; }");
        ui->label_StatusIconNvr1->setStyleSheet("image: url(:/new/icons/nvr_red.png);");
    }

    //NVR2
    if ( ping_for_nvr2 == 0) //Pinging NVR to check if it active
    {
        ui->label_StatusNvr2->setText("IP : 192.168.1.232");
        ui->label_StatusNvr2->setStyleSheet("QLabel { color : green; }");
        ui->label_StatusIconNvr2->setStyleSheet("image: url(:/new/icons/nvr_green.png);");
    }
    else
    {
        ui->label_StatusNvr2->setText("NVR 2 is OFFLINE");
        ui->label_StatusNvr2->setStyleSheet("QLabel { color : red; }");
        ui->label_StatusIconNvr2->setStyleSheet("image: url(:/new/icons/nvr_red.png);");
    }
}


void DeviceWindow::camStatus()
{
    //CAM1
//    int ping_for_cam1 = system("ping -c 1 192.168.1.221 > /dev/null 2>&1");
    if ( ping_for_cam1 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam1->setText("IP : 192.168.1.221");
        ui->label_StatusCam1->setStyleSheet("QLabel { color : green; }");
        ui->label_StatusIconCam1->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam1->setText("CAM 1 is OFFLINE");
        ui->label_StatusCam1->setStyleSheet("QLabel { color : red; }");
        ui->label_StatusIconCam1->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

    //CAM2
//    int ping_for_cam2 = system("ping -c 1 192.168.1.222 > /dev/null 2>&1");
    if ( ping_for_cam2 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam2->setText("IP : 192.168.1.222");
        ui->label_StatusCam2->setStyleSheet("QLabel { color : green; }");
        ui->label_StatusIconCam2->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam2->setText("CAM 2 is OFFLINE");
        ui->label_StatusCam2->setStyleSheet("QLabel { color : red; }");
        ui->label_StatusIconCam2->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

    //CAM3
//    int ping_for_cam3 = system("ping -c 1 192.168.1.223 > /dev/null 2>&1");
    if ( ping_for_cam3 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam3->setText("IP : 192.168.1.223");
        ui->label_StatusCam3->setStyleSheet("QLabel { color : green; }");
        ui->label_StatusIconCam3->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam3->setText("CAM 3 is OFFLINE");
        ui->label_StatusCam3->setStyleSheet("QLabel { color : red; }");
        ui->label_StatusIconCam3->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

//    int ping_for_cam4 = system("ping -c 1 192.168.1.224 > /dev/null 2>&1");
    if ( ping_for_cam4 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam4->setText("IP : 192.168.1.224");
        ui->label_StatusCam4->setStyleSheet("QLabel { color : green; }");
        ui->label_StatusIconCam4->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam4->setText("CAM 4 is OFFLINE");
        ui->label_StatusCam4->setStyleSheet("QLabel { color : red; }");
        ui->label_StatusIconCam4->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

//    int ping_for_cam5 = system("ping -c 1 192.168.1.225 > /dev/null 2>&1");
    if ( ping_for_cam5 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam5->setText("IP : 192.168.1.225");
        ui->label_StatusCam5->setStyleSheet("QLabel { color : green; }");
        ui->label_StatusIconCam5->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam5->setText("CAM 5 is OFFLINE");
        ui->label_StatusCam5->setStyleSheet("QLabel { color : red; }");
        ui->label_StatusIconCam5->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

//    int ping_for_cam6 = system("ping -c 1 192.168.1.226 > /dev/null 2>&1");
    if ( ping_for_cam6 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam6->setText("IP : 192.168.1.226");
        ui->label_StatusCam6->setStyleSheet("QLabel { color : green; }");
        ui->label_StatusIconCam6->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam6->setText("CAM 6 is OFFLINE");
        ui->label_StatusCam6->setStyleSheet("QLabel { color : red; }");
        ui->label_StatusIconCam6->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

//    int ping_for_cam7 = system("ping -c 1 192.168.1.227 > /dev/null 2>&1");
    if ( ping_for_cam7 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam7->setText("IP : 192.168.1.227");
        ui->label_StatusCam7->setStyleSheet("QLabel { color : green; }");
        ui->label_StatusIconCam7->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam7->setText("CAM 7 is OFFLINE");
        ui->label_StatusCam7->setStyleSheet("QLabel { color : red; }");
        ui->label_StatusIconCam7->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

//    int ping_for_cam8 = system("ping -c 1 192.168.1.228 > /dev/null 2>&1");
    if ( ping_for_cam8 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam8->setText("IP : 192.168.1.228");
        ui->label_StatusCam8->setStyleSheet("QLabel { color : green; }");
        ui->label_StatusIconCam8->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam8->setText("CAM 8 is OFFLINE");
        ui->label_StatusCam8->setStyleSheet("QLabel { color : red; }");
        ui->label_StatusIconCam8->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

//    int ping_for_cam9 = system("ping -c 1 192.168.1.229 > /dev/null 2>&1");
    if ( ping_for_cam9 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam9->setText("IP : 192.168.1.229");
        ui->label_StatusCam9->setStyleSheet("QLabel { color : green; }");
        ui->label_StatusIconCam9->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam9->setText("CAM 9 is OFFLINE");
        ui->label_StatusCam9->setStyleSheet("QLabel { color : red; }");
        ui->label_StatusIconCam9->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

//    int ping_for_cam10 = system("ping -c 1 192.168.1.230 > /dev/null 2>&1");
    if ( ping_for_cam10 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam10->setText("IP : 192.168.1.230");
        ui->label_StatusCam10->setStyleSheet("QLabel { color : green; }");
        ui->label_StatusIconCam10->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam10->setText("CAM 10 is OFFLINE");
        ui->label_StatusCam10->setStyleSheet("QLabel { color : red; }");
        ui->label_StatusIconCam10->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

//    int ping_for_cam11 = system("ping -c 1 192.168.1.231 > /dev/null 2>&1");
    if ( ping_for_cam11 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam11->setText("IP : 192.168.1.231");
        ui->label_StatusCam11->setStyleSheet("QLabel { color : green; }");
        ui->label_StatusIconCam11->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam11->setText("CAM 11 is OFFLINE");
        ui->label_StatusCam11->setStyleSheet("QLabel { color : red; }");
        ui->label_StatusIconCam11->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }

//    int ping_for_cam12 = system("ping -c 1 192.168.1.232 > /dev/null 2>&1");
    if ( ping_for_cam12 == 0) //Pinging IPCAM to check if it active
    {
        ui->label_StatusCam12->setText("IP : 192.168.1.232");
        ui->label_StatusCam12->setStyleSheet("QLabel { color : green; }");
        ui->label_StatusIconCam12->setStyleSheet("image: url(:/new/icons/cctv_green.png);");
    }
    else
    {
        ui->label_StatusCam12->setText("CAM 12 is OFFLINE");
        ui->label_StatusCam12->setStyleSheet("QLabel { color : red; }");
        ui->label_StatusIconCam12->setStyleSheet("image: url(:/new/icons/cctv_red.png);");
    }
}




