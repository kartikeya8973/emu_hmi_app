#include "ipcamsyncthread.h"

extern  slave_tasks_t    slave_task;

extern int inactive;

extern QList <camera*> hmi_slaves;


IpCamSyncThread::IpCamSyncThread(QObject *parent)
    :QThread(parent)
{

}

void IpCamSyncThread::setIndex(int idx)
{
    sIdx = idx;
}

void IpCamSyncThread::update_config(systemconfig sysconf)
{
    sysconfig = sysconf;
//    eth_setup();
}

void IpCamSyncThread::eth_setup()
{
    foreach(QNetworkInterface netInterface, QNetworkInterface::allInterfaces())
    {
        // Return only the first non-loopback MAC Address
        if (!(netInterface.flags() & QNetworkInterface::IsLoopBack))
        {
            if(netInterface.type()==QNetworkInterface::Ethernet)
            {
                ni_eth = netInterface;
                has_eth=true;

                mac_eth = ni_eth.hardwareAddress().remove(QChar(':')).toLower();
                if(sysconfig.eth_ip_mode=="STATIC")
                {
                    system(QString("connmanctl config ethernet_"+mac_eth+"_cable --ipv4 manual "+sysconfig.eth_ip_addr+" "+sysconfig.eth_net_mask
                                   +" "+sysconfig.eth_gateway).toLocal8Bit());
                    system(QString("connmanctl config ethernet_"+mac_eth+"_cable --nameservers "+sysconfig.eth_dns).toLocal8Bit());
                }
                else
                {
                    system(QString("connmanctl config ethernet_"+mac_eth+"_cable --ipv4 dhcp").toLocal8Bit());
                }

                //apply configuration

                //                if(ni_eth.IsUp)
                //                {
                //                    emit eth_up();

                //                    if(ni_eth.IsRunning)
                //                    {
                //                        emit eth_con();
                //                    }
                //                    else
                //                    {
                //                        emit eth_dis();
                //                    }
                //                }
                //                else
                //                {
                //                    emit eth_down();
                //                }

                return;
            }
        }
    }

    //    emit eth_absent();
}

void IpCamSyncThread::run()
{
    forever
    {
        int idx;
        int pctr;
        inactive=0;
        for(idx=0; idx< hmi_slaves.count();idx++)
        {
            if(hmi_slaves.at(idx)->installStatus == INSTALLED)
            {
                pctr=0;
                hmi_slaves.at(idx)->activeStatus = ACTIVE;
                QString ping_req ="ping -w 1 "+hmi_slaves.at(idx)->ip_addr;
                while(system(ping_req.toLocal8Bit()))
                {
                    pctr++;
                    if(pctr==3)
                    {
                        inactive++;

                        hmi_slaves.at(idx)->activeStatus = INACTIVE;
                        break;
                    }
                }
                emit slave_scan_progress_eth(/*100*idx/hmi_slaves.count(),*/hmi_slaves);
            }
        }
    }
}
