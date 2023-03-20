#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QDialog>
#include <QPixmap>
#include <QWidget>

class QCheckBox;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QSpinBox;
class QVBoxLayout;

namespace Ui {
class Screenshot;
}

class Screenshot : public QDialog
{
    Q_OBJECT

public:
    explicit Screenshot(QWidget *parent = nullptr);
    ~Screenshot();
protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void newScreenshot();
    void saveScreenshot();
    void shootScreen();
    void updateCheckBox();

private:
    Ui::Screenshot *ui;
    QPushButton *createButton(const QString &text, QWidget *receiver,
                              const char *member);
    void createOptionsGroupBox();
    void createButtonsLayout();
    void updateScreenshotLabel();
    QPixmap originalPixmap;


    QLabel *screenshotLabel;
    QGroupBox *optionsGroupBox;
    QSpinBox *delaySpinBox;
    QLabel *delaySpinBoxLabel;
    QCheckBox *hideThisWindowCheckBox;
    QPushButton *newScreenshotButton;
    QPushButton *saveScreenshotButton;
    QPushButton *quitScreenshotButton;
    QVBoxLayout *mainLayout;
    QGridLayout *optionsGroupBoxLayout;
    QHBoxLayout *buttonsLayout;
};

#endif // SCREENSHOT_H
