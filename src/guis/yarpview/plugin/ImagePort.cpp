/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

// ImagePort.cpp: implementation of the ImagePort class.
//
//////////////////////////////////////////////////////////////////////

#include "ImagePort.h"
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QVideoFrameFormat>
#endif
#include <yarp/sig/ImageFile.h>

using namespace yarp::os;

#include <cstring>

InputCallback::InputCallback()
{
    sigHandler = nullptr;
    counter = 0;
}

InputCallback::~InputCallback() = default;

/*! \brief the function callback
    \param img the image received
*/
#ifdef YARP_LITTLE_ENDIAN
void InputCallback::onRead(yarp::sig::ImageOf<yarp::sig::PixelBgra> &img)
#else
void InputCallback::onRead(yarp::sig::ImageOf<yarp::sig::PixelRgba> &img)
#endif
{

    const QSize size(img.width(), img.height());
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#ifdef YARP_LITTLE_ENDIAN
    const auto format = QVideoFrameFormat::Format_BGRX8888;
#else
    const auto format = QVideoFrameFormat::Format_RGBX8888;
#endif
    QVideoFrame frame(QVideoFrameFormat(size, format));
    if (!frame.map(QVideoFrame::WriteOnly)) {
        return;
    }
    auto *destination = frame.bits(0);
    const int stride = frame.bytesPerLine(0);
#else
    QVideoFrame frame(img.getRawImageSize(), size, img.getRowSize(), QVideoFrame::Format_RGB32);
    if (!frame.map(QAbstractVideoBuffer::WriteOnly)) {
        return;
    }
    auto *destination = frame.bits();
    const int stride = frame.bytesPerLine();
#endif
    // Qt and YARP may pad rows differently, particularly at odd image widths.
    for (std::size_t row = 0; row < img.height(); ++row) {
        memcpy(destination + row * stride, img.getRawImage() + row * img.getRowSize(),
               img.width() * 4);
    }
    frame.unmap();
    if(sigHandler){
        sigHandler->sendVideoFrame(frame);
    }

}

/*! \brief sets the signalhandler to the class
    \param handler the signal handler
*/
void InputCallback::setSignalHandler(SignalHandler *handler)
{
    sigHandler = handler;
}
