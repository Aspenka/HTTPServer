#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QNetworkAccessManager>
#include <QTcpServer>
#include <QJsonDocument>
#include "datamanager.h"

class HttpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit                HttpServer          (QTcpServer *parent = 0);
                            HttpServer          (qint16 port, QString url, QTcpServer *parent = 0);
                            ~HttpServer         ();

    void                    setParameters       (qint16 portName, QString urlString);
    void                    start               ();

private:
    DataManager  *          dm;
    QTcpServer              server;
    QString                 url;
    qint16                  port;

    void                    getParameters       ();
    void                    start_timer          ();
    bool                    post                (QJsonDocument data);
signals:

public slots:
    void                    onConnection        ();
    void                    restart             ();
    void                    onReadyRead         ();
    void                    onDisconnect        ();
    QJsonDocument           get                 (QString url);
};

#endif // HTTPSERVER_H
