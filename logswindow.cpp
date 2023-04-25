#include "logswindow.h"
#include "ui_logswindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common.h"

#include <QDateTime>
#include <QCalendarWidget>
#include <QFile>
#include <QTextStream>

extern QElapsedTimer timeractive;

QString date_string_logs="";
QStringList stringListsyslogs;
QStringList stringListlogs;

//counter for return button for logs window
int returncounter_log;

//Global variable for filepath of USB
QString usbpath_logs = "";

//Global variable for filename selected from USB
QString usbfilename_logs = "";

//absolute path of NVR logs file
QString SourcePath_logs ="";

//absolute path of HMI logs file
QString SourcePath_systemLogs="";

LogsWindow::LogsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LogsWindow)
{
    ui->setupUi(this);
    returncounter_log = 0;
    timer = new QTimer(this);//opens log file in text browser
    //void LogsWindow::on_pushButton_openfile_clicked()
    //{
    //    QFile file(filepathlog);
    //    if(!file.open(QIODevice::ReadOnly)){
    //        qDebug() << "error in opening file";
    //    }

        //Converts file to a stream to show in a text browser
    //    QTextStream in(&file);

        //Show file in the text browser by passing the stream
    //    ui->textBrowser->setText(in.readAll());
    //}
    connect(timer, SIGNAL(timeout()),this,SLOT(statusDateTimelogs()));
    timer->start(100); //timer to start the data and time on the status bar
    //Adding data and time widgets to the status bar
    ui->statusbar->addPermanentWidget(ui->label_Date);
    ui->statusbar->addPermanentWidget(ui->label_Time);

    //Date selector
    ui->dateEdit->setDisplayFormat("MMM dd yyyy");
    ui->dateEdit->setDate(ui->calendar->selectedDate());
    connect(ui->dateEdit, &QDateEdit::dateChanged,ui->calendar, &QCalendarWidget::setSelectedDate);
    connect(ui->calendar, &QCalendarWidget::selectionChanged,this, &LogsWindow::selectedDateChanged);


    model2 = new QFileSystemModel(this);
    ui->treeView_2->setModel(model2);
    ui->treeView_2->setRootIndex(model2->setRootPath(pathToExternalStorage));
    ui->treeView_2->setColumnWidth(0,600);
    ui->treeView_2->setColumnWidth(1,140);
    ui->treeView_2->setColumnWidth(2,100);
    ui->treeView_2->setColumnWidth(3,300);

    //Disable the buttons except home and return button
//    ui->pushButton_openfile->setEnabled(false);

    //Saving selected date in a string
    date_string_logs = ui->dateEdit->date().toString("yyyy.MM.dd");
}

LogsWindow::~LogsWindow()
{
    delete ui;
}

void LogsWindow::statusDateTimelogs()
{
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString time_text = time.toString("hh : mm : ss");
    QString date_text = date.toString("dd-MM-yyyy");
    ui->label_Time->setText(time_text);
    ui->label_Date->setText(date_text);
}

void LogsWindow::on_pushButton_home_button_clicked()
{
    emit homebuttonPressedlogs();
    //close this window
    close();

    //timer to keep the window active
    timeractive.start();

    //Clearing the listviews after viewing logs
    stringListlogs.clear();
    stringListsyslogs.clear();

    //setting heading back to logs
    ui->label_heading->setText(" LOGS");

}

void LogsWindow::selectedDateChanged()
{
    ui->dateEdit->setDate(ui->calendar->selectedDate());

    //Saving selected date in a string
    date_string_logs = ui->dateEdit->date().toString("yyyy.MM.dd");
//    qDebug("%s", qUtf8Printable(date_string_logs));
}

//Button that opens page in which we can view the logs
void LogsWindow::on_pushButton_openLogs_clicked()
{
    ui->label_heading->setText(" LOGS");

    ui->stackedWidget->setCurrentIndex(1);
    //setting page 2 on pushbutton

    //logs file source path
    SourcePath_logs = pathToLogs+ date_string_logs + "/"+date_string_logs+"_logs";

    QFile filelogs(SourcePath_logs);
    if(!filelogs.open(QIODevice::ReadOnly))
        qDebug() << "error in opening file";

    //Converts file to a stream to show in a text browser
    QTextStream logs(&filelogs);
    while (true)
    {
        QString line = logs.readLine();
        if (line.isNull())
            break;
        else
            stringListlogs.append(line); // populate the stringlist
    }

    filemodellogs = new QStringListModel(this);

    // Populate the model
    filemodellogs->setStringList(stringListlogs);

    // Glue model and view together
    ui->listView->setModel(filemodellogs);

    //Changing colour of alternate rows
    ui->listView->setAlternatingRowColors(true);

    returncounter_log = 1; //signifies we are going to page 2 of stackwidget

    //timer to keep the window active
    timeractive.start();
}

