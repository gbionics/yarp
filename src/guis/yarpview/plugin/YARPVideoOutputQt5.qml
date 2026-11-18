/*
 * SPDX-FileCopyrightText: 2026 Generative Bionics S.R.L.
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

import QtQuick 2.15
import QtMultimedia 5.0

VideoOutput {
    property var videoProducer
    source: videoProducer
    fillMode: VideoOutput.Stretch
}
