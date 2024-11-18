#include "I2CScanner.h"

int scanI2CBus(TwoWire &wire) {
    byte error, address;
    int nDevices = 0;
    int foundAddress = -1;

    Serial.println("Scanning I2C bus...");

    for (address = 1; address < 127; address++) {
        wire.beginTransmission(address);
        error = wire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) {
                Serial.print("0");
            }
            Serial.println(String(address, HEX));
            nDevices++;
            if (foundAddress == -1) {
                foundAddress = address;
            }
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            if (address < 16) {
                Serial.print("0");
            }
            Serial.println(String(address, HEX));
        }
    }
    Serial.println("I2C scan done.\n");
    return foundAddress; // Return the found address or -1 if none
}
