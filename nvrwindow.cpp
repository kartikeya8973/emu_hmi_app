#include "nvrwindow.h"
#include "ui_nvrwindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include "httpdownloader.h"
#include <QString>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "qjsontablemodel.h"
#include "common.h"

extern QElapsedTimer timeractive;

//counter for return button for NVR window
int returncounter_nvr=0;

extern int ping_for_nvr1;
extern int ping_for_nvr2;
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

QString dateListStart;
QString timeListStart;
QString dateListEnd;
QString timeListEnd;

NVRWindow::NVRWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NVRWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(statusDateTimeNVR()));
    timer->start(100); //timer to start the data and time on the status bar
    //Adding data and time widgets to the status bar
    ui->statusbar->addPermanentWidget(ui->label_Date);
    ui->statusbar->addPermanentWidget(ui->label_Time);

    //    int ping_for_button= system("ping -c 3 192.168.1.2 > /dev/null 2>&1");
    //    if ( ping_for_button == 0) //Pinging NVR to check if it active
    //    {
    //        ui->label_nvrStatus->setText("NVR ONLINE");
    //        ui->label_nvrStatus->setStyleSheet("QLabel { color : green; }");
    //    }

    //    else
    //    {
    //        ui->label_nvrStatus->setText("NVR OFFLINE");
    //        ui->label_nvrStatus->setStyleSheet("QLabel { color : red; }");
    //    }
}

NVRWindow::~NVRWindow()
{
    delete ui;
}

void NVRWindow::statusDateTimeNVR()
{
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString time_text = time.toString("hh : mm : ss");
    QString date_text = date.toString("dd-MM-yyyy");
    ui->label_Time->setText(time_text);
    ui->label_Date->setText(date_text);
}

//Function for opening the VidArchives window
void NVRWindow::openvidArvives(){

    VideoArchiveWindow *videoarcwindow = new VideoArchiveWindow(this);
    videoarcwindow->setWindowFlag(Qt::FramelessWindowHint);
    //This connect logic connects the home button press with the first page ( index 0) of stackWidget_Dynamic
    QObject::connect(videoarcwindow, SIGNAL(homebuttonPressedVid()), this, SLOT(openHomePage()));
    //This connect logic connects the return button press with the second page ( index 1) of stackWidget_Dynamic
    QObject::connect(videoarcwindow, SIGNAL(returnbuttonPressedVid()), this, SLOT(openMenuPagereturn()));
    videoarcwindow->showFullScreen();
}

void NVRWindow::on_pushButton_home_button_clicked()
{
    emit homebuttonPressedNVR();
    //close this window
    close();

    //timer to keep the window active
    timeractive.start();
}

void NVRWindow::on_pushButton_return_clicked()
{
    //timer to keep the window active
    timeractive.start();

    //Going to the main page on NVR window
    if(returncounter_nvr == 2)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->label_heading->setText(" VIDEO LIST");
        returncounter_nvr --;
    }

    else if (returncounter_nvr == 1){
        ui->stackedWidget->setCurrentIndex(0);
        ui->label_heading->setText(" NVR INTERFACE");
        returncounter_nvr --;
    }
    //returns to menu page of the Mainwindow
    else if(returncounter_nvr == 0){
        emit returnbuttonPressedNVR();
        close();
    }
}

void NVRWindow::on_pushButton_diagnostics_clicked()
{
    //Switch to the second page
    ui->stackedWidget->setCurrentIndex(1);

    ui->label_heading->setText(" SELF DIAGNOSTICS");

    returncounter_nvr = 1; //signifies we are going to page 2 of stackwidget

    nvrStatus();
}

void NVRWindow::on_pushButton_streamList_clicked()
{
    //Switch to the third page
    ui->stackedWidget->setCurrentIndex(2);

    ui->label_heading->setText(" VIDEO LIST");

    returncounter_nvr = 1; //signifies we are going to page 3 of stackwidget

}

