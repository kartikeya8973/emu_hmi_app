#include "logswindow.h"
#include "ui_logswindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QCalendarWidget>
#include <QFile>
#include <QTextStream>

extern QElapsedTimer timeractive;



QString date_string="";
//QString filepathlog="";
QStringList stringListfaults;
QStringList stringListlogs;

//counter for return button for logs window
int returncounter_log;

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


    //Disable the buttons except home and return button
//    ui->pushButton_openfile->setEnabled(false);

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

    //Clearing the listviews after viewing logs / faults
    stringListlogs.clear();
    stringListfaults.clear();

    //setting heading back to logs/faults
    ui->label_heading->setText(" LOGS/FAULTS");

}

void LogsWindow::selectedDateChanged()
{
    ui->dateEdit->setDate(ui->calendar->selectedDate());

    //Saving selected date in a string
    date_string = ui->dateEdit->date().toString("dd.MM.yyyy");
    qDebug("%s", qUtf8Printable(date_string));
}

//Button that opens page in which we can view the logs
void LogsWindow::on_pushButton_openLogs_clicked()
{
    ui->label_heading->setText(" LOGS");

    ui->stackedWidget->setCurrentIndex(1);
    //setting page 2 on pushbutton

    //log file source path
    QString SourcePath_logs = "/home/hmi/logs/" + date_string + "/log";
//    QString SourcePath_logs = "/home/csemi/logs/" + date_string + "/log";

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
    ui->label_heading->setText(" FAULTS");

    ui->stackedWidget->setCurrentIndex(2);
    //setting page 3 on pushbutton

    //log file source path
    QString SourcePath_faults = "/home/hmi/logs/" + date_string + "/faults";
//    QString SourcePath_faults = "/home/csemi/logs/" + date_string + "/faults";

    QFile filefaults(SourcePath_faults);
    if(!filefaults.open(QIODevice::ReadOnly))
        qDebug() << "error in opening file";

    //Converts file to a stream to show in a text browser
    QTextStream faults(&filefaults);
    while (true)
    {
        QString line = faults.readLine();
        if (line.isNull())
            break;
        else
            stringListfaults.append(line); // populate the stringlist
    }

    filemodelfaults = new QStringListModel(this);

    // Populate the model
    filemodelfaults->setStringList(stringListfaults);

    // Glue model and view together
    ui->listView_2->setModel(filemodelfaults);

    //Changing colour of alternate rows
    ui->listView_2->setAlternatingRowColors(true);

    returncounter_log = 1; //signifies we are going to page 2 of stackwidget

    //timer to keep the window active
    timeractive.start();
}

void LogsWindow::on_pushButton_return_clicked()
{
    //returns to the first page (index 0) of the logswindow
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

    //Clearing the listviews after viewing logs / faults
    stringListlogs.clear();
    stringListfaults.clear();

    //setting heading back to logs/faults
    ui->label_heading->setText(" LOGS/FAULTS");

}

void LogsWindow::on_calendar_clicked(const QDate &date)
{
    //timer to keep the window active
    timeractive.start();
}









