/*
 * This library deals with the logging
 * Including
 * 1. Log into SD card or filesystem
 * 2. Log with communication module
 *
 * Require:
 * Return:
 */

#ifndef _LOGGER_H
#define _LOGGER_H

#include "Arduino.h"
#include "configs.h"
#ifdef USE_PERIPHERAL_SD_CARD
#include <SD.h>
#include <SPI.h>
#elif defined(USE_FILE_SYSTEM)
#include <FS.h>
#include <LittleFS.h>
#endif

#ifdef USE_LORA_COMMUNICATION
#include <SX126x.h>
#endif

enum LOG_LEVEL { LEVEL_DEBUG, LEVEL_INFO, LEVEL_WARNING, LEVEL_ERROR };
enum LOG_LORA_MODE { LORA_ACCEL, LORA_GYRO, LORA_COMMAND };

typedef union {
    struct {
        uint16_t id;
        int16_t data[3];
    } Packet;
    uint8_t raw[8];
} LoraPacket;

class Logger
{
private:
    String file_ext;

#ifdef USE_LORA_COMMUNICATION
    LoraPacket packet;
    uint16_t lora_packet_id;
#endif

public:
#ifdef USE_LORA_COMMUNICATION
    SX126x lora;
#endif
#ifdef USE_FILE_SYSTEM
#ifdef LITTLE_FS
    FS *filesystem = &LittleFS;
#else
    FS *filesystem = &SPIFFS;
#endif
#endif

    Logger();

    /* Open the logger file in SD card and check the ramaining size
     * Return: False if fail to initialize.
     */
    bool init();

    void lora_init();

    /* Perform logging task */
    void log(String msg, LOG_LEVEL level = LEVEL_DEBUG);

    /* Log existing error code or info code */
    void log_code(int code, LOG_LEVEL level);

    /* List file on board */
    String listFile(String path = "/");

    /* Delete file */
    bool deleteFile(const char * filename);
    bool deleteFile(String fileName);

    /* Format all filesystem */
    bool formatFS();

#ifdef USE_LORA_COMMUNICATION
    void lora_send(LOG_LORA_MODE mode, int16_t *data);
#endif
};

#endif