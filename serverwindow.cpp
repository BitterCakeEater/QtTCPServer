#include "serverwindow.h"
#include "ui_serverwindow.h"
#include "TCPServer.h"

ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serverStarted = false;
    readLog = false;
    serverStatus = 0;
    ui->label_status->setText("Status: Off");

    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerFunction()));
    timer->start();
}

ServerWindow::~ServerWindow()
{
    delete ui;
}


void ServerWindow::on_pushButton_Start_clicked()
{
    if(!serverStarted && serverStatus == 0)
    {
        serverStatus = 2;
        Server.startServer();
        ui->label_status->setText("Status: On");
        ui->pushButton_Start->setText("Stop");
        serverStarted = true;
        readLog = true;
    }
    else
    {
        Server.stopServer();
        serverStatus = 0;
        serverStarted = false;
        ui->label_status->setText("Status: Off");
        ui->pushButton_Start->setText("Start");
    }
}

void ServerWindow::timerFunction()
{
    if(serverStarted)
    {
        ui->textEdit_Log->setText(QString::fromStdString(Server.log.str()));
        ui->textEdit_Log->verticalScrollBar()->setValue(ui->textEdit_Log->verticalScrollBar()->maximum());
    }

    else if(!serverStarted && readLog)
    {
        //serverStatus = 0;
        readLog = false;
        ui->textEdit_Log->setText(QString::fromStdString(Server.log.str()));
        ui->textEdit_Log->verticalScrollBar()->setValue(ui->textEdit_Log->verticalScrollBar()->maximum());
        //qDebug() << serverStatus;
    }
}

void ServerWindow::on_pushButton_Pause_clicked()
{
    if(serverStatus == 2){
        serverStatus = 1;
        Server.pauseServer();
        serverStarted = false;
        ui->label_status->setText("Status: Paused");
        ui->pushButton_Pause->setText("Unpause");
        //qDebug() << serverStatus;
    }
    else if(serverStatus == 1)
    {
        serverStatus = 2;
        Server.unpauseServer();
        serverStarted = true;
        readLog = true;
        ui->label_status->setText("Status: On");
        ui->pushButton_Pause->setText("Pause");
        //qDebug() << serverStatus;
    }
}

void ServerWindow::closeEvent (QCloseEvent *event)
{
    Server.stopServer();
}
