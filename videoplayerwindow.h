#ifndef VIDEOPLAYERWINDOW_H
#define VIDEOPLAYERWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QSlider>
#include <QProgressBar>

namespace Ui {
class VideoPlayerWindow;
}

class VideoPlayerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VideoPlayerWindow(QWidget *parent = nullptr);
    ~VideoPlayerWindow();

private slots:
    void on_slider_sliderMoved(int position);

private:
    Ui::VideoPlayerWindow *ui;
    QMediaPlayer *player;
    QVideoWidget *vw ;
    QSlider *slider;
    QProgressBar *bar;
};

#endif // VIDEOPLAYERWINDOW_H
