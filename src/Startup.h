#ifndef STARTUP_H
#define STARTUP_H

#include <Arduino.h>
#include <WiFi.h>

/**
 * @brief Connects the device to a WiFi network using predefined credentials.
 *
 * Configures the device to operate in station mode and attempts to connect to
 * the specified WiFi network using the SSID and password defined in `env.h`.
 * Waits for a connection or times out after a predefined period.
 *
 * @return `true` if the connection is successful, `false` otherwise.
 */
bool connectToWiFi();

/**
 * @brief Initializes all startup procedures for the device.
 *
 * Executes a series of initialization steps including setting up the logger,
 * configuring GPIO pins, connecting to WiFi, initializing the camera and
 * servos, mounting the SPIFFS file system, starting the web server, and
 * performing a health check to ensure all subsystems are operational.
 */
void initializeStartup();

/**
 * @brief Performs a health check by sending an HTTP request to the server's
 * health-check endpoint.
 *
 * Sends an HTTP GET request to the `/health-check` endpoint and verifies that
 * the server responds with a valid status indicating it is running correctly.
 * Retries the health check a specified number of times before determining
 * failure.
 *
 * @return `true` if the health check is successful, `false` otherwise.
 */
bool performHealthCheck();

#endif // STARTUP_H
