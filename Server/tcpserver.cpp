#include <QCoreApplication>
#include "tcpserver.h"

tcpServer::tcpServer():var_bytesHaveRecieve(0),var_bytesHaveSent(0)
{
    var_server = new QTcpServer(this);
    connect(var_server,&QTcpServer::newConnection,this,&tcpServer::accpetConnection);
    connect(var_server,&QTcpServer::acceptError,this,&tcpServer::errorOccur);
    connect(this,&tcpServer::dataAcquisitionSignal,this,&tcpServer::dataAcauisition);
    connect(this,&tcpServer::faultDiagnosisSignal,this,&tcpServer::faultDiagnosis);
    connect(this,&tcpServer::closeConnectionSignal,this,&tcpServer::closeConnection);
}

tcpServer::~tcpServer()
{
    var_server->close();
}

bool tcpServer::start()
{
    if(var_server->listen(QHostAddress::AnyIPv4,SERVER_PORT))
    {
        qDebug("listening......");
        return true;
    }
    else
    {
        qDebug("listen failed");
        return false;
    }
}

void tcpServer::dataAcauisition()
{
    QFile file("test.xlsx");
    if(file.open(QIODevice::ReadOnly))
    {
        qDebug("data is already acquisited");
        file.close();
        sendDataFiles();
    }
    else
    {
        qDebug("data is not acquisite");
        file.close();
    }
}

void tcpServer::sendDataFiles()
{
    var_bytesHaveSent = 0;
    var_totalBytes = 0;
    QDataStream out(&var_outblock,QIODevice::WriteOnly);
    var_file.setFileName("test.xlsx");
    if(var_file.open(QIODevice::ReadOnly))
    {
        QString fileName = var_file.fileName();
        var_totalBytes = var_file.size();
        out << qint8(0) << qint64(0) << qint64(0) << fileName;
        var_totalBytes += var_outblock.size();
        out.device()->seek(0);
        //不能直接用fileName.size()，size()只是字符串的大小而不是qstring的大小
        out << qint8(TYPE_FILE) << var_totalBytes << var_outblock.size()-qint64(2*sizeof(qint64));
        var_socket->write(var_outblock);
        var_bytesHaveSent += var_outblock.size();
        var_outblock.resize(0);
    }
    else
    {
        qDebug("open test.xlsx failed");
        var_file.close();
        return;
    }
}

void tcpServer::faultDiagnosis()
{

}

void tcpServer::sendDiagnosisResult()
{
}

void tcpServer::accpetConnection()
{
    qDebug("---------->accpetConnection()");
    var_socket = var_server->nextPendingConnection();
    var_socket->setReadBufferSize(READ_BUFFER);
    connect(var_socket,&QTcpSocket::readyRead,this,&tcpServer::readyReceive);
    connect(var_socket,&QTcpSocket::bytesWritten,this,&tcpServer::readySend);
    qDebug() << "Server IP: " << var_server->serverAddress().toString();
    qDebug() << "Client IP: " << var_socket->peerAddress().toString();
}

void tcpServer::closeConnection()
{
    qDebug("---------->closeConnection()");
    var_socket->close();
}

void tcpServer::errorOccur()
{
    qDebug() << var_server->errorString();
}

void tcpServer::readySend()
{
    qDebug("---------->readySend()");
    if(var_receivedCMD == cmd_dataAcquisition)
    {
        if(var_bytesHaveSent < var_totalBytes)
        {
            var_outblock = var_file.read(var_payloadSize);
            var_bytesHaveSent += var_socket->write(var_outblock);
            var_outblock.resize(0);
        }
        else if(var_bytesHaveSent == var_totalBytes)
        {
            var_file.close();
        }
    }
    else if(var_receivedCMD == cmd_faultDiagnosis)
    {

    }
}

void tcpServer::readyReceive()
{
    qDebug("---------->readyReceive()");
    QDataStream in(var_socket);
    if(var_bytesHaveRecieve == 0)
    {
        if(var_socket->bytesAvailable() < qint64(sizeof(qint8)))
            return;
        in >> var_dataType;
        var_bytesHaveRecieve += sizeof(var_dataType);
    }
    if(var_dataType == TYPE_CMD)
    {
        if(var_socket->bytesAvailable() >= qint64(sizeof(qint64)))
        {
            in >> var_receivedCMD;
            if(var_receivedCMD == cmd_dataAcquisition)
            {
                emit dataAcquisitionSignal();
            }
            else if(var_receivedCMD == cmd_faultDiagnosis)
            {
                emit faultDiagnosisSignal();
            }
            else if(var_receivedCMD == cmd_closeConnection)
            {
                emit closeConnectionSignal();
            }
            var_bytesHaveRecieve = 0;
        }
    }
    else if(var_dataType == TYPE_DATA)
    {

    }
    else if(var_dataType == TYPE_FILE)
    {

    }
}
//-------------------------UDP------------------------------//
udpServer::udpServer(QObject *parent):QUdpSocket(parent)
{
    bind(SERVER_PORT);
    connect(this,&udpServer::readyRead,this,&udpServer::readPendingDatagrams);
    connect(this,&udpServer::bytesWritten,this,&udpServer::sendPendingDatagrams);
}

udpServer::~udpServer()
{

}

void udpServer::readIpAndHostName()
{
    QString configFilePath = QCoreApplication::applicationDirPath() + "/config.ini";
    QFileInfo fileInfo(configFilePath);
    if(fileInfo.exists())
    {
        QSettings config(configFilePath,QSettings::IniFormat);
        var_IpAddress = config.value("IPAddress").toString();
        var_HostName = config.value("HostName").toString();
        qDebug()<<"ip:"<<var_IpAddress;
        qDebug()<<"name:"<<var_HostName;
    }
    else
    {
        QSettings config(configFilePath);
        config.setValue("IPAddress","127.0.0.1");
        config.setValue("HostName",QSysInfo::machineHostName());
        qDebug("----->config.ini");
    }
}

void udpServer::readPendingDatagrams()
{
    QNetworkDatagram datagrams = receiveDatagram();
    if(datagrams.isValid())
    {
        QByteArray inblock = datagrams.data();
        QDataStream in(&inblock,QIODevice::ReadOnly);
        in >> var_receivedCMD;
        if(var_receivedCMD == cmd_sendIpAndHostName)
        {
            readIpAndHostName();
            sendDatagrams();
        }
    }
}

void udpServer::sendDatagrams()
{
    QByteArray outblock;
    QDataStream out(&outblock,QIODevice::WriteOnly);
    out << var_IpAddress << var_HostName;
    writeDatagram(outblock,QHostAddress::Broadcast,SERVER_PORT);
}

void udpServer::sendPendingDatagrams()
{
    qDebug("------------>sendPendingDatagrams");
}
