#include "customle.h"

customle::customle(QWidget *parent) : QLineEdit(parent)
{
    installEventFilter(this);
}

bool customle::eventFilter(QObject* object, QEvent* event)
{
    if(event->type()==QEvent::MouseButtonPress)
    {
        KeyboardDialog *kd = new KeyboardDialog();
        kd->setModal(true);
        kd->setWindowFlags(Qt::FramelessWindowHint);
        connect(kd,SIGNAL(entered(QString)),this,SLOT(entered(QString)));
        kd->setText(this->text());
        #ifndef LAPTOP
//        kd->show();
        kd->showFullScreen();
        #else
        kd->showFullScreen();
        #endif
        update();
    }
    return false;
}

void customle::entered(QString str)
{
    this->setText(str);
    emit entered_text(str);
}
