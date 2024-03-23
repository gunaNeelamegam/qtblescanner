import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: homeScreen
    property bool connectionStatus: false

    ColumnLayout {
         id: screenBtns
         width: parent.width * 0.8
         anchors.centerIn: parent
         spacing: 40

        Button  {
             id: homeSrnBtn
             Layout.fillWidth: true
             icon.cache: true
             text: qsTr("Ble Discovery Screen")
         }

         Button  {
             id: bleScrBtn
             Layout.fillWidth: true
             text: qsTr("Actions")
             icon.cache: true
         }
     }

    function onHomeSrnBtnClicked() {
        navigationStack.currentIndex = 1;
    }

    function onActionsSrnBtnClicked() {
        navigationStack.currentIndex = 2;
    }

    Component.onCompleted: {
        homeSrnBtn.clicked.connect(onHomeSrnBtnClicked);
        bleScrBtn.clicked.connect(onActionsSrnBtnClicked)
    }
}
