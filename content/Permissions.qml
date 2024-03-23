pragma Singleton
import QtQuick
import QtCore


Item {
    id: permissions

    property BluetoothPermission blePermission: BluetoothPermission {
        id: blepermission
        onStatusChanged: {
            console.log("Bluetooth Permission Changed : ", status);
        }
    }

    property LocationPermission locationPermission: LocationPermission{
        id: locationPermission
        onStatusChanged:  {
            console.log("Location Permission Status Changed : ",  status);
        }
    }

    function requestBluetoothPermission() {
        if (blePermission.status  === 0) {
            blePermission.request();
            return;
        }
        if (blePermission.status === 2) {
            console.log("Bluetooth Permission Denied");
            return;
        }
        console.log("Bluetooth Permission Granted");

    }

    function requestLocationPermission() {
        if (locationPermission.status  === 0) {
            locationPermission.request();
            return;
        }
        if (locationPermission.status === 2) {
            console.log("Location Permission Denied");
            return;
        }
        console.log("Location Permission Granted");
    }


    Timer {
        id: locationRequester
        interval: 5000
        onTriggered: {
            requestLocationPermission();
            stop();
        }
        running: false
    }

    Timer {
        id: bluetoothRequester
        interval: 2000
        onTriggered: {
            requestBluetoothPermission();
            stop();
        }
        running: false
    }

    function start() {
        locationRequester.start();
        bluetoothRequester.start();
    }

}
