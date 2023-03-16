#include "pingthread.h"

PingThread::PingThread(QObject *parent) : QThread(parent)
{

}

QString result = "";

int ping_for_cam1;
int ping_for_cam2;
int ping_for_cam3;
int ping_for_cam4;
int ping_for_cam5;
int ping_for_cam6;
int ping_for_cam7;
int ping_for_cam8;
int ping_for_cam9;
int ping_for_cam10;
int ping_for_cam11;
int ping_for_cam12;

int ping_for_nvr1;
int ping_for_nvr2;

void PingThread::run()
{
    // ping devices and update UI code here

    ping_for_cam1 = system("ping -c 1 192.168.1.221 > /dev/null 2>&1");
    ping_for_cam2 = system("ping -c 1 192.168.1.222 > /dev/null 2>&1");
    ping_for_cam3 = system("ping -c 1 192.168.1.223 > /dev/null 2>&1");
    ping_for_cam4 = system("ping -c 1 192.168.1.224 > /dev/null 2>&1");
    ping_for_cam5 = system("ping -c 1 192.168.1.225 > /dev/null 2>&1");
    ping_for_cam6 = system("ping -c 1 192.168.1.226 > /dev/null 2>&1");
    ping_for_cam7 = system("ping -c 1 192.168.1.227 > /dev/null 2>&1");
    ping_for_cam8 = system("ping -c 1 192.168.1.228 > /dev/null 2>&1");
    ping_for_cam9 = system("ping -c 1 192.168.1.229 > /dev/null 2>&1");
    ping_for_cam10 = system("ping -c 1 192.168.1.230 > /dev/null 2>&1");
    ping_for_cam11 = system("ping -c 1 192.168.1.231 > /dev/null 2>&1");
    ping_for_cam12 = system("ping -c 1 192.168.1.232 > /dev/null 2>&1");

    ping_for_nvr1 = system("ping -c 1 192.168.1.2 > /dev/null 2>&1");
    ping_for_nvr2 = system("ping -c 1 192.168.1.232 > /dev/null 2>&1");

}
