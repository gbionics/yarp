/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "videoproducer.h"
#include "videoframeutils.h"
#include <QMutexLocker>

VideoProducer::VideoProducer(QObject *parent) : QObject(parent)
{
}

VideoProducer::~VideoProducer() = default;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
QVideoSink *VideoProducer::videoSink() const
{
    return m_sink;
}

void VideoProducer::setVideoSink(QVideoSink *sink)
{
    m_sink = sink;
}
#else
QAbstractVideoSurface *VideoProducer::videoSurface() const
{
    return m_surface;
}

void VideoProducer::setVideoSurface(QAbstractVideoSurface *surface)
{
    if (m_surface != surface && m_surface && m_surface->isActive()) {
        m_surface->stop();
    }
    m_surface = surface;
}
#endif

int VideoProducer::getWidth()
{
    QMutexLocker lock(&mutex);
    return m_frame.isValid() ? m_frame.width() : 0;
}

int VideoProducer::getHeight()
{
    QMutexLocker lock(&mutex);
    return m_frame.isValid() ? m_frame.height() : 0;
}

void VideoProducer::onNewVideoContentReceived(QVideoFrame *frame)
{
    if (!frame || !frame->isValid()) {
        return;
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (!m_sink) {
        return;
    }
    m_sink->setVideoFrame(*frame);
#else
    if (!m_surface) {
        return;
    }
    if (m_surface->isActive() &&
        (m_format.frameSize() != frame->size() || m_format.pixelFormat() != frame->pixelFormat())) {
        m_surface->stop();
    }
    if (!m_surface->isActive()) {
        m_format = QVideoSurfaceFormat(frame->size(), frame->pixelFormat());
        if (!m_surface->start(m_format)) {
            qWarning("Surface START ERROR");
            return;
        }
    }
    if (!m_surface->present(*frame)) {
        qWarning("Surface PRESENT ERROR");
        return;
    }
#endif
    bool resized;
    {
        QMutexLocker lock(&mutex);
        resized = m_frame.size() != frame->size();
        m_frame = *frame;
    }
    if (resized) {
        emit resizeWindowRequest();
    }
}

QString VideoProducer::getPixelAsStr(int x, int y)
{
    QMutexLocker lock(&mutex);
    if (!m_frame.isValid() || x < 0 || y < 0 || x >= m_frame.width() || y >= m_frame.height()) {
        return QStringLiteral("Invalid");
    }
    QImage image = yarpVideoFrameToImage(m_frame);
    if (image.isNull()) {
        return QStringLiteral("Invalid");
    }
    return QStringLiteral("#") + QString::number(image.pixel(x, y), 16);
}
