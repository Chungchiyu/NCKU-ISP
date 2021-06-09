#include "core.h"

System *sys;

bool run_mpu = false;
bool test_connection = true;
bool a = false;
bool record = false;

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(false);
    sys = new System();

    while (sys->init() != SYSTEM_READY) {
        sys->buzzer(BUZ_LEVEL2);
    }
    test_connection = false;
    // ISR_enable();
}

void loop()
{
    sys->loop();
    sys->imu.imu_isr_update();
    while (Serial.available()) {
        static String command = "";
        char c = (char) Serial.read();
        if (c != '\n')
            command += c;
        else {
            if (command.equals("list file"))
                sys->logger.listFile();
            else if (command == "format")
                sys->logger.formatFS();
            else if (command.indexOf("delete") != -1)
                sys->logger.deleteFile(command.substring(7));
            else if (command.indexOf("read") != -1)
                Serial.print(sys->logger.readFile(command.substring(5)));
            else if (command == "calibrate")
                sys->imu.init();
            else {
                Serial.println(command);
                Serial.println("command not found");
            }
            command = "";
        }
    }


    if (sys->comms.message.equals("stop mpu")) {
        run_mpu = false;
        sys->comms.message = "";
    } else if (sys->comms.message.equals("run mpu")) {
        run_mpu = true;
        sys->comms.message = "";
    } else if (sys->comms.message.equals("run test")) {
        test_connection = true;
        sys->comms.message = "";
    } else if (sys->comms.message.equals("stop test")) {
        test_connection = false;
        sys->comms.message = "";
    } else if (sys->comms.message.equals("calibrate")) {
        sys->imu.init();
        sys->comms.message = "";
    } else if (sys->comms.message.equals("record")) {
        record = true;
        sys->comms.message = "";
    } else if (sys->comms.message.equals("stop record")) {
        record = false;
        sys->comms.message = "";
    }

    if (test_connection) {
        static auto time = millis();
        if (millis() - time > 200) {
            time = millis();
            if (a) {
                sys->wifi_broadcast("1");
                a = false;
            } else {
                sys->wifi_broadcast("0");
                a = true;
            }
        }
    }
    if (abs(sys->imu.mpu.getAccelX_mss()) > 50 ||
        abs(sys->imu.mpu.getAccelY_mss()) > 50 ||
        abs(sys->imu.mpu.getAccelZ_mss()) > 50) {
        record = true;
        Serial.println("start recording");
        sys->wifi_broadcast("start recording");
    }
    if (sys->imu.pose == ROCKET_RISING) {
        // sys->buzzer(BUZ_LEVEL0);
        sys->wifi_broadcast("Rising");
        sys->logger.log("Rising");
    } else if (sys->imu.pose == ROCKET_FALLING) {
        sys->fairingOpen();
        sys->wifi_broadcast("Falling");
        // sys->buzzer(BUZ_LEVEL1);
        sys->logger.log("Falling");
    }

    // Continuous logging
    static auto last_log_time = millis();
    if (millis() - last_log_time > 50 && (run_mpu || record)) {
        sys->imu.bmp_update();
        sys->logger.log(String(sys->imu.altitude), LEVEL_INFO);
        last_log_time = millis();


// #if defined(USE_PERIPHERAL_MPU6050) || defined(USE_GY91_MPU9250)
//         if (sys->imu.imu_isr_update()) {
//             // int16_t data[3] = {sys->imu.aaWorld.x, sys->imu.aaWorld.y,
//             //                    sys->imu.aaWorld.z};
//             float data[10] = {
//                 sys->imu.mpu.getAccelX_mss(), sys->imu.mpu.getAccelY_mss(),
//                 sys->imu.mpu.getAccelZ_mss(), sys->imu.mpu.getGyroX_rads(),
//                 sys->imu.mpu.getGyroY_rads(), sys->imu.mpu.getGyroZ_rads(),
//                 sys->imu.mpu.getMagX_uT(),    sys->imu.mpu.getMagY_uT(),
//                 sys->imu.mpu.getMagZ_uT(),    sys->imu.mpu.getTemperature_C()};
// #ifdef USE_LORA_COMMUNICATION
//             sys->logger.lora_send(LORA_ACCEL, data);
// #elif defined(USE_GY91_MPU9250)
//             String payload = String(data[0]);
//             for (int i = 1; i < 10; i++) {
//                 payload += "\t" + String(data[i]);
//             }
//             sys->wifi_broadcast(payload);
//             Serial.println(payload);
// #endif
//         }
// #endif
    }
}

/*
void ISR_enable()
{
    TCCR2A = 0;
    TCCR2B = 0;
    TCCR2B |= (1 << WGM22);  // CTC mode; Clear Timer on Compare
    TCCR2B = (TCCR2B & 0b11111000) | TIMER_PRESCALER_1024;
    // TCCR2B |= (1<<CS22);
    TIMSK2 |= (1 << OCIE2A);  // enable CTC for TIMER1_COMPA_vect
    TCNT2 = 0;                // counter 歸零
    OCR2A = 156;
    // 100 Hz
}

void ISR_disable()
{
    TCCR2A = 0;
    TCCR2B = 0;
}

ISR(TIMER2_COMPA_vect)
{
#if defined(USE_PERIPHERAL_BMP280) || defined(USE_PERIPHERAL_BMP280_LIB)
    sei();
    sys->imu.bmp_update();
    cli();
#endif
}*/