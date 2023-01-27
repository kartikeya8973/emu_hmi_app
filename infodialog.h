#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include <QTimer>
#include <QLayout>
#include <QStatusBar>
#include <QLabel>

namespace Ui {
class infoDialog;
}

class infoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit infoDialog(QWidget *parent = nullptr);
    ~infoDialog();

public slots:
    void infostatusDataTime();

private:
    Ui::infoDialog *ui;
    QTimer *timer;
    QStatusBar *statusBar;
    QLabel *label_Date;
    QLabel *label_Time;
};

#endif // INFODIALOG_H
