#include <QCoreApplication>
#include "tcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    int num = 0;
    tcpServer server;
    qDebug("--------Server Is Started--------\n");
    while(!server.start())
    {
        ++num;
        if(num > 20)
        {
            qDebug("server init failed\n");
            break;
        }
    }
    udpServer udp;
    udp.readIpAndHostName();
    return a.exec();
}
