#pragma once

#include <QObject>
#include <QWebSocket>

class R3Client : public QObject
{
    Q_OBJECT

public:
    R3Client(QObject *parent = nullptr);

    void open();
    void close(QWebSocketProtocol::CloseCode closeCode = QWebSocketProtocol::CloseCodeNormal, const QString &reason = QString{});

    void sendMQTT(const QString &ctx, const QJsonValue &data);
    void sendYmhButton(const QString &btn);
    static QString mqtttopic_golightctrl(const QString &lightname);

signals:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError error);
    void statusReceived(const QString &ctx, const QJsonValue &jsonValue);

private slots:
    void textMessageReceived(const QString &message);
    void binaryMessageReceived(const QByteArray &message);

private:
    QWebSocket m_webSocket;
};
