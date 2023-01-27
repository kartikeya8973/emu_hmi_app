#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

signals:
    // signal for ok button(present in the access dialog) press
    void okbuttonPressed();

private slots:
    void on_pushButton_close_clicked();

    void on_pushButton_1_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_0_clicked();

    void on_pushButton_ok_clicked();

private:
    Ui::LoginDialog *ui;
    QString accessKey = "";

};

#endif // LOGINDIALOG_H
