#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include "TCPServer.h"
#include <QMainWindow>
#include <QTimer>
#include <QScrollBar>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    ServerWindow(QWidget *parent = nullptr);
    ~ServerWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_pushButton_Start_clicked();
    void timerFunction();

    void on_pushButton_Pause_clicked();

private:
    Ui::MainWindow *ui;
    TCPServer Server;
    bool serverStarted;
    bool readLog;
    int serverStatus;
    QTimer *timer;
};
#endif // MAINWINDOW_H
