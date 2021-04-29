#include "bpmdetector.h"

// system includes
#include <algorithm>

// Qt includes
#include <QDebug>

BpmDetector::BpmDetector(QObject *parent) :
    QIODevice{parent}
{
    setOpenMode(QIODevice::WriteOnly);
}

qint64 BpmDetector::readData(char *data, qint64 maxlen)
{
    qCritical("read not supported");
    return -1;
}

qint64 BpmDetector::writeData(const char *data, qint64 len)
{
    QVector<frame_t> frames{int(len / sizeof(frame_t))};
    frames.resize(len / sizeof(frame_t));

    const auto begin = reinterpret_cast<const frame_t *>(data);
    std::copy(begin, begin + frames.size(), std::begin(frames));

    emit receivedFrames(frames);

    return len;
}
