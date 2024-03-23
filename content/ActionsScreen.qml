import QtQuick 6.2
import QtQuick.Controls.Material
import QtQuick.Layouts

Item {
 id: actionScreen



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
 }

 ScrollView {
     anchors.top: topNavigationBar.bottom
     width: parent.width
     height: parent.height

 ColumnLayout {
     id: actionBar
     spacing: 5
     height: parent.height
     width: parent.width

     RowLayout {
         spacing: 2
         width: parent.width * 100
         Text {
             id: deviceInfoAddress
             font.bold: true
             font.pixelSize: 25
             text: device_finder?.connectedDevice?.deviceAddress ?? ""
         }

         Text {
             id: deviceInfoName
             font.bold: true
             font.pixelSize: 25
             text: device_finder?.connectedDevice?.deviceName ?? ""
         }
     }

     RoundButton {
         id: disconnect
         text: "diconnect"
         enabled: device_finder?.isDeviceConnected ?? false
         onClicked: {
            device_finder?.disconnectWithDevice();
         }


    }
     RoundButton {
         id: requestUnPair
         text: "unpair"
         enabled: device_finder?.isDeviceConnected ?? false
         onClicked: {
            device_finder?.disconnectWithDevice();
         }
     }


     RoundButton {
         id: turnOn
         text: "Turn On"
         onClicked: {
            device_finder?.turnOn();
         }
    }

     RoundButton {
         id: turnOff
         text: "Turn Off"
         onClicked: {
            device_finder?.turnOff();
         }
    }
 }
 }

}
