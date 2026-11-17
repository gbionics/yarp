/*
 * SPDX-FileCopyrightText: 2026 Generative Bionics S.R.L.
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARPVIEW_VIDEOFRAMEUTILS_H
#define YARPVIEW_VIDEOFRAMEUTILS_H

#include <QImage>
#include <QVideoFrame>

// Return an owning image: saving and color picking must not outlive a frame map.
inline QImage yarpVideoFrameToImage(QVideoFrame frame)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return frame.toImage();
#else
    if (!frame.map(QAbstractVideoBuffer::ReadOnly)) {
        return {};
    }
    QImage image(frame.bits(), frame.width(), frame.height(),
                 frame.bytesPerLine(), QImage::Format_RGB32);
    QImage copy = image.copy();
    frame.unmap();
    return copy;
#endif
}

#endif
