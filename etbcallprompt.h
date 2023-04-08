#ifndef ETBCALLPROMPT_H
#define ETBCALLPROMPT_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class etbCallPrompt;
}

class etbCallPrompt : public QDialog
{
    Q_OBJECT

public:
    explicit etbCallPrompt(QWidget *parent = nullptr);
    ~etbCallPrompt();

signals:
    // signal for ok button(present in the access dialog) press
    void yesbuttonPressed();

    void renameAudio();


private slots:
    void on_pushButton_no_clicked();

    void on_pushButton_yes_clicked();

    void opendefaultScreen();

private:
    Ui::etbCallPrompt *ui;

    QTimer *timer = new QTimer(this);
};

#endif // ETBCALLPROMPT_H
