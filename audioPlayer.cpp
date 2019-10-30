/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "audioPlayer.h"

#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QDebug>
#include <QVBoxLayout>
#include <qmath.h>
#include <qendian.h>
#include <QBuffer>
AudioPlayer::AudioPlayer()
    : m_pushTimer(new QTimer(this))
{
//    initializeAudio(QAudioDeviceInfo::defaultOutputDevice());
}

AudioPlayer::~AudioPlayer()
{
    m_pushTimer->stop();
}
void AudioPlayer::initializeAudio(QAudioFormat format)
{
    QAudioDeviceInfo deviceInfo = QAudioDeviceInfo::defaultOutputDevice();
    m_format = format;

    if (!deviceInfo.isFormatSupported(format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        format = deviceInfo.nearestFormat(format);
    }

    m_audioOutput.reset(new QAudioOutput(deviceInfo, format));
}

void AudioPlayer::Play(QList<float> sound){

    m_pushTimer->stop();
    m_audioOutput->stop();

   const int channelBytes = m_format.sampleSize() / 8;
   const int sampleBytes = m_format.channelCount() * channelBytes;
   int length = sound.length() * sampleBytes;


   m_buffer.resize(length);
   unsigned char *ptr = reinterpret_cast<unsigned char *>(m_buffer.data());

   for (int pos = 0; pos < sound.length(); pos++) {
       // Produces value (-1..1)

       const qreal x = sound[pos];
        //        const qreal x = qSin(2 * M_PI * 450.0 / 48000.0);
       //qDebug()<< "Real : " << x << " sound : " << sound[pos];
       for (int i=0; i<m_format.channelCount(); ++i) {
           if (m_format.sampleSize() == 8) {
               if (m_format.sampleType() == QAudioFormat::UnSignedInt) {
                   const quint8 value = static_cast<quint8>((1.0 + x) / 2 * 255);
                   *reinterpret_cast<quint8 *>(ptr) = value;
               } else if (m_format.sampleType() == QAudioFormat::SignedInt) {
                   const qint8 value = static_cast<qint8>(x * 127);
                   *reinterpret_cast<qint8 *>(ptr) = value;
               }
           } else if (m_format.sampleSize() == 16) {
               if (m_format.sampleType() == QAudioFormat::UnSignedInt) {
                   quint16 value = static_cast<quint16>((1.0 + x) / 2 * 65535);
                   if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                       qToLittleEndian<quint16>(value, ptr);
                   else
                       qToBigEndian<quint16>(value, ptr);
               } else if (m_format.sampleType() == QAudioFormat::SignedInt) {
                   qint16 value = static_cast<qint16>(x * 32767);
                   if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                       qToLittleEndian<qint16>(value, ptr);
                   else
                       qToBigEndian<qint16>(value, ptr);
               }
           }
           ptr += channelBytes;
       }
    }
    buff.setBuffer(&m_buffer);

    buff.open(QIODevice::ReadOnly);
    qDebug() << "Filled buffer";
    buff.seek(0); // at start
    m_audioOutput->start(&buff);
    qDebug() << "We are ready";
}


void AudioPlayer::volumeChanged(int value)
{
    qreal linearVolume = QAudio::convertVolume(value / qreal(100),
                                               QAudio::LogarithmicVolumeScale,
                                               QAudio::LinearVolumeScale);

    m_audioOutput->setVolume(linearVolume);
}
