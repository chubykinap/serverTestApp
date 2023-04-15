#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDate>
#include <QHostInfo>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_buttonStart_clicked();
    void newConnection();
    void readClient();
    void disconnectClient();
    void on_buttonSend_clicked();
    void writeLog(QString text, QTcpSocket* socket);
    QString getHostIp();
    void clearConnections();

private:
    Ui::MainWindow *ui;
    QTcpServer *server;
    QList<QTcpSocket *> clientSockets;
    int serverPort;
};
#endif // MAINWINDOW_H
