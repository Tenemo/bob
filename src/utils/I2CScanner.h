#ifndef I2CSCANNER_H
#define I2CSCANNER_H

#include <Arduino.h>
#include <Wire.h>

/**
 * @brief Scans the specified I2C bus for connected devices.
 *
 * This function iterates through possible I2C addresses and attempts to
 * communicate with devices at each address. It prints the addresses of any
 * detected devices to the serial monitor.
 *
 * @param wire Reference to the TwoWire instance representing the I2C bus to
 * scan.
 * @return int The address of the first found I2C device, or -1 if no devices
 * are found.
 */
int scanI2CBus(TwoWire &wire);

#endif // I2CSCANNER_H
