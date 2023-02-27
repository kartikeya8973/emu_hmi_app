#ifndef CUSTOMLE_H
#define CUSTOMLE_H

#include <QWidget>
#include <QLineEdit>
#include <QFocusEvent>
#include "keyboarddialog.h"

class customle : public QLineEdit
{
    Q_OBJECT
public:
    explicit customle(QWidget *parent = nullptr);

signals:
    void entered_text(QString);
public slots:
    void entered(QString);
protected:
    bool eventFilter(QObject* object, QEvent* event);
private:
    bool skip=false;
};

#endif // CUSTOMLE_H
