#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serverPort = -1;
    dataToRead = -1;

    ui->textInfo->setReadOnly(true);
}

MainWindow::~MainWindow()
{
    clearConnections();
    delete ui;
    form.close();
}

void MainWindow::on_buttonStart_clicked()
{
    QString newPort = ui->textPort->toPlainText();
    if (!checkServerCredentials(newPort)) {
        QMessageBox::warning(this, "Error", "Wrong server credentials!\r\nPlease enter correct data.");
        return;
    }
    if (serverPort == newPort.toInt()) {
        QMessageBox::information(this, "Information",
            QString("Already listening on port %1").arg(serverPort));
        return;
    }
    if (serverPort > 0) {
        clearConnections();
    }
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    serverPort = newPort.toInt();
    if (!server->listen(QHostAddress::Any, serverPort)) {
        QMessageBox::warning(this, "Error",
            QString("Cannot start listening to port %1").arg(serverPort));
    } else {
        writeLog("Started server.", NULL);
    }
    ui->textInfo->append(QString("Running on " + getHostIp() + ":%1").arg(serverPort));
}

bool MainWindow::checkServerCredentials(QString port)
{
    bool parsed;
    int intPort = port.toInt(&parsed);
    if (!parsed || intPort < 1 || intPort > 65535) {
        return false;
    }
    return true;
}

void MainWindow::newConnection()
{
    QTcpSocket* client = server->nextPendingConnection();
    clientSockets.append(client);
    connect(client, &QTcpSocket::readyRead, this, &MainWindow::readClient);
    connect(client, &QTcpSocket::disconnected, this, &MainWindow::disconnectClient);
    writeLog("Client connected", client);
    client->write("Hello, client!");
}

void MainWindow::readClient()
{
    QMutex mutex;

    QTcpSocket* client = (QTcpSocket*)sender();
    QByteArray reading = client->readAll();

    mutex.lock();
    if (dataToRead < 0) {
        messageHeader = reading.mid(0, 128);
        dataToRead = messageHeader.split(",").takeAt(1).split(":").last().toInt();
        buffer.append(reading.mid(128));
    } else {
        buffer.append(reading);
    }
    if (dataToRead > 0 && buffer.size() < dataToRead) {
        ui->textInfo->append("Waiting for full message from client...");
        reading.clear();
        return;
    }

    ui->textInfo->append("From " + client->peerAddress().toString().remove(0, 7)
        + ":" + QString::number(client->peerPort()) + ":");
    QString messageType = messageHeader.split(",").first().split(":").last();

    if (messageType == "message") {
        ui->textInfo->append(buffer);
        writeLog(buffer, client);
    } else {
        ui->textInfo->append("Got new image.");
        QPixmap map;

        QStringList list;
        list << "png"
             << "bmp"
             << "jpg"
             << "jpeg";
        QString fileEx = messageHeader.split(",").last().split(":").last().split(".").last();
        switch (list.indexOf(fileEx)) {
        case 0:
            map.loadFromData(buffer, "PNG");
            break;
        case 1:
            map.loadFromData(buffer, "BMP");
            break;
        case 2:
            map.loadFromData(buffer, "JPG");
            break;
        case 3:
            map.loadFromData(buffer, "JPEG");
            break;
        }

        form.setImage(map);
        form.show();

        writeLog("Got new image.", client);
    }
    buffer.clear();
    dataToRead = -1;
    mutex.unlock();
}

void MainWindow::disconnectClient()
{
    QTcpSocket* client = (QTcpSocket*)sender();
    clientSockets.removeOne(client);
    disconnect(client, &QTcpSocket::readyRead, this, &MainWindow::readClient);
    disconnect(client, &QTcpSocket::disconnected, this, &MainWindow::disconnectClient);
    writeLog("Client disconnected", client);
}

void MainWindow::on_buttonSend_clicked()
{
    if (ui->textMessage->toPlainText() == "") {
        return;
    }
    foreach (QTcpSocket* socket, clientSockets) {
        socket->write(ui->textMessage->toPlainText().toUtf8());
    }
    ui->textMessage->setText("");
}

void MainWindow::writeLog(QString text, QTcpSocket* socket)
{
    QFile file("logfile.txt");
    if (file.open(QIODevice::Append)) {
        QString address;
        if (socket == NULL) {
            address = QString(getHostIp() + ":%1").arg(serverPort);
        } else {
            address = socket->peerAddress().toString().remove(0, 7) + ":" + QString::number(socket->peerPort());
        }
        QTextStream stream(&file);
        stream << QDate::currentDate().toString(Qt::SystemLocaleShortDate)
               << " " << QTime::currentTime().toString()
               << " | " << address
               << " | " << text << endl;
    } else {
        QMessageBox::warning(this, "Error", "Cannot write to log file");
    }
    file.close();
}

QString MainWindow::getHostIp()
{
    return QHostInfo::fromName(QHostInfo::localHostName()).addresses().first().toString();
}

void MainWindow::clearConnections()
{
    foreach (QTcpSocket* socket, clientSockets) {
        disconnect(socket, &QTcpSocket::readyRead, this, &MainWindow::readClient);
        disconnect(socket, &QTcpSocket::disconnected, this, &MainWindow::disconnectClient);
    }
    clientSockets.clear();
    disconnect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    server->close();
    delete server;
    writeLog("Server stopped.", NULL);
}
