#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initTableView();

    var_thread = new QList<QThread>;
    var_udpClient = new udpClient(this);

    connect(ui->node_tableView,&QTableView::customContextMenuRequested,this,&MainWindow::popUpContextMenu);
    connect(ui->searchNode_button,&QPushButton::clicked,this,&MainWindow::updateServerInformation);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initTableView()
{
    var_itemModel = new QStandardItemModel(this);
    var_itemModel->setHorizontalHeaderLabels(QStringList({"IP Adress","Port Number","Host Name"}));

    var_delegate = new CenteredDelegate(ui->node_tableView); // 创建一个委托
    ui->node_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->node_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->node_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    ui->node_tableView->verticalHeader()->setHidden(true);
    ui->node_tableView->setItemDelegate(var_delegate); // 通过委托设置格式
    ui->node_tableView->setModel(var_itemModel);
}

void MainWindow::updateServerInformation()
{
    QList<QStandardItem*>item;
    QTime time;
    item.append(new QStandardItem(QString("%1").arg(time.currentTime().hour())));
    item.append(new QStandardItem(QString("%1").arg(time.currentTime().minute())));
    item.append(new QStandardItem(QString("%1").arg(time.currentTime().second())));
    var_itemModel->appendRow(item);
    ui->node_tableView->setModel(var_itemModel);

    var_standardItem = nullptr;
}

void MainWindow::popUpContextMenu(const QPoint &pos)
{
    if(!ui->node_tableView->indexAt(pos).isValid())
        return;

    int row = ui->node_tableView->indexAt(pos).row();
    ui->node_tableView->selectRow(row); //解决摁住鼠标再松开不显示鼠标所指row的问题

    QMenu contextMenu;
    QAction *connectAction = contextMenu.addAction("连接");
    QAction *cancelAction = contextMenu.addAction("取消");
    // QAction *selectedAction = contextMenu.exec(ui->node_tableView->mapToGlobal(pos));
    QAction *selectedAction = contextMenu.exec(ui->node_tableView->viewport()->mapToGlobal(pos));
    if(selectedAction == connectAction)
    {
        QString ip = var_itemModel->item(row,0)->text(); // IP Address
        QString name = var_itemModel->item(row,2)->text(); // Host name
        qDebug() << "Ip:" << ip;
        qDebug() << "Name:" << name;
    }
    else if(selectedAction == cancelAction)
    {
        qDebug("close!!");
    }
}


