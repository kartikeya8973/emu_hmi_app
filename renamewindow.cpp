#include "renamewindow.h"
#include "ui_renamewindow.h"

QString new_name = "";

//For Video Archives window
extern QString fileabspathmp4;
extern int renameVid;

//For ETB Archives window
extern QString fileabspathmp3;
extern int renameEtb;

//For Settings window
extern QString fileabspath;
extern int renameSet;

renamewindow::renamewindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::renamewindow)
{
    ui->setupUi(this);
    le = new customle(this);
    le->setStyleSheet("font-size: 12px");
    ui->horizontalLayout->addWidget(le);
}

renamewindow::~renamewindow()
{
    delete ui;
}

void renamewindow::on_pushButton_cancel_clicked()
{
    close();

    //rename dialog called by videoArchives window
    if(renameVid ==1)
    {
        new_name = fileabspathmp4;
        renameVid = 0;
    }

    //rename dialog called by ETBArchives window
    if(renameEtb ==1)
    {
        new_name = fileabspathmp3;
        renameEtb = 0;
    }

    //rename dialog called by Settings window
    if(renameSet ==1)
    {
        new_name = fileabspath;
        renameSet = 0;
    }
}

void renamewindow::on_pushButton_rename_clicked()
{
    new_name=le->text();

    ui->label_status->setText("File name updated to - " + new_name);

    close();
}

