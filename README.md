# Bob

[![Netlify Status](https://api.netlify.com/api/v1/badges/064eb119-d22e-4a70-9b0b-50c7c18712ab/deploy-status)](https://app.netlify.com/sites/bob-client/deploys)

An ESP32-S3-based hexapod robot with a voice, camera vision, and with autonomous usage of available "tools": basic movement and seeing.

Uses OpenAI Realtime and Structured Outputs APIs to process visual data and talk to the user in real time with a realistic voice.

## Overview

https://github.com/user-attachments/assets/bbba81ba-5eeb-43fc-8d48-552be9b4c37b

Bob is a hexapod robot project that can:

- ...

## Components

### Hardware

- FireBeetle 2 board ESP32-S3-U (N16R8)
- OV2640 camera module
- MAX98357A decoder stereo DAC with built-in class D amplifier
- Step-down voltage regulator D24V90F5 - 5V 9A (Pololu 2866)
- PCA9685 PWM controller
- 12x SG90 Servo motors
- TFT 1.8" 128x160px SPI display
- 2220 mAh 7.4 volt Li-pol battery

### Software

#### Embedded (ESP32)

- Async web server for API endpoints
- I2C communication for peripherals
- SPIFFS file system for storage
- PSRAM audio caching
- PWM servo control
- I2S audio output

#### Client (bob-client)

- React-based web interface
- Redux for state management
- Material-UI components
- Real-time video display
- Interactive controls for robot movement
- OpenAI vision analysis display
