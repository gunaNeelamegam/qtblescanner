#include "devicefinder.h"
#include "deviceinfo.h"

#include <QtBluetooth/qbluetoothdeviceinfo.h>
#include <QtBluetooth/qbluetoothlocaldevice.h>
#include <QLoggingCategory>


// constructor
DeviceFinder::DeviceFinder(QObject *parent):
    BluetoothBaseClass(parent)
{

    QLoggingCategory::setFilterRules(QStringLiteral("qt.bluetooth* = true"));
    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    m_deviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(150000); // Never Scanning Stopped
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &DeviceFinder::addDevice);

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred,
            this, &DeviceFinder::scanError);

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &DeviceFinder::scanFinished);

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled,
            this, &DeviceFinder::scanFinished);

    connect(&localDevice, &QBluetoothLocalDevice::pairingFinished, this,&DeviceFinder::onPairingFinished);

    connect(&localDevice, &QBluetoothLocalDevice::errorOccurred, this, [this] (QBluetoothLocalDevice::Error error) {

            if (error == QBluetoothLocalDevice::Error::PairingError) {
                if (m_pairingStatus) {
                    setError(tr("Pairing Error, Please try to Connect !"));
                    m_pairingStatus = false;
                }
            }
            if (error == QBluetoothLocalDevice::Error::MissingPermissionsError) {
                setError(tr("Permission Denied for Pairing"));
                m_pairingStatus = false;
            }
            if (QBluetoothLocalDevice::Error::UnknownError == error) {
                setError(tr("UnKnown Permission"));
            }

            emit errorChanged();
            emit pairingStatusChanged();
        });
}

// destructor
DeviceFinder::~DeviceFinder()
{
    qDeleteAll(m_devices);
    m_devices.clear();
}

void DeviceFinder::startDiscovery()
{
    stopDiscovery();
    clearMessages();
    m_currentDevice = nullptr;
    qDeleteAll(m_devices);
    m_devices.clear();

    emit devicesChanged();


    m_deviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);

    emit scanningChanged();
    setInfo(tr("Scanning for devices..."));
}

void DeviceFinder::stopDiscovery()
{
    if (m_deviceDiscoveryAgent->isActive()) {
        qDebug() << "Agent is current Active for Service Discovery" ;
        m_deviceDiscoveryAgent->stop();
    }
    emit scanningChanged();
}


void DeviceFinder::onPairingFinished(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing) {
    m_pairingStatus = true;
    connectToService(address.toString());
    emit pairingStatusChanged();
}


void DeviceFinder::turnOff() {
    if (localDevice.hostMode() != QBluetoothLocalDevice::HostMode::HostPoweredOff) {
        localDevice.setHostMode(QBluetoothLocalDevice::HostMode::HostPoweredOff);
    }
}

void DeviceFinder::turnOn() {
    if (localDevice.hostMode() == QBluetoothLocalDevice::HostMode::HostPoweredOff) {
        qDebug() << localDevice.isValid() <<  localDevice.address();
        localDevice.powerOn();
        qDebug() << "Enabled " << localDevice.hostMode();
    }
}

void DeviceFinder::requestPairing(const QString &requestedConnectionAddress) {
    if (localDevice.hostMode() == QBluetoothLocalDevice::HostMode::HostPoweredOff) {
        setError(tr("Please Turn On the Bluetooth Adaptor"));
        emit errorChanged();
        return ;
    }


    QBluetoothLocalDevice::Pairing pairingRequestType;
#ifdef Q_OS_LINUX
// Need's to explicitly pair and send data
// pairingRequestType = QBluetoothLocalDevice::Pairing::AuthorizedPaired;
#endif
#ifdef Q_OS_ANDROID
    pairingRequestType = QBluetoothLocalDevice::Pairing::Paired;
#endif
    qDebug() << requestedConnectionAddress.toStdString() << localDevice.name() <<localDevice.address();
    localDevice.requestPairing(QBluetoothAddress(requestedConnectionAddress), pairingRequestType);
}

void DeviceFinder::requestUnPairing(const QString &requestDisconnectConnectionAddress)
{
    if (requestDisconnectConnectionAddress != "") {
        for ( auto remoteDevice  : localDevice.connectedDevices()) {
            qDebug() << "Remote Device : "  << remoteDevice.toString();
        }
    }
    qDebug() << "unPairing";
#ifdef Q_OS_ANDROID
    localDevice.requestPairing(QBluetoothAddress(requestDisconnectConnectionAddress), QBluetoothLocalDevice::Pairing::Unpaired);
#endif
}