void LogsWindow::on_pushButton_openfaults_clicked()
{
    ui->label_heading->setText(" SYSTEM LOGS");

    ui->stackedWidget->setCurrentIndex(2);
    //setting page 3 on pushbutton

    //system logs file source path
    SourcePath_systemLogs = pathToLogs + date_string_logs + "/"+date_string_logs+"_systemlogs";

    QFile fileSystemLogs(SourcePath_systemLogs);
    if(!fileSystemLogs.open(QIODevice::ReadOnly))
        qDebug() << "error in opening file";

    //Converts file to a stream to show in a text browser
    QTextStream syslogs(&fileSystemLogs);
    while (true)
    {
        QString line = syslogs.readLine();
        if (line.isNull())
            break;
        else
            stringListsyslogs.append(line); // populate the stringlist
    }

    filemodelsyslogs = new QStringListModel(this);

    // Populate the model
    filemodelsyslogs->setStringList(stringListsyslogs);

    // Glue model and view together
    ui->listView_2->setModel(filemodelsyslogs);

    //Changing colour of alternate rows
    ui->listView_2->setAlternatingRowColors(true);

    returncounter_log = 1; //signifies we are going to page 2 of stackwidget

    //timer to keep the window active
    timeractive.start();
}

void LogsWindow::on_pushButton_return_clicked()
{
    //returns to the first page (index 0) of the logswindow
    if(returncounter_log == 2)
    {
        ui->stackedWidget->setCurrentIndex(1);
        ui->label_heading->setText(" LOGS");
        returncounter_log --;
    }

    if (returncounter_log == 1){
        ui->stackedWidget->setCurrentIndex(0);
        returncounter_log --;
    }
    //returns to menu page of the Mainwindow
     else if(returncounter_log == 0){
        emit returnbuttonPressedlogs();
        close();
    }

    //timer to keep the window active
    timeractive.start();

    //Clearing the listviews after viewing logs
    stringListlogs.clear();
    stringListsyslogs.clear();

    //setting heading back to logs
    ui->label_heading->setText(" LOGS");

}

void LogsWindow::on_calendar_clicked(const QDate &date)
{
    //timer to keep the window active
    timeractive.start();
}

//For copying NVR logs
void LogsWindow::on_pushButton_copylogs2USB_clicked()
{
    //timer to keep the window active
    timeractive.start();

    //Opens the USB copy and move page
    ui->stackedWidget->setCurrentIndex(3);
    returncounter_log = 2;

    ui->label_heading->setText(" USB / External Storage List");
    ui->label_status->setText("Select the USB or Harddisk");

    ui->pushButton_copy->setVisible(true);
    ui->pushButton_copy_hmi_logs->setVisible(false);
}

//For copying HMI logs
void LogsWindow::on_pushButton_copyfault2USB_clicked()
{
    //timer to keep the window active
    timeractive.start();

    //Opens the USB copy and move page
    ui->stackedWidget->setCurrentIndex(3);
    returncounter_log = 2;

    ui->label_heading->setText(" USB / External Storage List");
    ui->label_status->setText("Select the USB or Harddisk");

    ui->pushButton_copy->setVisible(false);
    ui->pushButton_copy_hmi_logs->setVisible(true);
}

void LogsWindow::on_treeView_2_pressed(const QModelIndex &index)
{
    usbpath_logs = model2->fileInfo(index).absoluteFilePath();
    usbfilename_logs = model2->fileInfo(index).fileName();

    ui->label_status->setText(usbpath_logs);
}

void LogsWindow::on_pushButton_copy_clicked()
{
    //timer to keep the window active
    timeractive.start();

    QString copy_file_to_usb = " cp " + SourcePath_logs + " " + usbpath_logs;

    qDebug() << copy_file_to_usb;

    system(qPrintable(copy_file_to_usb));

    ui->label_status->setText("NVR LOGS for "+date_string_logs+" copied to " + usbpath_logs );
    QTimer::singleShot(5000, this, [this] () { ui->label_status->setText(""); });
}

void LogsWindow::on_pushButton_copy_hmi_logs_clicked()
{
    //timer to keep the window active
    timeractive.start();

    QString copy_file_to_usb = " cp " + SourcePath_systemLogs + " " + usbpath_logs;

    qDebug() << copy_file_to_usb;

    system(qPrintable(copy_file_to_usb));

    ui->label_status->setText("HMI LOGS for "+date_string_logs+" copied to " + usbpath_logs );
    QTimer::singleShot(5000, this, [this] () { ui->label_status->setText(""); });
}

