#pragma once

// system includes
#include <memory>
#include <optional>

// Qt includes
#include <QMainWindow>
#include <QAbstractSocket>
#include <QDateTime>
#include <QTimer>
#include <QList>
#include <QAudioDeviceInfo>

// local includes
#include "ui_mainwindow.h"
#include "r3client.h"
#include "bpmdetector.h"
#include "audioformat.h"

class QAudioInput;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void connectPressed();
    void disconnectPressed();

    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError error);
    void statusReceived(const QString &ctx, const QJsonValue &jsonValue);

    void toggleAllPressed();
    void discoLeftPressed();
    void discoRightPressed();

    void bpmPressed();

    void openAudioDevicePressed();
    void closeAudioDevicePressed();

    void bpmTimeout();

    void receivedFrames(const QVector<frame_t> &frames);

private:
    void log(const QString &msg);
    void lightCmd(const QString &light, const QString &status);
    void updateDisco();
    void updateAudioDevices();

private:
    Ui::MainWindow m_ui;
    R3Client m_client;

    bool m_toggleState{};
    uint8_t m_discoState{};

    struct BpmTap { QDateTime begin{QDateTime::currentDateTime()}; int count{}; int msPerBeat, bpm; bool flag{}; };
    std::optional<BpmTap> m_bpmTap;

    QTimer m_timer;

    QList<QAudioDeviceInfo> m_devices;
    std::unique_ptr<QAudioInput> m_input;
    BpmDetector m_detector;

    sample_t m_min{}, m_max{};

    double m_distanceSum{};
    std::size_t m_count{};

    QTimer m_timer2;
};
