#include <Arduino.h>
#include <Wire.h>

#define IMU_ADDR 0x6A  

#define ACCEL_SENS  0.000244f   // g/LSB
#define GYRO_SENS   0.070f      // dps/LSB

TwoWire myWire(PA9, PF0); // Arduino pins

void imuWrite(uint8_t reg, uint8_t data) {
    myWire.beginTransmission(IMU_ADDR);  // Wire -> myWire
    myWire.write(reg);
    myWire.write(data);
    myWire.endTransmission();
}

void imuRead(uint8_t reg, uint8_t *buf, uint8_t len) {
    myWire.beginTransmission(IMU_ADDR);  // Wire -> myWire
    myWire.write(reg);
    myWire.endTransmission(false);
    myWire.requestFrom((uint8_t)IMU_ADDR, (uint8_t)len);  // Wire -> myWire
    for (uint8_t i = 0; i < len; i++) {
        buf[i] = myWire.read();  // Wire -> myWire
    }
}

void setup() {
    Serial.begin(115200);
    myWire.begin();
    pinMode(PA9, INPUT_PULLUP);
    pinMode(PF0, INPUT_PULLUP);
    myWire.setClock(400000);
    delay(10);

    Serial.println("Scanning I2C...");
    for (uint8_t addr = 1; addr < 127; addr++) {
        myWire.beginTransmission(addr);
        uint8_t err = myWire.endTransmission();
        if (err == 0) {
            Serial.print("Device found at 0x");
            Serial.println(addr, HEX);
        }
    }
    Serial.println("Scan done.");

    uint8_t who;
    imuRead(0x0F, &who, 1);
    Serial.print("WHO_AM_I = 0x");
    Serial.println(who, HEX);
    if (who != 0x71) {
        Serial.println("IMU not found");
        while (1);
    }
    Serial.println("IMU found");

    imuWrite(0x10, 0x34);  // CTRL1 - Accel: 120 Hz, high-performance
    imuWrite(0x11, 0x34);  // CTRL2 - Gyro: 120 Hz, high-performance
    imuWrite(0x15, 0x0C);  // CTRL6 - Gyro ±2000 dps
    imuWrite(0x17, 0x08);  // CTRL8 - Accel ±8g
}

void loop() {

    // uint8_t status = 0;
    // while (!(status & 0x03)) {
    //     imuRead(0x1E, &status, 1);
    // }

    // uint8_t raw[12];
    // imuRead(0x22, raw, 12);

    // int16_t gx_raw = (int16_t)(raw[1]  << 8 | raw[0]);
    // int16_t gy_raw = (int16_t)(raw[3]  << 8 | raw[2]);
    // int16_t gz_raw = (int16_t)(raw[5]  << 8 | raw[4]);
    // int16_t az_raw = (int16_t)(raw[7]  << 8 | raw[6]);
    // int16_t ay_raw = (int16_t)(raw[9]  << 8 | raw[8]);
    // int16_t ax_raw = (int16_t)(raw[11] << 8 | raw[10]);

    // float ax = ax_raw * ACCEL_SENS;
    // float ay = ay_raw * ACCEL_SENS;
    // float az = az_raw * ACCEL_SENS;
    // float gx = gx_raw * GYRO_SENS;
    // float gy = gy_raw * GYRO_SENS;
    // float gz = gz_raw * GYRO_SENS;

    // Serial.print("Accel (g):  ");
    // Serial.print(ax, 3); Serial.print("  ");
    // Serial.print(ay, 3); Serial.print("  ");
    // Serial.println(az, 3);

    // Serial.print("Gyro (dps): ");
    // Serial.print(gx, 2); Serial.print("  ");
    // Serial.print(gy, 2); Serial.print("  ");
    // Serial.println(gz, 2);

    // delay(100);
}