#include "mibandservice.h"

const char* MiBandService::UUID_SERVICE_MIBAND = "{0000FEE0-0000-1000-8000-00805f9b34fb}";
const char* MiBandService::UUID_CHARACTERISTIC_MIBAND_CONFIGURATION = "{00000003-0000-3512-2118-0009af100700}";
const char* MiBandService::UUID_CHARACTERISTIC_MIBAND_BATTERY_INFO = "{00000006-0000-3512-2118-0009af100700}";
const char* MiBandService::UUID_CHARACTERISTIC_MIBAND_DEVICE_EVENT = "{00000010-0000-3512-2118-0009af100700}";
const char* MiBandService::UUID_CHARACTERISTIC_MIBAND_NOTIFICATION = "{00000002-0000-3512-2118-0009af100700)";

MiBandService::MiBandService(QObject *parent) : BipService(UUID_SERVICE_MIBAND, parent)
{
    connect(this, &BipService::characteristicChanged, this, &MiBandService::characteristicChanged);
    //    connect(this, &BipService::readyChanged, this, &MiBandService::serviceReady);
}

void MiBandService::characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qDebug() << "MiBand Changed:" << characteristic.uuid() << "(" << characteristic.name() << "):" << value;

    if (value[0] == CHAR_RESPONSE && value[1] == COMMAND_REQUEST_GPS_VERSION && value[2] == CHAR_SUCCESS) {
        m_gpsVersion = value.mid(3);
        qDebug() << "Got gps version = " << m_gpsVersion;
        emit gpsVersionChanged();
    }

    if (characteristic.uuid().toString() == UUID_CHARACTERISTIC_MIBAND_DEVICE_EVENT) {
        if (value[0] == EVENT_DECLINE_CALL) {
            emit declineCall();
        } else if (value[0] == EVENT_IGNORE_CALL) {
            emit ignoreCall();
        }

    }
}

void MiBandService::characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qDebug() << "Read:" << characteristic.uuid() << "(" << characteristic.name() << "):" << value;
}

void MiBandService::requestGPSVersion()
{
    qDebug() << "Ready" << ready();
    if (ready()) {
        QLowEnergyCharacteristic characteristic = service()->characteristic(QBluetoothUuid(QString(UUID_CHARACTERISTIC_MIBAND_CONFIGURATION)));
        service()->writeCharacteristic(characteristic, QByteArray(&COMMAND_REQUEST_GPS_VERSION, 1), QLowEnergyService::WriteWithoutResponse);
    }
}


QString MiBandService::gpsVersion()
{
    return m_gpsVersion;
}
