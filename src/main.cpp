#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

#include "memorysaver.h"
#include <ArduCAM.h>
// Fixes ArduCAM overwriting swap
#ifdef swap
#undef swap
#endif
#include <Arduino.h>
#include <SPI.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Wire.h>

// This demo can only work on OV2640_MINI_2MP or ARDUCAM_SHIELD_V2 platform.
#if !(defined(OV2640_MINI_2MP) ||                                              \
      (defined(ARDUCAM_SHIELD_V2) && defined(OV2640_CAM)))
#error Please select the hardware platform and camera module in the ../libraries/ArduCAM/memorysaver.h file
#endif

const int CS = 14;

const char *ssid = "BestNetwork";         // Put your SSID here
const char *password = "samsunggalaxys2"; // Put your PASSWORD here

WebServer server(80);

ArduCAM myCAM(OV2640, CS);

void start_capture() {
    myCAM.clear_fifo_flag();
    myCAM.start_capture();
}

void camCapture(ArduCAM myCAM) {
    WiFiClient client = server.client();

    size_t len = myCAM.read_fifo_length();
    if (len >= 0x07ffff) {
        Serial.println("Over size.");
        return;
    } else if (len == 0) {
        Serial.println("Size is 0.");
        return;
    }

    myCAM.CS_LOW();
    myCAM.set_fifo_burst();
#if !(defined(ARDUCAM_SHIELD_V2) && defined(OV2640_CAM))
    SPI.transfer(0xFF);
#endif
    if (!client.connected())
        return;
    String response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: image/jpeg\r\n";
    response += "Content-Length: " + String(len) + "\r\n\r\n";
    server.sendContent(response);

    static const size_t bufferSize = 4096;
    static uint8_t buffer[bufferSize] = {0xFF};

    while (len) {
        size_t will_copy = (len < bufferSize) ? len : bufferSize;
        SPI.transferBytes(&buffer[0], &buffer[0], will_copy);
        if (!client.connected())
            break;
        client.write(&buffer[0], will_copy);
        len -= will_copy;
    }

    myCAM.CS_HIGH();
}

void serverCapture() {
    start_capture();
    Serial.println("CAM Capturing");

    int total_time = 0;

    total_time = millis();
    while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
        ;
    total_time = millis() - total_time;
    Serial.print("capture total_time used (in miliseconds):");
    Serial.println(total_time, DEC);

    total_time = 0;

    Serial.println("CAM Capture Done!");
    total_time = millis();
    camCapture(myCAM);
    total_time = millis() - total_time;
    Serial.print("send total_time used (in miliseconds):");
    Serial.println(total_time, DEC);
    Serial.println("CAM send Done!");
}

void setup() {
    esp_log_level_set("*", ESP_LOG_ERROR);
    esp_log_level_set("wifi", ESP_LOG_WARN);
    esp_log_level_set("dhcpc", ESP_LOG_INFO);
    uint8_t vid, pid;
    uint8_t temp;
#if defined(__SAM3X8E__)
    Wire1.begin();
#else
    Wire.begin();
#endif
    Serial.begin(115200);
    Serial.println("ArduCAM Start!");

    // set the CS as an output:
    pinMode(CS, OUTPUT);

    // initialize SPI:
    SPI.begin();
    SPI.setFrequency(4000000); // 4MHz

    // Check if the ArduCAM SPI bus is OK
    myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    temp = myCAM.read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55) {
        Serial.println("SPI1 interface Error!");
        while (1)
            ;
    }

    // Check if the camera module type is OV2640
    myCAM.wrSensorReg8_8(0xff, 0x01);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    if ((vid != 0x26) && ((pid != 0x41) || (pid != 0x42)))
        Serial.println("Can't find OV2640 module!");
    else
        Serial.println("OV2640 detected.");

    // Change to JPEG capture mode and initialize the OV2640 module
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
    //    myCAM.OV2640_set_JPEG_size(OV2640_320x240);
    myCAM.OV2640_set_JPEG_size(OV2640_1600x1200);
    delay(1000);
    myCAM.clear_fifo_flag();

    if (!strcmp(ssid, "SSID")) {
        Serial.println("Please set your SSID");
        while (1)
            ;
    }
    if (!strcmp(password, "PASSWORD")) {
        Serial.println("Please set your PASSWORD");
        while (1)
            ;
    }
    // Connect to WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
    Serial.println("");
    Serial.println(WiFi.localIP());

    // Start the server
    server.on("/capture", HTTP_GET, serverCapture);
    server.begin();
    Serial.println("Server started");
}

void loop() { server.handleClient(); }
