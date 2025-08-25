/**
******************************************************************************
    @file    Loader.h
    @author  Waveshare Team
    @version V2.0.0
    @date    10-August-2018
    @brief   The main file.
             This file provides firmware functions:
              + Initialization of Serial Port, SPI pins and server
              + Main loop

  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <Arduino.h>// Server functions

#include "BluetoothServer.h"

BluetoothServer BtServer;
/* Entry point ----------------------------------------------------------------*/
void setup()
{
    // Serial port initialization
    Serial.begin(115200);
    delay(1000);

    Serial.println("Serial is on");

    // Bluetooth initialization
    BtServer.btSetup();

    // Initialization is complete
    Serial.print("\r\nOk!\r\n");
}

/* The main loop -------------------------------------------------------------*/
void loop()
{
    BtServer.loop();
}
