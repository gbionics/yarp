/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef VIDEOPRODUCER_H
#define VIDEOPRODUCER_H

#include <QObject>
#include <QMutex>
#include <QPointer>
#include <QVideoFrame>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QVideoSink>
#else
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#endif

/*! Presents incoming YARP frames to the QML VideoOutput. */
class VideoProducer : public QObject
{
    Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    Q_PROPERTY(QVideoSink *videoSink READ videoSink WRITE setVideoSink)
#else
    Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface WRITE setVideoSurface)
#endif
    Q_PROPERTY(int frameWidth READ getWidth NOTIFY resizeWindowRequest)
    Q_PROPERTY(int frameHeight READ getHeight NOTIFY resizeWindowRequest)

public:
    VideoProducer(QObject *parent = nullptr);
    ~VideoProducer() override;
    QString getPixelAsStr(int x, int y);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QVideoSink *videoSink() const;
    void setVideoSink(QVideoSink *sink);
#else
    QAbstractVideoSurface *videoSurface() const;
    void setVideoSurface(QAbstractVideoSurface *surface);
#endif

    int getWidth();
    int getHeight();

private:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QPointer<QVideoSink> m_sink;
#else
    QPointer<QAbstractVideoSurface> m_surface;
    QVideoSurfaceFormat m_format;
#endif
    QVideoFrame m_frame;
    QMutex mutex;

signals:
    void resizeWindowRequest();

public slots:
    void onNewVideoContentReceived(QVideoFrame *frame);
};

#endif // VIDEOPRODUCER_H
