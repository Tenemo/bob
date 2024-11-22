# bob

An embedded system project using ESP32-S3 for controlling camera, servo motors, and audio playback with a web-based API interface.

## Hardware

- FireBeetle 2 board ESP32-S3-U (N16R8)
- OV2640 camera module
- MAX98357A decoder stereo DAC with a built-in class D amplifier
- Step-Down voltage regulator D24V90F5 - 5V 9A - Pololu 2866 (dedicated 5V power for servos)
- PCA9685 PWM controller
- SG90 Servo motors

## Pin configuration

### Servo control pins

```
SDA   -> GPIO 1
SCL   -> GPIO 2
```

### I2S audio pins

```
BCK   -> GPIO 12
WS    -> GPIO 13
DATA  -> GPIO 14
```

### Status LED

```
LED   -> GPIO 10
```

## API documentation

### Health check

#### GET /health-check

Verifies the operational status of all subsystems.

**Response 200 OK**

```json
{
  "status": "OK"
}
```

### File management

#### GET /file-list

Retrieves a list of all files stored in the SPIFFS filesystem.

**Response 200 OK**

```json
{
  "files": [
    {
      "name": "/sample_music.wav",
      "size": 1048576,
      "humanReadableSize": "1.0 MB"
    },
    {
      "name": "/uploaded_audio.wav",
      "size": 524288,
      "humanReadableSize": "512.0 KB"
    }
  ]
}
```

### Camera control

#### GET /capture

Captures a photo using the OV2640 camera module.

**Parameters**

- None required

**Headers**

- Response includes `Content-Disposition: inline; filename=capture.jpg`

**Response 200 OK**

- Returns JPEG image binary data
- Content-Type: image/jpeg

### Servo control

#### POST /rotate

Controls servo motor position.

**Request body parameters**

```json
{
  "motorIndex": 0, // Required, Integer (0-15)
  "degrees": 90 // Required, Integer (0-180)
}
```

**Response 200 OK**

```json
{
  "status": "success",
  "motorIndex": 0,
  "degrees": 90
}
```

### Audio management

#### POST /audio

Uploads and plays a WAV audio file.

**Request parameters**

- Content-Type: multipart/form-data
- Maximum file size: 8MB (limited by PSRAM)
- Supported format: WAV files only
- File field name: "file"

**Response 200 OK**

```json
{
  "status": "Upload successful",
  "size": 1048576
}
```

[Previous sections remain the same until Setup instructions]

## Setup instructions

### Development environment

1. Install Visual Studio Code
2. Install PlatformIO IDE extension in VSCode
3. Clone the repository
4. Open the project folder in VSCode
5. Wait for PlatformIO to initialize and install dependencies

### Project configuration

1. Create an `include/Env.h` file with your WiFi credentials:

   ```cpp
   #define WIFI_SSID "your_ssid"
   #define WIFI_PASSWORD "your_password"
   ```

2. Install required libraries (automatically handled by PlatformIO)

### Building and uploading

1. Connect your ESP32-S3 via USB
2. Click the PlatformIO icon in the VSCode sidebar
3. Upload filesystem:
   - Build Filesystem Image
   - Upload Filesystem Image
4. Upload program:
   - Click Upload under General tasks
5. Monitor serial output:
   - Click Monitor under General tasks
   - Note the device's IP address for API access

## Initialization sequence

1. SPIFFS mounting
2. WiFi connection
3. Web server startup
4. Camera initialization
5. Servo system initialization
