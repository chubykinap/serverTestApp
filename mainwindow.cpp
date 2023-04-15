#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->textInfo->setReadOnly(true);
}

MainWindow::~MainWindow()
{
    clearConnections();
    serverPort = -1;
    delete ui;
}


void MainWindow::on_buttonStart_clicked()
{
    int newPort = ui->textPort->toPlainText().toInt();
    if (serverPort == newPort){
        QMessageBox::information(this, "Information",
                             QString("Already listening on port %1").arg(serverPort));
        return;
    }
    if (serverPort > 0){
        clearConnections();
    }
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    serverPort = newPort;
    if (!server->listen(QHostAddress::Any, serverPort)){
        QMessageBox::warning(this, "Error",
                             QString("Cannot start listening to port %1").arg(serverPort));
    } else{
        writeLog("Started server.", NULL);
    }
    ui->textInfo->append(QString("Running on " + getHostIp() + ":%1").arg(serverPort));
}

void MainWindow::newConnection(){
    QTcpSocket* client = server->nextPendingConnection();
    clientSockets.append(client);
    connect(client, &QTcpSocket::readyRead, this, &MainWindow::readClient);
    connect(client, &QTcpSocket::disconnected, this, &MainWindow::disconnectClient);
    writeLog("Client connected", client);
    client->write("Hello, client!");
}

void MainWindow::readClient(){
    QTcpSocket* client = (QTcpSocket*) sender();
    QString info = "From " + client->peerAddress().toString().remove(0,7) +
            ":" + QString::number(client->peerPort()) + ":";
    ui->textInfo->append(info);
    QByteArray text = client->readAll();
    writeLog(text, client);
    ui->textInfo->append(text);
}

void MainWindow::disconnectClient(){
    QTcpSocket* client = (QTcpSocket*) sender();
    clientSockets.removeOne(client);
    disconnect(client, &QTcpSocket::readyRead, this, &MainWindow::readClient);
    disconnect(client, &QTcpSocket::disconnected, this, &MainWindow::disconnectClient);
    writeLog("Client disconnected", client);
}

void MainWindow::on_buttonSend_clicked()
{
    foreach (QTcpSocket* socket, clientSockets) {
        socket->write(ui->textMessage->toPlainText().toUtf8());
    }
    ui->textMessage->setText("");
}

void MainWindow::writeLog(QString text, QTcpSocket* socket){
    QFile file("logfile.txt");
    if (file.open(QIODevice::Append)){
        QString address;
        if (socket == NULL){
            address = QString(getHostIp() + ":%1").arg(serverPort);
        } else {
            address = socket->peerAddress().toString() +
                    ":" + QString::number(socket->peerPort());
        }
        QTextStream stream(&file);
        stream << QDate::currentDate().toString(Qt::SystemLocaleShortDate)
               << " " << QTime::currentTime().toString()
               << " | " << address
               << " | " << text << endl;
    } else {
        QMessageBox::warning(this, "Error", "Cannot write to log file");
    }
}

QString MainWindow::getHostIp(){
    QHostInfo host = QHostInfo::fromName(QHostInfo::localHostName());

    return host.addresses().first().toString();
}

void MainWindow::clearConnections(){
    foreach (QTcpSocket* socket,  clientSockets) {
        disconnect(socket, &QTcpSocket::readyRead, this, &MainWindow::readClient);
        disconnect(socket, &QTcpSocket::disconnected, this, &MainWindow::disconnectClient);
    }
    clientSockets.clear();
    disconnect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    server->close();
    server = NULL;
    writeLog("Server stopped.", NULL);
}
