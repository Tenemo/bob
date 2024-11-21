#ifndef I2CSCANNER_H
#define I2CSCANNER_H

#include <Arduino.h>
#include <Wire.h>

int scanI2CBus(TwoWire &wire);

#endif // I2CSCANNER_H
