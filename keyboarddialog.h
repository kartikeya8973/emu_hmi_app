#ifndef KEYBOARDDIALOG_H
#define KEYBOARDDIALOG_H

#include <QDialog>

namespace Ui {
class KeyboardDialog;
}

class KeyboardDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KeyboardDialog(QWidget *parent = nullptr);
    ~KeyboardDialog();

    QPushButton *enterButton;
    QTimer *btnTimer;
    bool btnBusy=false;
    QString inputText;

    void setText(QString text);
    void clearLineEdit();

signals:
    void entered(QString);
    void text_entered(QString);

private slots:
    void keyPressHandler();
    void keyReleaseHandler();
    void keyboardHandler();
    void on_shift_clicked();
    void on_char_2_toggled(bool checked);
    void on_clear_clicked();
    void on_enterButton_clicked();
    void on_backButton_clicked();

private:
    Ui::KeyboardDialog *ui;
    QString outputText;
    bool shift;
};

#endif // KEYBOARDDIALOG_H
