/*
 * This library builds the environment for user usage
 * Including
 * 1. Log into SD card
 * 2. Log with communication module
 *
 * Require:
 * Return:
 */

#ifndef _WIFI_COMMS_H
#define _WIFI_COMMS_H


#include <Arduino.h>
#include "configs.h"
#include "logger.h"
#ifdef USE_WIFI_COMMUNICATION
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>
#ifdef ESP_NOW
#include <espnow.h>
typedef struct payload_t {
    char message[256];
} payload_t;
payload_t data;
uint8_t sendTo[];

void onDataSend(uint8_t *mac_addr, uint8_t status);
void onDataRecv(uint8_t *mac_addr, uint8_t *payload, uint8_t length);
#endif

class wifiServer : public Logger
// Inherit from logger.h to use function of file operation
{
private:
    String getContentType(String fileName);

    // WebSocketEvent waits for webSocket client to send command
    void webSocketEvent(uint8_t num,
                        WStype_t type,
                        uint8_t *payload,
                        size_t length);

public:
    ESP8266WebServer server;
    WebSocketsServer webSocket;

    String message;

    wifiServer(void);

    bool init(const char *ssid = WIFI_SSID,
              const char *passward = WIFI_PASSWARD);

    bool handleFileRead(String path);  // Stream file for web client

    void loop();  // Put this loop to main.ino loop()
};
#endif
#endif
