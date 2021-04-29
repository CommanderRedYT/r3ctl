#pragma once

#include <QAudioFormat>

namespace {
constexpr int frameRate = 44100;
constexpr int channelCount = 2;
constexpr int sampleSize = 32;
constexpr auto codec = "audio/pcm";
constexpr QAudioFormat::Endian byteOrder = QAudioFormat::LittleEndian;
constexpr QAudioFormat::SampleType sampleType = QAudioFormat::Float;
using sample_t = float;
using frame_t = std::array<sample_t, channelCount>;

auto makeFormat()
{
    QAudioFormat format;
    format.setSampleRate(frameRate);
    format.setChannelCount(channelCount);
    format.setSampleSize(sampleSize);
    format.setCodec(codec);
    format.setByteOrder(byteOrder);
    format.setSampleType(sampleType);
    return format;
}
}
