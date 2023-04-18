#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "imageform.h"
#include <QDate>
#include <QFile>
#include <QFileInfo>
#include <QHostInfo>
#include <QMainWindow>
#include <QMessageBox>
#include <QMutex>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_buttonStart_clicked();
    bool checkServerCredentials(QString port);
    void newConnection();
    void readClient();
    void disconnectClient();
    void on_buttonSend_clicked();
    void writeLog(QString text, QTcpSocket* socket);
    QString getHostIp();
    void clearConnections();

private:
    Ui::MainWindow* ui;
    QTcpServer* server;
    QList<QTcpSocket*> clientSockets;
    int serverPort;
    imageForm form;
    QByteArray buffer;
    int dataToRead;
    QString messageHeader;
};
#endif // MAINWINDOW_H