void DeviceFinder::addDevice(const QBluetoothDeviceInfo &device)
{
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        auto devInfo = new DeviceInfo(device);
        auto it = std::find_if(m_devices.begin(), m_devices.end(),
                               [devInfo](DeviceInfo *dev) {
                                   return devInfo->getAddress() == dev->getAddress();
                               });

        if (it == m_devices.end()) {
            qDebug() << device.name() << device.address();
            m_devices.append(devInfo);
        } else {
            auto oldDev = *it;
            *it = devInfo;
            delete oldDev;
        }
        setInfo(tr("Low Energy device found. Scanning more..."));
        emit infoChanged();
        emit devicesChanged();
    }
}

void DeviceFinder::scanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
        setError(tr("The Bluetooth adaptor is powered off."));
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
        setError(tr("Writing or reading from the device resulted in an error."));
    else
        setError(tr("An unknown error has occurred."));
}

void DeviceFinder::scanFinished()
{
    if (m_devices.isEmpty())
        setError(tr("No Low Energy devices found."));
    else
        setInfo(tr("Scanning done."));
    emit scanningChanged();
    emit devicesChanged();
}

void DeviceFinder::connectToService(const QString &address)
{
    m_deviceDiscoveryAgent->stop();

    DeviceInfo *currentDevice = nullptr;

    for (QObject *entry : std::as_const(m_devices)) {
        auto device = qobject_cast<DeviceInfo *>(entry);
        if (device && device->getAddress() == address) {
            currentDevice = device;
            break;
        }
    }
    if (currentDevice)
    {
        connectToDevice(currentDevice);
        setInfo(tr("Device Connected ", currentDevice->getAddress().toStdString().c_str()));
        emit devicesChanged();
    }
    clearMessages();
}


void DeviceFinder::connectToDevice(DeviceInfo *deviceInfo) {
    disconnectWithDevice();
    m_currentDevice = deviceInfo;


    if (m_currentDevice) {
        m_control = QLowEnergyController::createCentral(m_currentDevice->getDevice(), this);
        m_control->setRemoteAddressType(QLowEnergyController::PublicAddress);

        connect(m_control, &QLowEnergyController::stateChanged, this, [this](QLowEnergyController::ControllerState state) {
            if (state == QLowEnergyController::UnconnectedState) {
                m_control->disconnectFromDevice();
                qDebug() << "Remote Device Disconnected .." ;
            }
        });
        connect(m_control, &QLowEnergyController::serviceDiscovered,
                this, [this](const QBluetoothUuid &newService) {
            qDebug() << newService.toString();
        });


        connect(m_control, &QLowEnergyController::discoveryFinished,
                this, [this]() {
            qDebug() << "Device Discovery Finished" ;
        });

        connect(m_control, &QLowEnergyController::errorOccurred, this,
                [this](QLowEnergyController::Error error) {
                    Q_UNUSED(error);
                    setError("Cannot connect to remote device.");
                    emit errorChanged();
                });


        connect(m_control, &QLowEnergyController::connected, this, [this]() {
            m_isDeviceConnected = true;
            emit isDeviceConnectedChanged();
            m_control->discoverServices();
            setInfo("Controller connected. Search services...");
        });

        connect(m_control, &QLowEnergyController::disconnected, this, [this]() {
            m_isDeviceConnected = false;
            emit isDeviceConnectedChanged();
            setError("LowEnergy controller disconnected");
            qInfo() << "Disconnected Successfully";
        });

        m_control->connectToDevice();
        emit connectedDeviceChanged();
        qDebug() << "After Registered Peripheral Callback's";
    }
}

void DeviceFinder::disconnectWithDevice() {
    // disconnect with device
    if (m_control) {
        m_control->disconnectFromDevice();
        delete m_service;
        delete m_control;
        m_service = nullptr;
        m_control = nullptr;
    }

    setInfo(tr("Disconnected Successfully ", m_currentDevice->getName().toStdString().c_str()));
}

bool DeviceFinder::scanning() const {
    return m_deviceDiscoveryAgent->isActive();
}

QVariant DeviceFinder::devices() const
{
    return QVariant::fromValue(m_devices);
}

bool DeviceFinder::pairingStatus() const
{
    return m_pairingStatus;
}

bool DeviceFinder::isDeviceConnected() const
{
    return m_isDeviceConnected;
}

QVariant DeviceFinder::connectedDevice() const  {
    return QVariant::fromValue(m_currentDevice);
}
