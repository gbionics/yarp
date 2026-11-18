/*
 * SPDX-FileCopyrightText: 2026 Generative Bionics S.R.L.
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

import QtQuick.Dialogs

FileDialog {
    property bool selectExisting: false
    readonly property url fileUrl: selectedFile
    fileMode: selectExisting ? FileDialog.OpenFile : FileDialog.SaveFile
}
