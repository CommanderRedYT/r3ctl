#include "mainwindow.h"

// system includes
#include <algorithm>

// Qt includes
#include <QMetaEnum>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QAudioInput>
#include <QJsonDocument>

namespace {
template<typename QEnum>
QString enumToString(const QEnum value)
{
  return QMetaEnum::fromType<QEnum>().valueToKey(value);
}
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{parent}
{
    m_ui.setupUi(this);

    connect(m_ui.connectButton, &QAbstractButton::pressed, this, &MainWindow::connectPressed);
    connect(m_ui.disconnectButton, &QAbstractButton::pressed, this, &MainWindow::disconnectPressed);

    connect(m_ui.lamp1OnButton, &QAbstractButton::pressed, this, [&](){ lightCmd("basiclight1", "on"); });
    connect(m_ui.lamp1OffButton, &QAbstractButton::pressed, this, [&](){ lightCmd("basiclight1", "off"); });
    connect(m_ui.lamp2OnButton, &QAbstractButton::pressed, this, [&](){ lightCmd("basiclight2", "on"); });
    connect(m_ui.lamp2OffButton, &QAbstractButton::pressed, this, [&](){ lightCmd("basiclight2", "off"); });
    connect(m_ui.lamp3OnButton, &QAbstractButton::pressed, this, [&](){ lightCmd("basiclight3", "on"); });
    connect(m_ui.lamp3OffButton, &QAbstractButton::pressed, this, [&](){ lightCmd("basiclight3", "off"); });
    connect(m_ui.lamp4OnButton, &QAbstractButton::pressed, this, [&](){ lightCmd("basiclight4", "on"); });
    connect(m_ui.lamp4OffButton, &QAbstractButton::pressed, this, [&](){ lightCmd("basiclight4", "off"); });
    connect(m_ui.lamp5OnButton, &QAbstractButton::pressed, this, [&](){ lightCmd("basiclight5", "on"); });
    connect(m_ui.lamp5OffButton, &QAbstractButton::pressed, this, [&](){ lightCmd("basiclight5", "off"); });
    connect(m_ui.lamp6OnButton, &QAbstractButton::pressed, this, [&](){ lightCmd("basiclight6", "on"); });
    connect(m_ui.lamp6OffButton, &QAbstractButton::pressed, this, [&](){ lightCmd("basiclight6", "off"); });

    connect(m_ui.toggleAllButton, &QAbstractButton::pressed, this, &MainWindow::toggleAllPressed);
    connect(m_ui.discoLeftButton, &QAbstractButton::pressed, this, &MainWindow::discoLeftPressed);
    connect(m_ui.discoRightButton, &QAbstractButton::pressed, this, &MainWindow::discoRightPressed);

    connect(m_ui.bpmButton, &QAbstractButton::pressed, this, &MainWindow::bpmPressed);

    connect(m_ui.openAudioDeviceButton, &QAbstractButton::pressed, this, &MainWindow::openAudioDevicePressed);
    connect(m_ui.closeAudioDeviceButton, &QAbstractButton::pressed, this, &MainWindow::closeAudioDevicePressed);

    connect(&m_client, &R3Client::connected, this, &MainWindow::connected);
    connect(&m_client, &R3Client::disconnected, this, &MainWindow::disconnected);
    connect(&m_client, &R3Client::error, this, &MainWindow::error);
    connect(&m_client, &R3Client::statusReceived, this, &MainWindow::statusReceived);

    updateAudioDevices();

    m_timer.setTimerType(Qt::PreciseTimer);
    connect(&m_timer, &QTimer::timeout, this, &MainWindow::bpmTimeout);

    connect(m_ui.spinBox, qOverload<int>(&QSpinBox::valueChanged), &m_timer, qOverload<int>(&QTimer::setInterval));

    connect(&m_detector, &BpmDetector::receivedFrames, this, &MainWindow::receivedFrames);

    connect(&m_timer2, &QTimer::timeout, this, [&](){
        const sample_t min = -m_min * 100;
        m_ui.verticalSlider->setValue(min);
        m_min = 0;

        const sample_t max = m_max * 100;
        m_ui.verticalSlider_2->setValue(max);
        m_max = 0;

        const sample_t avg = m_distanceSum / m_count * 100.;
        m_ui.verticalSlider_3->setValue(avg);
        m_distanceSum = 0.;
        m_count = 0;

        const auto selectedWert = [&](){
            if (m_ui.radioButton->isChecked())
                return min;
            else if (m_ui.radioButton_2->isChecked())
                return max;
            else if (m_ui.radioButton_3->isChecked())
                return avg;
        }();

        if (selectedWert > m_ui.verticalSlider_4->value())
            switch (m_ui.comboBox->currentIndex())
            {
            case 0: discoLeftPressed(); break;
            case 1: discoRightPressed(); break;
            case 2: toggleAllPressed(); break;
            }
    });
    m_timer2.start(1000/20);
}

MainWindow::~MainWindow() = default;

void MainWindow::connectPressed()
{
    log("Connecting...");
    m_client.open();
}

void MainWindow::disconnectPressed()
{
    log("Disconnecting...");
    m_client.close();
}

void MainWindow::connected()
{
    log("Connected successfully!");
}

void MainWindow::disconnected()
{
    log("Disconnected!");
}

void MainWindow::error(QAbstractSocket::SocketError error)
{
    log(QString{"Error: %0"}.arg(enumToString(error)));
}

void MainWindow::statusReceived(const QString &ctx, const QJsonValue &jsonValue)
{
    qDebug() << jsonValue;
    log(QString{"ctx=%0"}.arg(ctx));
}

void MainWindow::toggleAllPressed()
{
    lightCmd("basiclight1", m_toggleState ? "on" : "off");
    lightCmd("basiclight2", m_toggleState ? "on" : "off");
    lightCmd("basiclight3", m_toggleState ? "on" : "off");
    lightCmd("basiclight4", m_toggleState ? "on" : "off");
    lightCmd("basiclight5", m_toggleState ? "on" : "off");
    lightCmd("basiclight6", m_toggleState ? "on" : "off");
    m_toggleState = !m_toggleState;
}

void MainWindow::discoLeftPressed()
{
    if (m_discoState == 0)
        m_discoState = 5;
    else
        m_discoState--;
    updateDisco();
}

void MainWindow::discoRightPressed()
{
    if (m_discoState == 5)
        m_discoState = 0;
    else
        m_discoState++;
    updateDisco();
}

void MainWindow::bpmPressed()
{
    discoRightPressed();

    if (m_bpmTap)
    {
        m_bpmTap->count++;
        const auto elapsed = m_bpmTap->begin.msecsTo(QDateTime::currentDateTime());
        m_bpmTap->msPerBeat = elapsed / m_bpmTap->count;
        m_bpmTap->bpm = 60000 / m_bpmTap->msPerBeat;
        m_ui.bpmButton->setText(QString{"%0 %1 BPM"}.arg(m_bpmTap->count).arg(m_bpmTap->bpm));
        m_ui.spinBox->setValue(m_bpmTap->msPerBeat);
        m_timer.start(m_bpmTap->msPerBeat);
        m_bpmTap->flag = false;
    }
    else
    {
        m_timer.start(2000);
        m_bpmTap = BpmTap{};
    }
}

void MainWindow::openAudioDevicePressed()
{
    const auto index = m_ui.audioDeviceSelection->currentIndex();
    if (index < 0 || index >= m_devices.count())
        return;

    m_input = std::make_unique<QAudioInput>(m_devices.at(index), makeFormat());
    m_input->start(&m_detector);
}

void MainWindow::closeAudioDevicePressed()
{
    m_input = nullptr;
}

void MainWindow::bpmTimeout()
{
    if (m_bpmTap)
    {
        if (!m_bpmTap->flag)
        {
            m_bpmTap->flag = true;
            return;
        }

        if (m_bpmTap->count)
        {
            qDebug() << "starting normal";
            discoRightPressed();
        }
        else
        {
            m_ui.bpmButton->setText(tr("BPM tap"));
            qDebug() << "aborted";
            m_timer.stop();
        }
        m_bpmTap = std::nullopt;
    }
    else
    {
        qDebug() << "disco timer";
        discoRightPressed();
    }
}

void MainWindow::receivedFrames(const QVector<frame_t> &frames)
{
    for (const auto &frame : frames)
    {
        if (frame[0] < m_min)
            m_min = frame[0];

        if (frame[0] > m_max)
            m_max = frame[0];

        m_distanceSum += std::abs(frame[0]);
        m_count++;
    }
}

void MainWindow::log(const QString &msg)
{
    m_ui.logView->appendPlainText(QString{"%0 %1"}.arg(QDateTime::currentDateTime().toString(), msg));
}

void MainWindow::lightCmd(const QString &light, const QString &status)
{
    m_client.sendMQTT("action/GoLightCtrl/" + light, QJsonObject{ {"Action", status }});
}

void MainWindow::updateDisco()
{
    lightCmd("basiclight1", m_discoState == 0 ? "on" : "off");
    lightCmd("basiclight2", m_discoState == 1 ? "on" : "off");
    lightCmd("basiclight3", m_discoState == 2 ? "on" : "off");
    lightCmd("basiclight4", m_discoState == 3 ? "on" : "off");
    lightCmd("basiclight5", m_discoState == 4 ? "on" : "off");
    lightCmd("basiclight6", m_discoState == 5 ? "on" : "off");
}

void MainWindow::updateAudioDevices()
{
    m_ui.audioDeviceSelection->clear();
    m_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for (const auto &x : m_devices)
        m_ui.audioDeviceSelection->addItem(x.deviceName());
}
