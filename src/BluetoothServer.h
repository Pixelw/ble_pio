//
// Created by Carl Su on 25-8-25.
//

#ifndef BLUETOOTHSERVER_H
#define BLUETOOTHSERVER_H


#include <BluetoothSerial.h>
// e-Paper driver
//
// Created by carl- on 25/8/24.
// refined version from srvr.h
//
class BluetoothServer {
    bool btIsOn = false;
    bool btIsConnected = false;
    int msgPos = 0;
    int length = 0;

    BluetoothSerial btClient;

    bool available();

    void write(const char *value);

    int read();

    void flush();

public:
    bool btSetup();
    bool loop();
};

#endif //BLUETOOTHSERVER_H