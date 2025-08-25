#include "BluetoothServer.h"

#include "buff.h"
#include "epd.h"

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
        Serial.println("Bluetooth server is on");
    } else {
        Serial.println("Bluetooth server is off");
    }

    btIsConnected = false;

    // SPI initialization
    EPD_initSPI();

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

    // Exit if there is no bluetooth connection
    if (!btIsConnected) {
        return false;
    }

    // Waiting the client is ready to send data
    while (!btClient.available()) {
        delay(10);
    }

    // Set buffer's index to zero
    // It means the buffer is empty initially
    Buff__bufInd = 0;

    // While the stream of 'client' has some data do...
    while (available()) {
        int read = btClient.read();
        Buff__bufArr[Buff__bufInd++] = read;
        Serial.printf("0x%x ,", static_cast<byte>(read));
        if (Buff__bufInd % 16 == 0) {
            Serial.println();
        }
    }
    Serial.println();

    const char command = Buff__bufArr[0];
    // get epd type from packet
    if (command == 'I') {
        length = 0;
        EPD_dispIndex = Buff__bufArr[1];

        Serial.printf("<<<EPD %s", EPD_dispMass[EPD_dispIndex].title);

        EPD_dispInit();

        Buff__bufInd = 0;
        flush();
    } else if (command == 'L') {
        Serial.print("<<<LOAD");
        const int dataSize = Buff__getWord(1);
        length += dataSize;

        if (Buff__bufInd < dataSize || length != Buff__getN3(3)) {
            Buff__bufInd = 0;
            flush();

            Serial.print(" - failed!>>>");
            write("Error!");
            return true;
        }

        // Load data into the e-Paper
        // if there is loading function for current channel (black or red)
        if (EPD_dispLoad != 0) {
            EPD_dispLoad();
        }

        Buff__bufInd = 0;
        flush();
    } else if (command == 'N') {
        // Initialize next channel
        // Print log message: next data channel
        Serial.print("<<<NEXT");

        // Instruction code for writing data into
        // e-Paper's memory
        int code = EPD_dispMass[EPD_dispIndex].next;
        if (EPD_dispIndex == 34) {
            if (flag == 0)
                code = 0x26;
            else
                code = 0x13;
        }

        EPD_invert = EPD_dispIndex == 8;

        if (code != -1) {
            // Print log message: instruction code
            Serial.printf(" %d", code);

            // Do the selection of the next data channel
            EPD_SendCommand(code);
            delay(2);
        }

        // Set up the function for loading chosen channel's data
        EPD_dispLoad = EPD_dispMass[EPD_dispIndex].chRd;

        Buff__bufInd = 0;
        flush();
    } else if (command == 'S') {
        EPD_dispMass[EPD_dispIndex].show();

        Buff__bufInd = 0;
        flush();

        Serial.printf("<<<SHOW");
    }

    write("Ok!");
    delay(1);

    Serial.print(">>>");
    return true;
}
