// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 6.2
import zilogicbluetooth
import QtQuick.Layouts
import QtQuick.Controls.Material

ApplicationWindow {

    property list <Loader> loaders:  [homeScreen, discoveryScreen, actionScreen]

    width: Constants.width
    height: Constants.height
    visible: true
    title: "zilogicbluetooth"
    Material.theme: Material.Dark


    Loader {
        id: homeScreen
        anchors.fill: parent
        source: "HomeScreen.qml"
        active: true
        visible: false
    }

    Loader {
        id: discoveryScreen
        source: "BleDiscoveryScreen.qml"
        active: true
        visible: false
        anchors.fill: parent
    }

    Loader {
        id: actionScreen
        anchors.fill: parent
        source: "ActionsScreen.qml"
        active: true
        visible: false
    }


    StackLayout {
        id: navigationStack
        data: loaders.map((loader) => loader.item)
        currentIndex: 0
        anchors.fill: parent
    }

    Component.onCompleted: {
        Permissions.start();
    }

}

