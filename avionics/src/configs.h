#ifndef _CONFIG_H
#define _CONFIG_H

#include "Arduino.h"


/*--------------- BOARD_VERSIOIN_SETTING ---------------*/
// #define V1_ATMEGA328P
#define V2_ESP8266
// #define V2_ESP32

/*--------------- System function enable ---------------*/
// #define USE_SERIAL_DEBUGGER
#define USE_SERIAL_COMMS
#define USE_GPS_NEO6M
#define USE_PERIPHERAL_BMP280
//#define USE_PERIPHERAL_BMP280_LIB
//#define USE_PERIPHERAL_MPU6050
#define USE_GY91_MPU9250
//#define USE_PERIPHERAL_BUZZER

#if defined(V1_ATMEGA328P) || defined(V2_ESP32)
    #define USE_PERIPHERAL_SD_CARD
    #define USE_LORA_COMMUNICATION
    #define USE_DUAL_SYSTEM_WATCHDOG
#elif defined(V2_ESP8266)
    #define USE_WIFI_COMMUNICATION 
    #define USE_FILE_SYSTEM
    // #define ESP_NOW
#endif

/*--------------------- PIN_SETTING --------------------*/
#ifdef USE_DUAL_SYSTEM_WATCHDOG
#define PIN_PARTNER_RESET 8
#endif

// Communication
// A0~A5 map to 14~19
// A0-14, A1-15, A2-16, A3-17, A4-18, A5-19
#ifdef USE_LORA_COMMUNICATION
#define PIN_LORA_INTERRUPT 3
#define PIN_LORA_BUSY A2
#define PIN_LORA_RESET A3
#define PIN_LORA_SELECT A1

#define RF_FREQUENCY 433000000  // Hz  center frequency
#define TX_OUTPUT_POWER 22      // dBm tx output power
#define LORA_BANDWIDTH \
    4  // bandwidth=125khz  0:250kHZ,1:125kHZ,2:62kHZ,3:20kHZ.... look for radio
       // line 392
#define LORA_SPREADING_FACTOR 7  // spreading factor=11 [SF5..SF12]
#define LORA_CODINGRATE \
    1  // [1: 4/5,
       //  2: 4/6,
       //  3: 4/7,
       //  4: 4/8]

#define LORA_PREAMBLE_LENGTH 8            // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT 0             // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false  // variable data payload
#define LORA_IQ_INVERSION_ON false
#define LORA_PAYLOADLENGTH \
    0  // 0: variable receive length
       // 1..255 payloadlength
#endif

#ifdef USE_DUAL_SYSTEM_WATCHDOG
#define PIN_SPI_CS_PARTNER 10
#endif

// Signal
#ifdef USE_PERIPHERAL_BUZZER
#define PIN_BUZZER 2
#endif
#ifdef V1_ATMEGA328P
#define PIN_TRIGGER 6
#define PIN_MOTOR 5
#elif defined(V2_ESP8266)
#define PIN_TRIGGER 12
#define PIN_TRIGGER_2 14
#define PIN_MOTOR 13
#elif defined(V2_ESP32)
#define PIN_TRIGGER 6
#define PIN_MOTOR 5
#endif

/*------------------ WIFI Communication ------------------*/
#define AP_AS_SERVER   // Access point as server (Sky)
#define STA_AS_SERVER  // Station as server (Ground)
#define WIFI_SSID "ESP8266_avionics"     // default ssid
#define WIFI_PASSWARD "nckuispavionics"  // default passward
#define WIFI_HOST_NAME "NCKU.ISP"        // default host name


/*------------ Configuration for parachute --------------*/
#define SERVO_INITIAL_ANGLE 180
#define SERVO_RELEASE_ANGLE 0

/*------------------ Constants for imu ------------------*/
#ifdef USE_PERIPHERAL_MPU6050
//#define USE_MPU_ISP_INTERFACE
// IMU interrupt
// Only available on specific pins,
// 328: 2, 3,
// 32u4: 0, 1, 22, 3, 7
#define PIN_IMU_INT 2
// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
#define IMU_MPU_ADDR 0x68

#ifdef USE_MPU_ISP_INTERFACE
#define PIN_SPI_CS_IMU 9
#endif
#endif

// BMP280 setting
#ifdef USE_PERIPHERAL_BMP280
//#define IMU_BMP_ADDR       0x76
// The sampling times of sea level pressure while initializing
#define IMU_BMP_SEA_LEVEL_PRESSURE_SAMPLING 50
#define IMU_BMP_SAMPLING_PERIOD 10  // ms
#endif
// Altitude setting
// tau = (-T) / log(a), with a=0.8 and T=10(ms), tau about to 103.2 (ms)
#define IMU_ALTITUDE_SMOOTHING_CONSTANT 0.0f
#define IMU_RISING_CRITERIA 10.0f
#define IMU_FALLING_CRITERIA -10.0f

/*---------------------- Data logger --------------------*/
#ifdef USE_PERIPHERAL_SD_CARD
#define LOGGER_SD_CS 10
#define LOGGER_FILENAME_BUFFER 20
#elif defined(USE_FILE_SYSTEM)
//
// Choose using SPIFF or LittleFS as filesystem engine.
// You can only choose one of these
// If you don't comment one of them. LittleFS would be chosed
//
#define LITTLE_FS   //default
// #define SPIFF
#endif
#define LOGGER_FILENAME "logger"
#define LOGGER_FILE_EXT ".txt"
#define LOGGER_LOG_INTERVAL 100

/*-------------------- Serial debugger ------------------*/
#ifdef USE_SERIAL_DEBUGGER
#define SERIAL_DEBUGGER_BAUDRATE 115200
#elif defined(USE_SERIAL_COMMS)
#define SERIAL_COMMS_BAUDRATE 9600
#ifdef ESP8266
#define GPS_RXpin 3
#define GPS_TXpin 1
#endif
#endif

/*------------------ Watchdog protection ----------------*/
#ifdef USE_DUAL_SYSTEM_WATCHDOG
#define WATCH_SPI_TIMEOUT 100  // ms
#endif

enum ERROR_CODE {
    ERROR_OK,
    ERROR_SD_INIT_FAILED,
    ERROR_MPU_INIT_FAILED,
    ERROR_DMP_INIT_FAILED,
    ERROR_BMP_INIT_FAILED,
    ERROR_IMU_INIT_FAILED
};

enum INFO_CODE {
    INFO_LOGGER_INIT,
    INFO_IMU_INIT,
    INFO_SERVO_INIT,
    INFO_ALL_SYSTEM_INIT,
    INFO_LORA_INIT
};

#endif