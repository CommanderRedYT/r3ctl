#pragma once

// Qt includes
#include <QIODevice>
#include <QVector>

// local includes
#include "audioformat.h"

class BpmDetector : public QIODevice
{
    Q_OBJECT

public:
    explicit BpmDetector(QObject *parent = nullptr);

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

signals:
    void receivedFrames(const QVector<frame_t> &frames);
};
