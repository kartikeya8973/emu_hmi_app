#include "slavesyncthread.h"

extern  slave_tasks_t    slave_task;

extern int inactive;

//#define POW_LO_LED      36
//#define POW_HI_LED      37
//#define POW_LO_SENSE    169
//#define POW_HI_SENSE    170
//#define MIC_DETECT      39
//#define MIC_SWITCH      2
//#define PA_SWITCH       6
//#define RS485_IO        38
//#define OP_ON           1
//#define OP_OFF          0

SlaveSyncThread::SlaveSyncThread(QObject *parent)
    : QThread(parent)
{
//#ifndef LAPTOP
//    gpio.gpio_unexport(POW_LO_LED);
//    gpio.gpio_unexport(POW_HI_LED);
//    gpio.gpio_unexport(POW_LO_SENSE);
//    gpio.gpio_unexport(POW_HI_SENSE);
//    gpio.gpio_unexport(MIC_DETECT);
//    gpio.gpio_unexport(MIC_SWITCH);
//    gpio.gpio_unexport(PA_SWITCH);
//    gpio.gpio_unexport(RS485_IO);
//    gpio.gpio_export(POW_LO_LED);
//    gpio.gpio_export(POW_HI_LED);
//    gpio.gpio_export(POW_LO_SENSE);
//    gpio.gpio_export(POW_HI_SENSE);
//    gpio.gpio_export(MIC_DETECT);
//    gpio.gpio_export(MIC_SWITCH);
//    gpio.gpio_export(PA_SWITCH);
//    gpio.gpio_export(RS485_IO);

//    gpio.gpio_set_dir(POW_LO_LED, "out");
//    gpio.gpio_set_dir(POW_HI_LED, "out");
//    gpio.gpio_set_dir(POW_LO_SENSE, "in");
//    gpio.gpio_set_dir(POW_HI_SENSE, "in");
//    gpio.gpio_set_dir(MIC_DETECT, "in");
//    gpio.gpio_set_dir(MIC_SWITCH, "out");
//    gpio.gpio_set_dir(PA_SWITCH, "out");
//    gpio.gpio_set_dir(RS485_IO, "out");

//    gpio.gpio_set_value(MIC_SWITCH,OP_OFF);
//    gpio.gpio_set_value(PA_SWITCH,OP_OFF);
//#endif

}

SlaveSyncThread::~SlaveSyncThread()
{

}

void SlaveSyncThread::setIndex(int idx)
{
    sIdx = idx;
}

void SlaveSyncThread::update_config(systemconfig sysconf)
{
    sysconfig = sysconf;
    eth_setup();
}

void SlaveSyncThread::eth_setup()
{
    foreach(QNetworkInterface netInterface, QNetworkInterface::allInterfaces())
    {
        // Return only the first non-loopback MAC Address
        if (!(netInterface.flags() & QNetworkInterface::IsLoopBack))
        {
            if(netInterface.type()==QNetworkInterface::Ethernet)
            {
                ni_eth = netInterface;
                emit eth_present();
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

                if(ni_eth.IsUp)
                {
                    emit eth_up();

                    if(ni_eth.IsRunning)
                    {
                        emit eth_con();
                    }
                    else
                    {
                        emit eth_dis();
                    }
                }
                else
                {
                    emit eth_down();
                }

                return;
            }
        }
    }

    emit eth_absent();
}

