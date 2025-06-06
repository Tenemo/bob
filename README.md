# Bob

[![Netlify Status](https://api.netlify.com/api/v1/badges/064eb119-d22e-4a70-9b0b-50c7c18712ab/deploy-status)](https://app.netlify.com/sites/bob-client/deploys)

Hexapod breadboard-on-legs toy project based on ESP32-S3 with a voice, camera vision, autonomous basic movement and seeing. Uses OpenAI Realtime API and Structured Output API to process visual data and talk to the user in (delayed, due to processing power constraints) real time with a realistic voice.

## Demo

Demo (with sound) of the robot booting up, speaking, moving and seeing.

https://github.com/user-attachments/assets/bbba81ba-5eeb-43fc-8d48-552be9b4c37b

Note that the Realtime API calls appropriate "tools" itself, I don't explicitly tell it to use the camera or the movement methods.

## Overview

Bob is a hexapod robot controlled by a web app that can:
- Talk to you in many different languages. It uses the latest (as of March 2025) OpenAI Realtime API version to do that.
- See what's in front of it using a wide-angle camera module. The Realtime API doesn't support images, so the image first is described into JSON using an OpenAI old-school text API with Structured Output. The output is then passed to the Realtime API.
- Autonomously, based on what you tell it do to, decide what it wants to use, camera or movement capabilities.
- Stand up, sit down, and wiggle - unfortunately I couldn't get it to walk properly, the servomotors used ended up being too weak, mostly due to the battery weight.
- Stay powered using its onboard 2220 mAH power supply.
- Output logs to the onboard LCD display.

![bob_camera](https://github.com/user-attachments/assets/d629813a-951a-4147-8271-3bfea1b55f13)
![bob_top](https://github.com/user-attachments/assets/c647f8c9-22f2-4072-9830-78c0b706d7cf)
![bob_underside](https://github.com/user-attachments/assets/38bd589b-22ac-4261-b01d-05b78a0e3a40)


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
