#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QTcpSocket>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QFile>
#include <QDataStream>
#include <QByteArray>

#define SERVER_PORT 44444
#define READ_BUFFER 1024*1024
#define WRITE_BUFFER 1024*1024

const qint64 cmd_sendIpAndHostName = 111;
const qint64 cmd_dataAcquisition = 333;
const qint64 cmd_faultDiagnosis = 444;
const qint64 cmd_closeConnection = 999;

QList<QStringList>IpAndHostName;

enum _DataType
{
    TYPE_CMD=1, TYPE_DATA=2, TYPE_FILE=3
};

class TcpClient: public QTcpSocket
{
    Q_OBJECT
public:
    TcpClient(QObject *parent = nullptr);
    ~TcpClient();
    void receiveData();
    void receiveFiles(QString path);//通过信号把路径信息传过来，在新线程内直接存文件

public slots:
    void createConnection(const QString &addr, const quint16 port = SERVER_PORT);
    void connectSuccess();
    void closeConnection();
    void closeSuccess();
    void readyReceive();
    void sendCommand(qint64 cmd);
    void readySend();
    void errorOccur();

signals:
    void errorSignal(QString err);

private:
    QFile var_newFiles;
    QString var_filePath;
    QString var_fileName;
    // 接收
    QByteArray var_fileContent;
    qint8 var_dataType;
    qint64 var_bytesHaveReceive;
    qint64 var_toalBytesShouldReceive;
    qint64 var_fileNameSize;
    // 发送
    qint64 var_cmd;
};

class udpClient : public QUdpSocket
{
    Q_OBJECT
public:
    udpClient(QObject *parent = nullptr);
    virtual ~udpClient();

public slots:
    void readPendingDatagrams();
    void sendPendingDatagrams();
    void sendCommand();
    void errorOccur();

private:
    QString var_IpAddress;
    QString var_HostName;
};

#endif // TCPCLIENT_H
