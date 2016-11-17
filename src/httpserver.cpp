#include "httpserver.h"
#include <qsettings.h>
#include <QDebug>
#include <QCoreApplication>
#include <QDateTime>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTcpSocket>
#include <QEventLoop>

HttpServer::HttpServer(QTcpServer *parent) : QTcpServer(parent)
{
    getParameters();
}

HttpServer::HttpServer(qint16 port, QString url, QTcpServer *parent) : QTcpServer(parent)
{
    setParameters(port, url);
}

HttpServer::~HttpServer()
{
    delete dm;
}

void HttpServer::setParameters(qint16 portName, QString urlString)
{
    port = portName;
    url = urlString;
}

void HttpServer::start()
{
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString message;
    if(listen(QHostAddress::Any, port))
    {
        QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        message = "Server started";
        qDebug() << tr(" [ ][HTTPServer::101][%1] %2, port %3").arg(date, message,QString::number(port));
        message = "Listening...";
        qDebug() << tr(" [ ][HTTPServer::102][%1] %2, port %3").arg(date, message,QString::number(port));

        connect(this, SIGNAL(newConnection()), this, SLOT(onConnection()));
        dm = new DataManager();
        start_timer();

        /*dm = new DataManager();
        connect(dm, SIGNAL(sentUrl(QString)), this, SLOT(get(QString)));
        dm->setData(get(url));*/
    }
    else
    {
        message = "Server error! Unable start the server! " + server.errorString();
        qDebug() << tr(" [!][HTTPServer::103][%1] %2").arg(date, message);
        server.close();
        return;
    }
}

void HttpServer::start_timer()
{    
    connect(dm, SIGNAL(sentUrl(QString)), this, SLOT(get(QString)));
    dm->setData(get(url));
}

void HttpServer::getParameters()
{
    QString path = QCoreApplication::applicationDirPath() + "/config.cfg";
    QSettings set (path, QSettings::IniFormat);
    set.beginGroup("CONNECTION");
    qint16 port = set.value("port", "8180").toInt();
    QString url = set.value("url", "http://pvo.ascae.ru/api/schedule?access-token=api_token").toString();
    set.endGroup();
    setParameters(port, url);
}

void HttpServer::onConnection()
{
    QTcpSocket *socket = this->nextPendingConnection();

    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));

    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString message = "Have a new connection!";
    qDebug() << tr(" [ ][HTTPServer::108][%1] %2").arg(date, message);
}

QJsonDocument HttpServer::get(QString url)
{
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QNetworkAccessManager * manager = new QNetworkAccessManager();
    QUrl requestUrl(url);
    QNetworkRequest request(requestUrl);
    QNetworkReply *reply = manager->get(request);

    QEventLoop   eLoop;
    QJsonDocument parser;

    connect(manager, SIGNAL(finished(QNetworkReply*)), &eLoop, SLOT(quit()));

    if(eLoop.exec())
    {
        if(reply->error() != QNetworkReply::NoError)
        {
            qDebug() << tr(" [!][HTTPServer::104][%1] %2").arg(date, reply->errorString());
        }
    }
    else
    {
        int res = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString message;
        switch(res)
        {
            case 200:
            {
                message = "Get request, status = 200";
                qDebug() << tr(" [ ][HTTPServer::105][%1] %2").arg(date, message);
                QString json = reply->readAll();
                parser = QJsonDocument::fromJson(json.toUtf8());
                break;
            }
            default:
            {
                message = "Error " + QString::number(res) + "!";
                qDebug() << tr(" [!][HTTPServer::106][%1] %2").arg(date, message);
                break;
            }
        }
    }
    delete reply;
    delete manager;
    return parser;
}

void HttpServer::restart()
{
    disconnect(dm, SIGNAL(sentUrl(QString)), this, SLOT(get(QString)));
    dm->clear();
    start_timer();
}

void HttpServer::onReadyRead()
{
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QTcpSocket *socket = qobject_cast <QTcpSocket *> (sender());

    QString message = "Restarting timer...";
    qDebug() << tr(" [ ][HTTPServer::107][%1] %2").arg(date, message);

    QString str = "HTTP/1.1 200 OK\r\n\r\n%1";
    socket->write(str.arg(date).toUtf8());
    socket->disconnectFromHost();
    onDisconnect();

    restart();
}

void HttpServer::onDisconnect()
{
    QTcpSocket *socket = qobject_cast <QTcpSocket *> (sender());   
    socket->close();
    socket->deleteLater();
}
