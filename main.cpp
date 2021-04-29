#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>

#include "r3client.h"
#include "mainwindow.h"

#include <memory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return a.exec();

    const auto arguments = [&](){
        auto arguments = a.arguments();
        arguments.removeFirst();
        return arguments;
    }();

    if (arguments.isEmpty())
    {
        puts("No cmd given!");
        return -1;
    }

    R3Client client;

    QObject::connect(&client, &R3Client::disconnected, [](){ qDebug() << "disconnected()"; });
    QObject::connect(&client, &R3Client::error, [](QAbstractSocket::SocketError error){ qDebug() << "error()" << error; });

    if (arguments.first() == "log")
    {
        QObject::connect(&client, &R3Client::connected, [](){ qDebug() << "connected()"; });
        QObject::connect(&client, &R3Client::statusReceived, [](const QString &ctx, const QJsonValue &jsonValue){ qDebug() << "statusReceived()" << ctx << jsonValue; });
    }
    else if (arguments.first() == "script")
    {
        QObject::connect(&client, &R3Client::connected, [&](){
            client.sendMQTT("action/ceilingscripts/activatescript", QJsonObject{
                {"colourlist", QJsonArray{
                     QJsonObject{{"b", 0},   {"cw", 0}, {"g", 0},   {"r", 1000}, {"ww", 0}},
                     QJsonObject{{"b", 100}, {"cw", 0}, {"g", 0},   {"r", 800},  {"ww", 0}},
                     QJsonObject{{"b", 300}, {"cw", 0}, {"g", 0},   {"r", 0},    {"ww", 0}},
                     QJsonObject{{"b", 100}, {"cw", 0}, {"g", 500}, {"r", 0},    {"ww", 0}},
                     QJsonObject{{"b", 0},   {"cw", 0}, {"g", 800}, {"r", 0},    {"ww", 0}},
                     QJsonObject{{"b", 0},   {"cw", 0}, {"g", 200}, {"r", 800},  {"ww", 0}}
                 }},
                {"fadeduration", 500},
                {"script", "wave"}
            });
        });
    }
    else if (arguments.first() == "sendYmhButton")
    {
        if (arguments.size() < 2)
        {
            puts("No button given for sendYmhButton");
            return -3;
        }

        const auto btn = arguments.at(1);

        QObject::connect(&client, &R3Client::connected, [&client,btn](){
            client.sendYmhButton(btn);
        });
        QObject::connect(&client, &R3Client::statusReceived, [btn](const QString &ctx, const QJsonValue &jsonValue){
            if (ctx != "action/yamahastereo/ircmd")
                return;
            if (!jsonValue.isObject())
            {
                qWarning() << "json data is not an object";
                qWarning() << jsonValue;
                return;
            }
            const auto object = jsonValue.toObject();
            if (!object.contains("Cmd"))
            {
                qWarning() << "json data does not contain Cmd";
                qWarning() << object;
                return;
            }
            const auto cmdValue = object.value("Cmd");
            if (!cmdValue.isString())
            {
                qWarning() << "json data Cmd is not a string";
                qWarning() << object;
                return;
            }
            const auto cmd = cmdValue.toString();
            if (cmd == btn)
                QCoreApplication::quit();
        });
    }
    else
    {
        puts("Invalid cmd. Valid are log, script, sendYmhButton");
        return -2;
    }

    client.open();

    return a.exec();
}