void NVRWindow::on_pushButton_videoList_clicked()
{
    //Switch to the third page


    QDate selectedDateStart = ui->dateEditStart->date();
    dateListStart = selectedDateStart.toString("yyyy-MM-dd");

    QDate selectedDateEnd = ui->dateEditEnd->date();
    dateListEnd = selectedDateEnd.toString("yyyy-MM-dd");

    QTime selectedTimeStart = ui->timeEditStart->time();
    timeListStart = selectedTimeStart.toString("hh:mm:ss");

    QTime selectedTimeEnd = ui->timeEditEnd->time();
    timeListEnd = selectedTimeEnd.toString("hh:mm:ss");


    //    ui->label_3->setText(dateListStart+" " +dateListEnd);
    //    ui->label_4->setText(timeListStart+" " +timeListEnd);

    ui->stackedWidget->setCurrentIndex(3);

    ui->label_heading->setText(" VIDEO LIST");

    returncounter_nvr = 2; //signifies we are going to page 3 of stackwidget
}



//Http download for record list
//##########################################################################################

void NVRWindow::downloadJson()
{
    managerList = new QNetworkAccessManager(this);

    connect(managerList,
            &QNetworkAccessManager::finished,
            this,
            &NVRWindow::replyList);

    QString downloadUrlList = "http://admin:admin@192.168.1.2/recordlist.cgi?starttime="+dateListStart+"T"+timeListStart+"Z&endtime="+dateListEnd+"T"+timeListEnd+"&maxcount=999";

    //        managerList->get(QNetworkRequest(QUrl("http://admin:admin@192.168.1.3/recordlist.cgi?starttime=2010-01-01T00:00:00Z&endtime=2010-12-31T23:59:59Z&maxcount=999")));
    managerList->get(QNetworkRequest(QUrl(downloadUrlList)));

}

void NVRWindow::replyList (QNetworkReply *replyList)
{
    if(replyList->error())
    {
        qDebug() << "ERROR!";
        qDebug() << replyList->errorString();

        ui->label_status->setText("Error in downloading Video List !!! Can not establish connection to NVR");
        ui->label_status->setStyleSheet("QLabel {background-color: rgb(114, 159, 207); color : red; }");
    }
    else
    {
        qDebug() << replyList->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug() << replyList->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();
        qDebug() << replyList->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        qDebug() << replyList->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << replyList->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

        QFile *file = new QFile(streamListJsonPath);
        if(file->open(QFile::Append))
        {
            file->write(replyList->readAll());
            file->flush();
            file->close();
        }
        delete file;

        ui->label_status->setText("Video List downloaded successfully, click on VIEW VIDEO LIST button to get list view");
        ui->label_status->setStyleSheet("QLabel {background-color: rgb(114, 159, 207); color : green; }");
    }

    replyList->deleteLater();
}

void NVRWindow::on_pushButton_downloadReloadList_clicked()
{
    //Deleting previous json
    //For HMI
    QString delPreviousJson = "rm " + streamListJsonPath;
    system(qPrintable(delPreviousJson));

    // Calling fucntion to download record list from NVR
    downloadJson();
}
//##########################################################################################

//Http download for Video Stream
//##########################################################################################

void NVRWindow::downloadStream()
{
    managerStream = new QNetworkAccessManager(this);

    connect(managerStream,
            &QNetworkAccessManager::finished,
            this,
            &NVRWindow::replyStream);

    QString downloadUrlStream = "";

    managerStream->get(QNetworkRequest(QUrl("http://admin:admin@192.168.1.2/playback.mp4?id=147&starttime=2010-02-13T23:09:06Z&endtime=2010-02-13T23:10:22Z")));

    //For downloading from a specific channel (need to make it dyanmcic)
    managerStream->get(QNetworkRequest(QUrl("http://admin:admin@192.168.1.2/playback.mp4?channel=2&speed=1&id=268&stime=2010-02-01+05:30:21")));
}