void SlaveSyncThread::run()
{
    forever
    {
//        switch(slave_task)
//        {
//        case SLAVE_NO_TASK:
//        {
//#ifndef LAPTOP

//            gpio.gpio_get_value(POW_LO_SENSE,&pow_lo_val);
//            gpio.gpio_get_value(POW_HI_SENSE,&pow_hi_val);

//            if(pow_lo_val==0 && pow_hi_val==0)
//            {
//                //pow hi
//                powerState="HIGH";
//               gpio.gpio_set_value(POW_HI_LED,OP_ON);
////               emit pow_hi();
//            }
//            else
//            if(pow_lo_val==1 && pow_hi_val==1)
//            {
//                //pow low
//                powerState="LOW";
//               gpio.gpio_set_value(POW_LO_LED,OP_ON);
////               emit pow_lo();
//            }
//            else
//            if((pow_lo_val==1 && pow_hi_val==0)||(pow_lo_val==0 && pow_hi_val==1))
//            {
//                //pow ok
//                powerState="OK";
//                gpio.gpio_set_value(POW_LO_LED,OP_OFF);
//                gpio.gpio_set_value(POW_HI_LED,OP_OFF);
////                emit pow_ok();
//            }

////            gpio.gpio_get_value(MIC_DETECT,&mic_in_val);
////            if(mic_in_val==0)
////            {
////                emit mic_in();
////                gpio.gpio_set_value(MIC_SWITCH,OP_ON);
////            }
////            else
////            {
////                emit mic_out();
////                gpio.gpio_set_value(MIC_SWITCH,OP_OFF);
////            }
//#endif
//            eth_ctr++;
//            if(eth_ctr==5)
//            {
//                 eth_ctr=0;
//                if(has_eth)
//                {
//                    if(ni_eth.IsRunning)
//                    {
//                        emit eth_con();
//                    }
//                    else
//                    {
//                        emit eth_dis();

//                        if(ni_eth.IsUp)
//                        {
//                            emit eth_up();
//                        }
//                        else
//                        {
//                            emit eth_down();
//                        }
//                    }
//                }
//                else
//                {
//                    eth_setup();
//                }
//            }
//            this->msleep(1000);
//        }
//            break;
//        case SLAVE_INIT:
//        {
//                //ddc doesnt support connmanctl
////            eth_setup();

////            QString ping_req ="ping -w 1 "+sysconfig.eth_ip_addr;
////            while(system(ping_req.toLocal8Bit())){this->msleep(2000);}
//            emit init_done();
//            slave_task=SLAVE_NO_TASK;
//        }
//            break;
//        case SLAVE_SCAN_IP:
//        {
//            int pctr;
//            if(papis_slaves.at(sIdx)->installStatus==INSTALLED)
//            {
//                pctr=0;
//                QString ping_req ="ping -w 1 "+papis_slaves.at(sIdx)->ip_addr;
//                while(system(ping_req.toLocal8Bit()))
//                {
//                    pctr++;
//                    emit slave_scan_progress_eth(100/pctr,papis_slaves);
//                    if(pctr==4)
//                    {
//                        inactive++;

//                        papis_slaves.at(sIdx)->activeStatus = INACTIVE;
//                        break;
//                    }
//                }
//                emit slave_scan_progress_eth(100,papis_slaves);
//            }
//        }
//        break;
//        case SLAVE_SCAN_ALL:
//        {
            int idx;
            int pctr;
            inactive=0;
            for(idx=0; idx< papis_slaves.count();idx++)
            {
                if(/*papis_slaves.at(idx)->conn_mode=="ETH" &&*/ papis_slaves.at(idx)->installStatus == INSTALLED)
                {
                    pctr=0;
                    papis_slaves.at(idx)->activeStatus = ACTIVE;
                    QString ping_req ="ping -w 1 "+papis_slaves.at(idx)->ip_addr;
                    while(system(ping_req.toLocal8Bit()))
                    {
                        pctr++;
                        if(pctr==3)
                        {
                            inactive++;

                            papis_slaves.at(idx)->activeStatus = INACTIVE;
                            break;
                        }
                    }
                    emit slave_scan_progress_eth(100*idx/papis_slaves.count(),papis_slaves);
                }
            }
            emit slave_scan_progress_eth(100,papis_slaves);
            slave_task = SLAVE_NO_TASK;
        }
//            break;
//        default:
//            break;

//        }
//    }
}

//void SlaveSyncThread::eth_scan_all(QList <slave*> slaves)
//{
//    papis_slaves=slaves;
//    slave_task = SLAVE_SCAN_ALL;
//}
