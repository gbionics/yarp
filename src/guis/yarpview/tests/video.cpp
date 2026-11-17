/*
 * SPDX-FileCopyrightText: 2026 Generative Bionics S.R.L.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ImagePort.h"
#include "signalhandler.h"
#include "videoproducer.h"
#include "videoframeutils.h"

#include <yarp/os/Time.h>

#include <QCoreApplication>
#include <QDebug>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
class TestSurface : public QAbstractVideoSurface
{
public:
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType) const override
    {
        return {QVideoFrame::Format_RGB32};
    }
    bool present(const QVideoFrame &frame) override
    {
        lastFrame = frame;
        return true;
    }
    QVideoFrame lastFrame;
};
#endif

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    yarp::os::Time::useSystemClock();
    VideoProducer producer;
    SignalHandler handler;
    handler.synchDisplayPeriod(true);
    QObject::connect(&handler, &SignalHandler::sendFrame,
                     &producer, &VideoProducer::onNewVideoContentReceived);
    InputCallback callback;
    callback.setSignalHandler(&handler);
    int resizes = 0;
    QObject::connect(&producer, &VideoProducer::resizeWindowRequest, [&] { ++resizes; });
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QVideoSink sink;
    producer.setVideoSink(&sink);
#else
    TestSurface surface;
    producer.setVideoSurface(&surface);
#endif

#ifdef YARP_LITTLE_ENDIAN
    yarp::sig::ImageOf<yarp::sig::PixelBgra> image;
#else
    yarp::sig::ImageOf<yarp::sig::PixelRgba> image;
#endif
    // Odd widths exercise the differing YARP and Qt scan-line alignment.
    for (int width : {3, 5}) {
        image.resize(width, 2);
        for (int y = 0; y < image.height(); ++y) {
            for (int x = 0; x < image.width(); ++x) {
                auto &pixel = image.pixel(x, y);
                pixel.r = 17 + x;
                pixel.g = 42 + y;
                pixel.b = 123;
                pixel.a = 255;
            }
        }
        callback.onRead(image);
        QCoreApplication::processEvents();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        const QImage displayed = yarpVideoFrameToImage(sink.videoFrame());
#else
        const QImage displayed = yarpVideoFrameToImage(surface.lastFrame);
#endif
        if (producer.getWidth() != width || producer.getHeight() != 2 ||
            displayed.size() != QSize(width, 2) || displayed.pixelColor(width - 1, 1) != QColor(17 + width - 1, 43, 123) ||
            producer.getPixelAsStr(width - 1, 1) != QString("#") + QString::number(qRgb(17 + width - 1, 43, 123), 16)) {
            qCritical() << "Frame dimensions, row stride, colors, or color picking failed";
            return 1;
        }
        // A second color pick also verifies that the frame was unmapped correctly.
        if (producer.getPixelAsStr(0, 0) != "#ff112a7b" || producer.getPixelAsStr(-1, 0) != "Invalid") {
            return 1;
        }
    }
    if (resizes != 2) {
        qCritical() << "Missing resize notification";
        return 1;
    }
    return 0;
}