void NVRWindow::replyStream (QNetworkReply *replyStream)
{
    if(replyStream->error())
    {
        qDebug() << "ERROR!";
        qDebug() << replyStream->errorString();

        ui->label_status->setText("Error in downloading Video stream !!! Can not establish connection to NVR");
        ui->label_status->setStyleSheet("QLabel {background-color: rgb(114, 159, 207); color : red; }");
    }
    else
    {
        qDebug() << replyStream->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug() << replyStream->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();
        qDebug() << replyStream->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        qDebug() << replyStream->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << replyStream->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();


        QDate date = QDate::currentDate();
        QTime time = QTime::currentTime();
        QString date_string ="";
        QString time_string ="";
        date_string = date.toString("yyyy.MM.dd");
        time_string = time.toString("hh.mm.ss");

        QString filename = date_string + time_string;

        QFile *file = new QFile(pathToVidArchives+date_string+"_recordings/"+filename+"_NVR.mp4");
        //        QFile *file = new QFile("/home/csemi/VidArchives/"+ date_string +"_recordings/video.mp4");
        if(file->open(QFile::Append))
        {
            file->write(replyStream->readAll());
            file->flush();
            file->close();
        }
        delete file;
        ui->label_status->setText("Video stream downloaded successfully");
        ui->label_status->setStyleSheet("QLabel {background-color: rgb(114, 159, 207); color : green; }");
    }


    replyStream->deleteLater();
}

void NVRWindow::on_pushButton_downloadVideo_clicked()
{
    // Calling fucntion to download video stream from NVR
    downloadStream();
}
//##########################################################################################

//Read Json of record list and display in TableView
//##########################################################################################

void NVRWindow::readJson()
{

    QString jsonFile;
    QFile file;

    //for hmi
    file.setFileName(streamListJsonPath);
    //for host PC
    //        file.setFileName("/home/csemi/Downloads/streamList.json");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray saveData = file.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
        jsonFile=loadDoc.toJson();

        QJsonObject json = loadDoc.object();

        file.close();

        QByteArray streamJson;

        for (int i = 14; i <= saveData.count() - 2 ; i++)
        {
            streamJson.append(saveData.at(i));
        }

        qDebug() << saveData.count();
        //    qDebug() << saveData.at(14);
        //    qDebug() << saveData.at(9872);
        qDebug() << streamJson;
        //    qDebug() << jsonFile;

        QJsonTableModel::Header header;
        header.push_back( QJsonTableModel::Heading( { {"title","ID"},    {"index","id"} } ) );
        header.push_back( QJsonTableModel::Heading( { {"title","Start"},   {"index","start"} }) );
        header.push_back( QJsonTableModel::Heading( { {"title","End"},  {"index","end"} }) );
        header.push_back( QJsonTableModel::Heading( { {"title","Type"}, {"index","type"} }) );

        streamList = new QJsonTableModel( header, this );
        ui->tableView_streamList->setModel(streamList);

        QJsonDocument jsonDocument = QJsonDocument::fromJson(streamJson);
        streamList->setJson( jsonDocument );

        ui->label_status->setText("Video List successfully loaded");
        ui->label_status->setStyleSheet("QLabel {background-color: rgb(114, 159, 207); color : green; }");

        ui->tableView_streamList->setColumnWidth(0,20);
        ui->tableView_streamList->setColumnWidth(1,350);
        ui->tableView_streamList->setColumnWidth(2,350);
        ui->tableView_streamList->setColumnWidth(3,20);

    }

    else
    {
        ui->label_status->setText("Video list file not found");
        ui->label_status->setStyleSheet("QLabel {background-color: rgb(114, 159, 207); color : red; }");
    }


}

void NVRWindow::on_pushButton_viewList_clicked()
{
    // Calling fucntion to read record list and display in tableview
    readJson();
}

//##########################################################################################

//##########################################################################################

//Function for getting status of NVR
void NVRWindow::nvrStatus()
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


void NVRWindow::on_pushButton_openDownloadedList_clicked()
{
    openvidArvives();
}


void NVRWindow::on_tableView_streamList_pressed(const QModelIndex &index)
{
    QString test = "";

    test = index.data().toString();

    QVariant data = ui->tableView_streamList->model()->data(index);

    // retrieve the row and column indices
    int rowNo = index.row();

    QString rowData = "";
    for (int col = 0; col < ui->tableView_streamList->model()->columnCount(); col++)
    {
        QModelIndex cellIndex = ui->tableView_streamList->model()->index(rowNo, col);
        QString cellData = ui->tableView_streamList->model()->data(cellIndex).toString();
        rowData += cellData + ", ";
    }

    ui->label_status->setText(rowData);
    qDebug() << test ;
}

