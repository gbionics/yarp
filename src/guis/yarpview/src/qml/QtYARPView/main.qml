/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Window 2.1

// Import the YARPView module
import robotology.yarp.view 1.0
// Imports the resources of the YARPView Module.
// This serves in case the the module has its
// own qml defined in the resources
import "qrc:/YARPView/" 1.0

ApplicationWindow {
    id: window

    x: vSurface.posX
    y: vSurface.posY
    width: vSurface.windowWidth
    height: vSurface.windowHeight
    property int menuH: 0
    title: vSurface.moduleTitle

    Component.onCompleted: function(){
        vSurface.synchronizeRightEnabled();
    }

    function calc()    {
        if(menuH != 0){
            return;
        }

        menuH = window.height - vSurface.height - statusBar.height
        vSurface.menuHeight =  menuH
    }


    /**************************************************/
    menuBar: YARPViewMenu{
        id: menu
    }

    footer: YARPViewStatusBar{
        id: statusBar
    }


    VideoSurface{
        id: vSurface
        objectName: "YARPVideoSurface"
        anchors.fill: parent
        dataArea: statusBar
        menuHeight: menuH
    }
    /**************************************************/

    Connections{
        target: vSurface
        function onChangeWindowSize(w, h) {
            window.width = w
            window.height = h
        }
        function onAutosize(check) {
            if(menu !== undefined){
                menu.enableAutosize(check)
            }
        }
        function onSynchRate(check) {
            if(menu !== undefined){
                menu.enableSynch(check)
            }
        }
        function onSetName(name) {
            statusBar.setName(name)
        }
        function onSetTitle(inputTitle) {
            window.title=inputTitle
        }
        function onSaveSetClosed(check) {
            menu.saveSetChecked(check);
        }
        function onSaveSingleClosed(check) {
            menu.saveSingleChecked(check);
        }
        function onRightClickEnabled(enabled) {
            menu.externallyCheckRightClick(enabled);
        }
    }

    Connections{
        target: menu
        function onFreeze(checked) {
            vSurface.freeze(checked)
        }
        function onSetOriginalSize() {
            calc()
            vSurface.setOriginalSize()
        }

        function onSetOriginalAspectRatio() {
            calc()
            vSurface.setOriginalAspectRatio()
        }

        function onChangeRefreshInterval() {
            vSurface.changeRefreshInterval()
        }

        function onSynchDisplayPeriod(checked) {
            vSurface.synchDisplayPeriod(checked)
        }

        function onSynchDisplaySize(checked) {
            vSurface.synchDisplaySize(checked)
        }

        function onSaveSingleImage(checked) {
            vSurface.saveSingleImage(checked)
        }

        function onSaveSetImages(checked) {
            vSurface.saveSetImages(checked)

        }

        function onPickColor(checked) {
            vSurface.enableColorPicking(checked);
            statusBar.setPixelValVisibility(checked);
        }

        function onRightClickEnable(checked) {
            vSurface.enableRightClick(checked);
        }

        function onAbout() {
            vSurface.about()
        }

        function onQuit() {
            Qt.quit()
        }
    }

    Connections{
        target: statusBar

        function onHeightChanged() {
            calc();
            vSurface.setOriginalAspectRatio()
        }
    }
}
