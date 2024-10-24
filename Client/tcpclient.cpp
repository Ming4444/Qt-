#include "tcpclient.h"

TcpClient::TcpClient(QObject *parent):QTcpSocket(parent),var_bytesHaveReceive(0)
{
    setReadBufferSize(READ_BUFFER); // 1024*1024个字节大小
    connect(this,&TcpClient::errorSignal,this,&TcpClient::errorOccur);
    connect(this,&TcpClient::readyRead,this,&TcpClient::readyReceive);
    connect(this,&TcpClient::bytesWritten,this,&TcpClient::readySend);
}

TcpClient::~TcpClient()
{
}

void TcpClient::createConnection(const QString &addr, quint16 port)
{
    if(state() == QAbstractSocket::UnconnectedState)
    {
        abort();
        connectToHost(addr,port);
        connect(this,&TcpClient::connected,this,&TcpClient::connectSuccess);
    }
}

void TcpClient::connectSuccess()
{
    qDebug("---------->connectSuccess()");
    qDebug() << "Server IP: " << peerAddress().toString();
    qDebug() << "Client IP: " << localAddress().toString();
}

void TcpClient::closeConnection()
{
    disconnectFromHost();
    connect(this,&TcpClient::disconnected,this,&TcpClient::closeSuccess);
}

void TcpClient::closeSuccess()
{
    qDebug("---------->closeSuccess()");
}

void TcpClient::receiveData()
{

}

void TcpClient::receiveFiles(QString path) //通过mainwindow信号驱动
{
    var_filePath = path;
    sendCommand(cmd_dataAcquisition);
}

void TcpClient::errorOccur()
{
    qDebug() << errorString();
}

void TcpClient::readyReceive()
{
    QDataStream in(this); //没有">>"操作，就没有把数据取出来，所以就算销毁in对象，数据也会存在buffer中
    if(var_bytesHaveReceive == 0)
    {
        if(bytesAvailable() < qint64(sizeof(qint8)))
            return;
        in >> var_dataType;
        var_bytesHaveReceive += sizeof(var_dataType);
    }
    if(var_dataType == TYPE_FILE)
    {
        if(var_bytesHaveReceive <= qint64(sizeof(qint64)*2+sizeof(var_dataType)))
        {
            if(bytesAvailable()>=qint64(sizeof(qint64)*2) && var_bytesHaveReceive<qint64(sizeof(qint64)*2+sizeof(qint8)))
            {
                in >> var_toalBytesShouldReceive >> var_fileNameSize;
                var_bytesHaveReceive += sizeof(qint64)*2;
            }
            if(bytesAvailable()>=var_fileNameSize && (var_fileNameSize!=0))
            {
                in >> var_fileName;
                var_bytesHaveReceive += var_fileNameSize;
                var_newFiles.setFileName(var_filePath+var_fileName);
                if(!var_newFiles.open(QIODevice::WriteOnly))
                {
                    qDebug("file open failed");
                    return;
                }
            }
            else
            {
                return;
            }
        }
        if(var_bytesHaveReceive < var_toalBytesShouldReceive)
        {
            if(bytesAvailable() >= var_toalBytesShouldReceive-var_bytesHaveReceive)
            {
                var_fileContent = this->read(var_toalBytesShouldReceive-var_bytesHaveReceive);
            }
            else
            {
                var_fileContent = this->readAll();
            }
            var_bytesHaveReceive += var_fileContent.size();
            var_newFiles.write(var_fileContent);
            var_fileContent.resize(0);

        }
        if(var_bytesHaveReceive == var_toalBytesShouldReceive)
        {
            qDebug("write complete");
            var_newFiles.close();
            var_bytesHaveReceive = 0;
            var_toalBytesShouldReceive = 0;
        }
    }
    else if(var_dataType == TYPE_DATA)
    {

    }
    else if(var_dataType == TYPE_CMD)
    {

    }
}

void TcpClient::sendCommand(qint64 cmd)
{
    if(cmd == cmd_dataAcquisition)
    {
        var_cmd = cmd_dataAcquisition;
    }
    else if(cmd == cmd_faultDiagnosis)
    {
        var_cmd = cmd_faultDiagnosis;
    }
    else if(cmd == cmd_closeConnection)
    {
        var_cmd = cmd_closeConnection;
    }
    QByteArray outblock;
    QDataStream out(&outblock,QIODevice::WriteOnly);
    out << qint8(TYPE_CMD) << var_cmd;
    write(outblock);
}

void TcpClient::readySend()
{
    qDebug("---------->readySend()");
}

//-------------------------UDP------------------------------//
udpClient::udpClient(QObject *parent):QUdpSocket(parent)
{
    bind(SERVER_PORT);
    connect(this,&udpClient::readyRead,this,&udpClient::readPendingDatagrams);
    connect(this,&udpClient::bytesWritten,this,&udpClient::sendPendingDatagrams);
}

udpClient::~udpClient()
{

}

void udpClient::readPendingDatagrams()
{
    QNetworkDatagram datagrams = receiveDatagram();
    if(datagrams.isValid())
    {
        QByteArray inblock = datagrams.data();
        QDataStream in(&inblock,QIODevice::ReadOnly);
        in >> var_IpAddress >> var_HostName;
        IpAndHostName.append(QStringList() << var_IpAddress << var_HostName);
    }
}

void udpClient::sendCommand()
{
    QByteArray outblock;
    QDataStream out(&outblock,QIODevice::WriteOnly);
    out << cmd_sendIpAndHostName;
    writeDatagram(outblock,QHostAddress::Broadcast,SERVER_PORT);

    IpAndHostName.resize(0);
}

void udpClient::sendPendingDatagrams()
{
    qDebug("------------>sendPendingDatagrams");
}
