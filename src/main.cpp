#include <QCoreApplication>
#include "httpserver.h"
#include "httpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    HttpServer *server = new HttpServer();
    server->start();
    return a.exec();
}
