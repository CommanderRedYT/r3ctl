#include <QCoreApplication>
#include <QWebSocket>
#include <QDebug>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QTimer>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QWebSocket webSocket;
    QObject::connect(&webSocket, &QWebSocket::connected, [&](){ qDebug() << "connected()"; });
    QObject::connect(&webSocket, &QWebSocket::disconnected, [&](){ qDebug() << "disconnected()"; });
//    QObject::connect(&webSocket, &QWebSocket::textFrameReceived, [&](const QString &frame, bool isLastFrame){
//        qDebug() << "textFrameReceived()";
//    });
//    QObject::connect(&webSocket, &QWebSocket::binaryFrameReceived, [&](const QByteArray &frame, bool isLastFrame){
//        qDebug() << "binaryFrameReceived()";
//    });
    QObject::connect(&webSocket, &QWebSocket::textMessageReceived, [&](const QString &message){
        qDebug() << "textMessageReceived()" << message;
    });
    QObject::connect(&webSocket, &QWebSocket::binaryMessageReceived, [&](const QByteArray &message){
        qDebug() << "binaryMessageReceived()" << message;
    });
    QObject::connect(&webSocket, qOverload<QAbstractSocket::SocketError>(&QWebSocket::error), [&](QAbstractSocket::SocketError error){
        qDebug() << "error()" << error;
    });
    webSocket.open(QUrl{"ws://licht.realraum.at/sock"});

    const auto sendMQTT = [&](const QString &ctx, const QJsonValue &data){
        const QJsonObject m{{"ctx", ctx}, {"data", data}};
        const auto text = QJsonDocument{m}.toJson();
        qDebug() << "sendTextMessage()" << text;
        webSocket.sendTextMessage(text);
    };

    const auto mqtttopic_golightctrl = [](const QString &lightname){
        return QString{"action/GoLightCtrl/%0"}.arg(lightname);
    };

    const auto sendYmhButton = [&](const QString &btn){
        sendMQTT(mqtttopic_golightctrl(btn), QJsonObject{{"Action", "send"}});
    };

    QTimer::singleShot(1000, [&](){
        sendYmhButton("ymhvolup");
    });

    return a.exec();
}
