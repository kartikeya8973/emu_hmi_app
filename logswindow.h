#ifndef LOGSWINDOW_H
#define LOGSWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QWidget>
#include <QDateTime>
#include <QtCore>
#include <QtGui>
#include <QFileSystemModel>

QT_BEGIN_NAMESPACE
class QCalendarWidget;
//class QCheckBox;
//class QComboBox;
//class QDate;
//class QDateEdit;
//class QGridLayout;
//class QGroupBox;
//class QLabel;
QT_END_NAMESPACE

namespace Ui {
class LogsWindow;
}

class LogsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LogsWindow(QWidget *parent = nullptr);
    ~LogsWindow();

signals:
    // signal for home button(present in the logs window) press
    void homebuttonPressedlogs();

    // signal for return button(present in the logs window) press
    void returnbuttonPressedlogs();

public slots:
    void statusDateTimelogs();

private slots:
    void on_pushButton_home_button_clicked();

    //to set selected date to dateEdit widget
    void selectedDateChanged();

    //On page 1
    void on_pushButton_openLogs_clicked();

    //On page 2
//    void on_pushButton_openfile_clicked();

//    void on_treeView_pressed(const QModelIndex &index);

    void on_pushButton_return_clicked();

    void on_calendar_clicked(const QDate &date);

    void on_pushButton_openfaults_clicked();

    void on_treeView_2_pressed(const QModelIndex &index);

    void on_pushButton_copy_clicked();

    void on_pushButton_copylogs2USB_clicked();

    void on_pushButton_copyfault2USB_clicked();

    void on_pushButton_copy_hmi_logs_clicked();

private:
    Ui::LogsWindow *ui;
    QTimer *timer;
    QCalendarWidget *calendar;

    //Model for showing logs in listview
    QStringListModel *filemodellogs;

    //Model for showing faults in listview
    QStringListModel *filemodelsyslogs;

    //For USB copy page
    QFileSystemModel *model2;

};

#endif // LOGSWINDOW_H
