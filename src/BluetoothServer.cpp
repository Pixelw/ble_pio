#include <BluetoothSerial.h>

#include "buff.h"
#include "buff.h"       // POST request data accumulator
#include "epd.h"        // e-Paper driver
//
// Created by carl- on 25/8/24.
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

    bool btSetup();

    bool loop();
};

bool BluetoothServer::available() {
    return btIsOn ? btClient.available() : false;
}

void BluetoothServer::write(const char *value) {
    if (btIsOn) {
        btClient.write(reinterpret_cast<const uint8_t *>(value), strlen(value));
    }
}

int BluetoothServer::read() {
    return btIsOn ? btClient.read() : -1;
}

void BluetoothServer::flush() {
    if (btIsOn) {
        btClient.flush();
    }
}

bool BluetoothServer::btSetup() {
    String devName("Inkintosh32");
    btIsOn = btClient.begin(devName);
    if (btIsOn) {
        Serial.println("Bluetooth is on");
    } else {
        Serial.println("Bluetooth is off");
    }

    btIsConnected = false;

    return btIsOn;
}

bool BluetoothServer::loop() {
    if (!btIsOn) {
        return false;
    }
    if (btIsConnected != btClient.hasClient()) {
        Serial.print("Bluetooth status:");
        btIsConnected = !btIsConnected;
        if (btIsConnected) {
            Serial.println("Bluetooth is connected");
        } else {
            Serial.println("Bluetooth is disconnected");
        }
    }

    if (btIsConnected) {
        return false;
    }

    // Waiting the client is ready to send data
    while (btClient.available()) {
        delay(10);
    }

    // Set buffer's index to zero
    // It means the buffer is empty initially

    while (available()) {
        int read = btClient.read();
        Buff__bufArr[Buff__bufInd++] = read;
        Serial.printf("0x%x ,", static_cast<byte>(read));
        if (Buff__bufInd % 16 == 0) {
            Serial.println();
        }
    }
    Serial.println();

    // get epd type from packet
    if (Buff__bufArr[0] == 'I') {
        length = 0;


        EPD_dispIndex = Buff__bufArr[1];

        Serial.printf("<<<EPD %s", EPD_dispMass[EPD_dispIndex].title);

        EPD_dispInit();

        Buff__bufInd = 0;
        flush();
    } else if (Buff__bufArr[0] == 'L') {
        Serial.print("<<<LOAD");
        int dataSize = Buff__getWord(1);
        length += dataSize;

        if (Buff__bufInd < dataSize || length != Buff__getN3(3)) {
        }
    }
}
