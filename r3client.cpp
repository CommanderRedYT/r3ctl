#include "r3client.h"

#include <QJsonObject>
#include <QJsonDocument>

R3Client::R3Client(QObject *parent) :
    QObject{parent}
{
    connect(&m_webSocket, &QWebSocket::connected, this, &R3Client::connected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &R3Client::disconnected);
    connect(&m_webSocket, qOverload<QAbstractSocket::SocketError>(&QWebSocket::error), this, &R3Client::error);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &R3Client::textMessageReceived);
    connect(&m_webSocket, &QWebSocket::binaryMessageReceived, this, &R3Client::binaryMessageReceived);
}

void R3Client::open()
{
    m_webSocket.open(QUrl{"ws://licht.realraum.at/sock"});
}

void R3Client::close(QWebSocketProtocol::CloseCode closeCode, const QString &reason)
{
    m_webSocket.close(closeCode, reason);
}

void R3Client::sendMQTT(const QString &ctx, const QJsonValue &data)
{
    const QJsonObject m{{"ctx", ctx}, {"data", data}};
    //qDebug() << m;
    const auto text = QJsonDocument{m}.toJson(QJsonDocument::Compact);
    //qDebug() << "sendTextMessage()" << text;
    m_webSocket.sendTextMessage(text);
}

void R3Client::sendYmhButton(const QString &btn)
{
    sendMQTT(mqtttopic_golightctrl(btn), QJsonObject{{"Action", "send"}});
}

QString R3Client::mqtttopic_golightctrl(const QString &lightname)
{
    return QString{"action/GoLightCtrl/%0"}.arg(lightname);
}

void R3Client::textMessageReceived(const QString &message)
{
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(message.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError)
    {
        qWarning() << "could not parse JSON" << error.errorString();
        qDebug() << "textMessageReceived()" << message;
        return;
    }

    if (!document.isObject())
    {
        qWarning() << "json is not an object";
        qDebug() << "textMessageReceived()" << message;
        return;
    }

    const auto obj = document.object();

    if (!obj.contains("ctx"))
    {
        qWarning() << "json does not contain ctx";
        qDebug() << "textMessageReceived()" << message;
        return;
    }

    QString ctx;

    {
        const auto ctxValue = obj.value("ctx");
        if (!ctxValue.isString())
        {
            qWarning() << "ctx is not a string!";
            qDebug() << "textMessageReceived()" << message;
            return;
        }

        ctx = ctxValue.toString();
    }

    if (!obj.contains("data"))
    {
        qWarning() << "json does not contain data";
        qDebug() << "textMessageReceived()" << message;
        return;
    }

    const auto data = obj.value("data");

    emit statusReceived(ctx, data);
}

void R3Client::binaryMessageReceived(const QByteArray &message)
{
    qWarning() << "binaryMessageReceived()" << message;
}
