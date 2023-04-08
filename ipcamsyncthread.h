#ifndef IPCAMSYNCTHREAD_H
#define IPCAMSYNCTHREAD_H

#include <QObject>
#include <QThread>
#include <QNetworkInterface>

#include "camera.h"
#include "common.h"
//#include "gpio_utils.h"
#include "systemconfig.h"

#include <QObject>
#include <QThread>
#include <QNetworkInterface>

#include "slave.h"
#include "common.h"
//#include "gpio_utils.h"
#include "systemconfig.h"

class IpCamSyncThread : public QThread
{
    Q_OBJECT
public:
    IpCamSyncThread(QObject *parent = nullptr);
    void setIndex(int sIdx);

    void eth_setup();
    void update_config(systemconfig sysconf);

signals:
    void slave_scan_progress_eth(/*int pc,*/ QList <camera*> cameras);
    void slaves_cleared();
    void slaves_driven();

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
//    QList <camera*> hmi_slaves;

    QMutex m_idle;
    QWaitCondition wc_idle;

private slots:

};


#endif // IPCAMSYNCTHREAD_H
