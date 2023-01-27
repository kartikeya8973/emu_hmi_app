#ifndef MENUDIALOG_H
#define MENUDIALOG_H

#include <QDialog>

namespace Ui {
class menuDialog;
}

class menuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit menuDialog(QWidget *parent = nullptr);
    ~menuDialog();

private:
    Ui::menuDialog *ui;
};

#endif // MENUDIALOG_H
