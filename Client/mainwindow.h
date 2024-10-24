#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tcpclient.h"
#include <QMainWindow>
#include <QStandardItemModel>
#include <QTableView>
#include <QThread>
#include <QMouseEvent>
#include <QTimer>
#include <QTime>
#include <QStyledItemDelegate>
#include <QPainter>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class CenteredDelegate : public QStyledItemDelegate
{
public:
    CenteredDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent)
    {

    }
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        // 设置文本对齐方式为居中
        opt.displayAlignment = Qt::AlignCenter;

        QStyledItemDelegate::paint(painter, opt, index);
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void popUpContextMenu(const QPoint &pos);
    void initTableView();
    void updateServerInformation();
private:
    Ui::MainWindow *ui;

    QList<QThread>*var_thread;
    QList<TcpClient>*var_tcpClient;
    QUdpSocket *var_udpClient;
    QStandardItemModel *var_itemModel;
    QStandardItem *var_standardItem;
    CenteredDelegate *var_delegate;
};


#endif // MAINWINDOW_H
