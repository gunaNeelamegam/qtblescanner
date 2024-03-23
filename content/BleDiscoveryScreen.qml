import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts


Item {
    id : ble_discover
    Layout.fillHeight: true
    Layout.fillWidth:  true

    property string errorMessage: device_finder?.info ?? ""
    property string infoMessage: device_finder?.error ?? ""


    // Need's to move this may use for remove the ui elements after the timeout

    RowLayout  {
        id:  topNavigationBar
        width: parent.width
        anchors.top: parent.top
        z:  3


        // Need's to show if the scanning is done or in Progress
        RoundButton {
            id: backBtn
            icon.cache: true
            Layout.alignment: Qt.AlignLeft
            icon.source: "assets/back.png"
            onClicked: {
                navigationStack.currentIndex = 0;
            }
        }

        RoundButton {
            id: startDiscoveryBtn
            Layout.alignment: Qt.AlignRight
            icon.cache: true
            checkable: true
            icon.source: device_finder?.scanning ? "assets/scanning.svg" : "assets/scanner.svg"
            onClicked: {
                if (!checked && device_finder.scanning) {
                    device_finder.stopDiscovery();
                } else {
                    device_finder.startDiscovery();
                }
            }
        }
    }

    ColumnLayout {
        id: discovery_devices
        anchors.top: topNavigationBar.bottom
        width: parent.width * 0.8
        anchors.centerIn: parent
        height: parent.height - (topNavigationBar.height * 2)

        ListView {
            id: devices
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignCenter
            model: device_finder?.devices
            spacing: 20
            clip: true

            delegate: Rectangle {
                id: box
                radius: 20
                anchors.margins: 10

                required property int index
                required property var modelData
                width: devices.width
                height: connect_btn.height

                Text {
                    id: device
                    anchors.leftMargin: 20
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter

                    text: box.modelData.deviceName
                }

                Text {
                    id: deviceAddress
                    text: box.modelData.deviceAddress
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle {
                    implicitHeight: connect_btn.implicitHeight
                    implicitWidth:  connect_btn.implicitWidth
                    anchors.right: parent.right
                    anchors.rightMargin: 20
                    anchors.verticalCenter: parent.verticalCenter


                    Button {
                        id: connect_btn
                        text: "connect"
                        hoverEnabled: true
                    }
                }

                Connections {
                    target: connect_btn
                    function onClicked () {
                        device_finder.requestPairing(box.modelData.deviceAddress);
                    }
                }
            }

            remove: Transition {
                ParallelAnimation {
                    NumberAnimation { property: "opacity"; to: 0; duration: 1000 }
                    NumberAnimation { properties: "x,y"; to: 100; duration: 1000 }
                }
            }
        }
    }

    Dialog {
        id: travelTypeDialog
        title: qsTr("Navigation Type")
        anchors.centerIn: parent
        modal: true
        ColumnLayout {
            anchors.fill: parent
            Label {
                id : navigationInfo
                text: "Please, provide which type of Navigation you Excited."
                font.bold: true
                font.pixelSize: 20
                smooth: true
            }

            Button {
                id: textToTextNavigationBtn
                text: "Text to Text Navigation"
                onClicked: {
                    dialog2.accept();
                }
                smooth: true
            }
            Button {
                id: mapNavigation
                text: "route Navigation"
                onClicked: {
                    dialog2.accept();
                }
                smooth: true
            }
        }


        function isConnectedCallback(isConnectedStatus) {
            if (isConnectedStatus) {
                travelTypeDialog.open();
            }
            // show the notification Message for Connection Lost
        }

        Component.onCompleted: {
            device_finder.isDeviceConnectedChanged.connect(isConnectedCallback);
        }
    }

    Connections {
        target: device_finder
        function onIsDeviceConnectedChanged() {
            console.log("Is Connected Callback");
            console.log(device_finder.connectedDevice.address, device_finder.connectedDevice.name);
        }
    }

    onErrorMessageChanged: {
        if (errorMessage.trim.length) {
            error_stackbar.open();
        }
    }

    onInfoMessageChanged: {
        if (infoMessage.trim.length) {
            info_stackbar.open()
        }
    }

    CustomToolTip {
        id: error_stackbar
        background_color: Material.theme === Material.Light ? Qt.lighter(Material.Red): Qt.darker(Material.Red)
        message: errorMessage
    }


    CustomToolTip {
        id: info_stackbar
        background_color: Material.theme === Material.Light ? Qt.lighter(Material.Green): Qt.darker(Material.Grey)
        message: infoMessage
    }

    Component.onCompleted: {
        console.log(Material.theme)
    }
}
