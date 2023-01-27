#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QDialog>
#include <QMediaPlayer>
#include <QVideoWidget>

namespace Ui {
class audioplayer;
}

class audioplayer : public QDialog
{
    Q_OBJECT

public:
    explicit audioplayer(QWidget *parent = nullptr);
    ~audioplayer();

    void updateDurationInfo(qint64 currentInfo);


private slots:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void seek(int seconds);


    void on_pushButton_play_clicked();

    void on_pushButton_pause_clicked();

    void on_pushButton_close_clicked();

    void on_label_linkActivated(const QString &link);

private:
    Ui::audioplayer *ui;
    QMediaPlayer *player;
    QVideoWidget *vw ;

    //Duration of the video
    qint64 m_duration;
};

#endif // AUDIOPLAYER_H
