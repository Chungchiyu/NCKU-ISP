#include "logger.h"

Logger::Logger()
#ifdef USE_LORA_COMMUNICATION
    : lora(PIN_LORA_SELECT,    // Port-Pin Output: SPI select
           PIN_LORA_RESET,     // Port-Pin Output: Reset
           PIN_LORA_BUSY,      // Port-Pin Input:  Busy
           PIN_LORA_INTERRUPT  // Port-Pin Input:  Interrupt DIO1
           )
#endif
{
#ifdef USE_LORA_COMMUNICATION
    lora_packet_id = 0;
#endif
}

bool Logger::init()
{
#ifdef USE_SERIAL_DEBUGGER
    Serial.begin(SERIAL_DEBUGGER_BAUDRATE);
#endif

#ifdef USE_PERIPHERAL_SD_CARD
    if (!SD.begin(LOGGER_SD_CS)) {
        // SD card initialization failed
        // log_error(ERROR_SD_INIT_FAILED);
        log_code(ERROR_SD_INIT_FAILED, LEVEL_ERROR);
        // DOTO: Check remaining size
        return false;
    }

    // Check whether the filename is unique
    String filename = LOGGER_FILENAME;
    String extension = LOGGER_FILE_EXT;
    file_ext = filename + extension;
    for (int i = 0; SD.exists(file_ext); i++)
        file_ext = filename + String(i) + extension;

#elif defined(USE_FILE_SYSTEM)
    if (!filesystem->begin()) {
        Serial.println("LittleFS mount failed");
        return false;
    }

    // Check whether the filename is unique
    String fileName = LOGGER_FILENAME;
    String extension = LOGGER_FILE_EXT;
    file_ext = fileName + extension;
    for (int i = 0; filesystem->exists(file_ext); i++)
        file_ext = fileName + String(i) + extension;
#endif
    return true;
}

void Logger::lora_init()
{
#ifdef USE_LORA_COMMUNICATION
    lora.begin(SX126X_PACKET_TYPE_LORA,  // LoRa or FSK, FSK
                                         // currently not supported
               RF_FREQUENCY,             // frequency in Hz
               -3);                      // tx power in dBm

    lora.LoRaConfig(LORA_SPREADING_FACTOR, LORA_BANDWIDTH, LORA_CODINGRATE,
                    LORA_PREAMBLE_LENGTH, LORA_PAYLOADLENGTH,
                    false,   // crcOn
                    false);  // invertIrq
#endif
}

void Logger::log(String msg, LOG_LEVEL level)
{
    // Adding prefix message
    String prefix = "";
    switch (level) {
    case LEVEL_DEBUG:
        prefix = "D:";
        break;
    case LEVEL_INFO:
        prefix = "I:";
        break;
    case LEVEL_WARNING:
        prefix = "W:";
        break;
    case LEVEL_ERROR:
        prefix = "E:";
        break;
    }
    prefix += String(millis()) + ',';

#ifdef USE_PERIPHERAL_SD_CARD
    // Create file if it is not exist
    File sd;
    sd = SD.open(file_ext, FILE_WRITE);
    if (sd) {
        sd.println(prefix + msg);
        sd.close();
    }
#elif defined(USE_FILE_SYSTEM)
    // Open for appending (writing at end of file).
    // The file is created if it does not exist.
    // The stream is positioned at the end of the file.
    File f = filesystem->open(file_ext, "a");
    if (!f) {
        Serial.println("Failed to open file for appending");
        return;
    }
    f.write(prefix + msg + String('\n'));
    f.close();
#endif


#ifdef USE_SERIAL_DEBUGGER
    Serial.println(prefix + msg);
#endif
}

void Logger::log_code(int code, LOG_LEVEL level)
{
    log(String(code), LEVEL_ERROR);
}

// void Logger::handleFileList(String path = "/")
// {
//     // Assuming there are no subdirectories
//     Dir dir = LittleFS.openDir(path);
//     String output = "[";
//     while (dir.next()) {
//         File entry = dir.openFile("r");
//         // Separate by comma if there are multiple files
//         if (output != "[")
//             output += ",";
//         output += String(entry.name()).substring(0);
//         entry.close();
//     }
//     output += "]";
//     Serial.println(output);
// }

#ifdef USE_LORA_COMMUNICATION
void Logger::lora_send(LOG_LORA_MODE mode, int16_t *data)
{
    packet.Packet.id = mode << 12 | lora_packet_id;
    memcpy(packet.Packet.data, data, sizeof(int16_t) * 3);

    lora.Send(packet.raw, 8, SX126x_TXMODE_SYNC);
    lora_packet_id++;
    lora_packet_id &= 0x0FFF;
}
#endif