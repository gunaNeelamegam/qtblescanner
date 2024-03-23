#ifndef DEVICEFINDER_H
#define DEVICEFINDER_H

#include "bluetoothbaseclass.h"
#include "deviceinfo.h"
#include <QtBluetooth/qbluetoothdevicediscoveryagent.h>
#include <QtBluetooth/qbluetoothlocaldevice.h>
#include <QtBluetooth/qlowenergycontroller.h>
#include <QtBluetooth/qlowenergyservice.h>

#include <QtCore/qtimer.h>
#include <QtCore/qvariant.h>

#include <QtQmlIntegration/qqmlintegration.h>

QT_BEGIN_NAMESPACE
class QBluetoothDeviceInfo;
QT_END_NAMESPACE


class DeviceFinder: public BluetoothBaseClass
{

    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)
    Q_PROPERTY(QVariant devices READ devices NOTIFY devicesChanged)
    Q_PROPERTY(bool pairingStatus READ pairingStatus NOTIFY pairingStatusChanged FINAL)
    Q_PROPERTY(bool isDeviceConnected READ isDeviceConnected NOTIFY isDeviceConnectedChanged FINAL)
    Q_PROPERTY(QVariant connectedDevice READ connectedDevice NOTIFY connectedDeviceChanged FINAL)
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("This class is not intended to be created directly")

public:
    DeviceFinder(QObject *parent = nullptr);
    ~DeviceFinder();

    bool scanning() const;
    QVariant devices() const;
    bool pairingStatus() const;
    bool isDeviceConnected() const ;
    QVariant connectedDevice() const;

public slots:
    void turnOff();
    void turnOn();
    void startDiscovery();
    void connectToDevice(DeviceInfo *deviceInfo);
    void disconnectWithDevice();
    void stopDiscovery();
    void connectToService(const QString &address);
    void requestPairing(const QString &requestedConnectionAddress);
    void requestUnPairing(const QString &requestConnectionAddress = "");
    void addDevice(const QBluetoothDeviceInfo &device);
    void scanError(QBluetoothDeviceDiscoveryAgent::Error error);
    void scanFinished();
    void onPairingFinished(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing);

signals:
    void scanningChanged();
    void devicesChanged();
    void pairingStatusChanged();
    void isDeviceConnectedChanged();
    void connectedDeviceChanged();

private:
    QBluetoothDeviceDiscoveryAgent *m_deviceDiscoveryAgent; // discovering ble agent
    QList<DeviceInfo *> m_devices; // discovering devices
    QBluetoothLocalDevice localDevice;
    QTimer m_demoTimer;
    bool m_pairingStatus;
    bool m_isDeviceConnected;
    QLowEnergyController *m_control = nullptr; // controller
    QLowEnergyService *m_service = nullptr; // service
    DeviceInfo *m_currentDevice = nullptr; // connected Device

};

#endif // DEVICEFINDER_H
