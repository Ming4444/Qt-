#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QByteArray>
#include <QTimer>
#include <QThread>

#define SERVER_PORT 44444
#define READ_BUFFER 1024*1024
#define WRITE_BUFFER 1024*1024

const qint64 cmd_sendIpAndHostName = 111;
const qint64 cmd_dataAcquisition = 333;
const qint64 cmd_faultDiagnosis = 444;
const qint64 cmd_closeConnection = 999;

enum _DataType
{
    TYPE_CMD=1, TYPE_DATA=2, TYPE_FILE=3
};

class tcpServer : public QObject
{
    Q_OBJECT
public:
    tcpServer();
    ~tcpServer();
    bool start();
    void sendDataFiles();
    void sendDiagnosisResult();

public slots:
    void accpetConnection();
    void errorOccur();
    void readySend();
    void readyReceive();
    void dataAcauisition();
    void faultDiagnosis();
    void closeConnection();

signals:
    void dataAcquisitionSignal();
    void faultDiagnosisSignal();
    void closeConnectionSignal();

private:
    const qint64 var_payloadSize = 64*1024; // 每次发送块的大小=64KB
    QTcpServer *var_server;
    QTcpSocket *var_socket;
    QFile var_file;
    QThread *var_udpThread;
    // 接收
    qint8 var_dataType;
    qint64 var_bytesHaveRecieve;
    qint64 var_totalBytesShouldRecieve;
    qint64 var_receivedCMD;
    // 发送
    QByteArray var_outblock;
    qint64 var_bytesHaveSent;
    qint64 var_totalBytes;
};

class udpServer : public QUdpSocket
{
    Q_OBJECT
public:
    udpServer(QObject *parent = nullptr);
    virtual ~udpServer();

public slots:
    void readPendingDatagrams();
    void sendPendingDatagrams();
    void sendDatagrams();
    void readIpAndHostName();

private:
    QString var_IpAddress;
    QString var_HostName;
    qint64 var_receivedCMD;
};

#endif // TCPSERVER_H
