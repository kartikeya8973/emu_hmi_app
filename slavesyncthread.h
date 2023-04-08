#ifndef SLAVESYNCTHREAD_H
#define SLAVESYNCTHREAD_H

#include <QObject>
#include <QThread>
#include <QNetworkInterface>

#include "slave.h"
#include "common.h"
//#include "gpio_utils.h"
#include "systemconfig.h"

class SlaveSyncThread : public QThread
{
    Q_OBJECT
public:
    SlaveSyncThread(QObject *parent = 0);
    ~SlaveSyncThread();
    void setIndex(int sIdx);

    void eth_setup();
    void update_config(systemconfig sysconf);

signals:
    void slave_scan_progress_eth(int pc,QList <slave*> slaves);
    void slaves_cleared();
    void slaves_driven();

    void pow_lo();
    void pow_hi();
    void pow_ok();
    void mic_in();
    void mic_out();
    void tms_alarm(int);

    void eth_present();
    void eth_absent();
    void eth_up();
    void eth_down();
    void eth_con();
    void eth_dis();

    void init_done();

protected:
    void run() override;

private:
    int sIdx=0;
//    gpio_utils gpio;
    unsigned int pow_hi_val=0, pow_lo_val=0, mic_in_val=0;
    int eth_ctr=0;

    QNetworkInterface ni_eth;
    QString mac_eth;
    bool has_eth=false;

    systemconfig sysconfig;

//    QNetworkInterface ni_wifi;
//    QString mac_wifi;
//    bool has_wifi=false;

    QString powerState="";
    QList <slave*> papis_slaves;

    QMutex m_idle;
    QWaitCondition wc_idle;

private slots:
    void eth_scan_all(QList <slave*> papis_slaves);
};

#endif // SLAVESYNCTHREAD_H
