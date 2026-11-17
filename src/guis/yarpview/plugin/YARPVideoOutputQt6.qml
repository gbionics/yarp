/*
 * SPDX-FileCopyrightText: 2026 Generative Bionics S.R.L.
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

import QtQuick
import QtMultimedia

VideoOutput {
    property var videoProducer
    fillMode: VideoOutput.Stretch
    Component.onCompleted: videoProducer.videoSink = videoSink
}
