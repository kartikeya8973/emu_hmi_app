#include "etbcallprompt.h"
#include "ui_etbcallprompt.h"
#include "mainwindow.h"
#include "defaults.h"

etbCallPrompt::etbCallPrompt(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::etbCallPrompt)
{
    ui->setupUi(this);
}

etbCallPrompt::~etbCallPrompt()
{
    delete ui;
}

void etbCallPrompt::on_pushButton_no_clicked()
{
    close();
    emit renameAudio();
    opendefaultScreen();
}


void etbCallPrompt::on_pushButton_yes_clicked()
{
    close();
    emit yesbuttonPressed();
    emit renameAudio();
    opendefaultScreen();
}

void etbCallPrompt::opendefaultScreen()
{
    DefaultS *defaultS = new DefaultS(this);
    defaultS->setWindowFlag(Qt::FramelessWindowHint);
    defaultS->showFullScreen();
}
