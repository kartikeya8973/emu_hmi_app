#include "menudialog.h"
#include "ui_menudialog.h"

menuDialog::menuDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::menuDialog)
{
    ui->setupUi(this);
}

menuDialog::~menuDialog()
{
    delete ui;
}
