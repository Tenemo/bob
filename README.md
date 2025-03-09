# Bob

[![Netlify Status](https://api.netlify.com/api/v1/badges/064eb119-d22e-4a70-9b0b-50c7c18712ab/deploy-status)](https://app.netlify.com/sites/bob-client/deploys)

A full-stack robotics project featuring an ESP32-S3 based hexapod robot with camera vision.

Uses OpenAI Realtime and Structured Outputs APIs to process visual data and talk to the user in real time with an incredibly realistic voice.

## Overview

Bob is a hexapod robot project that combines:

- ESP32-S3 microcontroller for core functionality
- React-based web client for control interface
- Express.js server for API routing and coordination
- Real-time video streaming and analysis
- OpenAI integration for vision processing
- Servo control for movement and positioning
- Audio playback capabilities

## System architecture

### Hardware components

- FireBeetle 2 board ESP32-S3-U (N16R8)
- OV2640 camera module
- MAX98357A decoder stereo DAC with built-in class D amplifier
- Step-down voltage regulator D24V90F5 - 5V 9A (Pololu 2866)
- PCA9685 PWM controller
- 12x SG90 Servo motors
- TFT 1.8" 128x160px SPI display
- 2220 mAh 7.4 volt Li-pol battery

### Software components

#### Client (bob-client)

- React-based web interface
- Redux for state management
- Material-UI components
- Real-time video display
- Interactive controls for robot movement
- OpenAI vision analysis display

#### Server (bob-server)

- Express.js based API server
- CORS support
- Request logging and error handling
- API routing for robot control
- File system operations for audio management

#### Embedded (ESP32)

- Async web server for API endpoints
- I2C communication for peripherals
- SPIFFS file system for storage
- PSRAM audio caching
- PWM servo control
- I2S audio output

## Getting started

### Prerequisites

1. Development tools:

   - Visual Studio Code with PlatformIO extension
   - Node.js 18+ and npm
   - Git

2. Hardware setup:
   - ESP32-S3 development board
   - Configured peripherals (camera, servos, etc.)
   - USB connection for programming

### Installation

1. Clone the repository with submodules:

   ```bash
   git clone --recursive https://github.com/yourusername/bob.git
   cd bob
   ```

2. Set up the client:

   ```bash
   cd bob-client
   npm install
   cp .env.example .env
   ```

3. Set up the server:

   ```bash
   cd ../bob-server
   npm install
   cp .env.example .env
   ```

4. Configure the ESP32:
   - Copy `include/Env.sample` to `include/Env.h`
   - Update WiFi credentials in `Env.h`:
     ```cpp
     #define WIFI_SSID "your_wifi_ssid"
     #define WIFI_PASSWORD "your_wifi_password"
     ```

### Building and deploying

1. ESP32 firmware:

   ```bash
   # Using PlatformIO CLI
   pio run -t upload
   # Upload SPIFFS
   pio run -t uploadfs
   ```

2. Start the client:

   ```bash
   cd bob-client
   npm start
   ```

3. Start the server:
   ```bash
   cd bob-server
   npm run dev
   ```

## API documentation

### ESP32 endpoints

#### Health check

- `GET /health-check` - System status check
- Response: `{"status": "OK", "message": "Server is running"}`

#### Camera control

- `GET /capture` - Capture photo
- Returns: JPEG image

#### Servo control

- `POST /rotate` - Control servo position
  ```json
  {
    "motorIndex": 0,
    "degrees": 90
  }
  ```
- `POST /move` - Predefined movement patterns
  ```json
  {
    "type": "reset"
  }
  ```

#### Audio management

- `POST /audio` - Upload and play audio file
  - Max file size: 8MB
  - Format: WAV only
  - Content-Type: multipart/form-data

#### File system

- `GET /file-list` - List stored files
- Response: List of files with sizes

### Web server endpoints

The Express.js server provides additional endpoints for:

- Robot control coordination

## Development

### Project structure

```
bob/
├── bob-client/          # React web interface
│   ├── src/
│   │   ├── app/        # Core application setup
│   │   ├── features/   # Feature modules
│   │   └── components/ # Reusable components
│   └── public/         # Static assets
├── bob-server/         # Express.js server
│   └── src/
│       ├── routes/    # API routes
│       └── config.ts  # Server configuration
└── include/           # ESP32 header files
    ├── Camera.h      # Camera functionality
    ├── Servos.h     # Servo control
    └── audio/       # Audio processing
```

### Code style

- C++: Follow Arduino style guide
- TypeScript: ESLint with provided config
- React: Function components with hooks
- Formatting: Prettier for web code

## Performance considerations

1. Memory management:

   - ESP32 PSRAM usage for image buffering
   - Audio buffer size limitations
   - React component optimization

2. Real-time operations:

   - Servo movement coordination
   - Video streaming optimization
   - Audio playback timing

3. Network efficiency:
   - WebSocket for real-time data
   - Image compression settings
   - Request batching where appropriate
