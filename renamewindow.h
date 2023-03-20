#ifndef RENAMEWINDOW_H
#define RENAMEWINDOW_H

#include <QDialog>
#include "customle.h"

namespace Ui {
class renamewindow;
}

class renamewindow : public QDialog
{
    Q_OBJECT

public:
    explicit renamewindow(QWidget *parent = nullptr);
    ~renamewindow();

private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_rename_clicked();

private:
    Ui::renamewindow *ui;
    //For adding new password
    customle *le;
};

#endif // RENAMEWINDOW_H
