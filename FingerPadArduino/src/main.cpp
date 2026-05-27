#include "imu.h"

#define ACCEL_SENS  0.000244f   // g/LSB
#define GYRO_SENS   0.070f      // dps/LSB

// Look at these: https://primalcortex.wordpress.com/2022/12/02/some-stm32-arduino-framework-tips-for-i2c-and-sh1106-oleds/
// https://community.st.com/t5/others-stm32-mcus-related/nucleo-g474re-arduino-i2c-not-working/td-p/98317
// https://www.saleae.com/support/tutorials-learning/example-projects/how-to-analyze-i2c

TwoWire myWire(PB7, PA15); // Arduino pins (SDA, SCL)

void setup() {
    
    Serial.begin(115200);
    Serial.println("start");
    delay(1000);

    myWire.begin();
    Serial.println("wire began");
    myWire.setClock(400000);
    Serial.println("clock set");

    for (uint8_t addr = 1; addr < 127; addr++) {
        myWire.beginTransmission(addr);
        if (myWire.endTransmission() == 0) {
            Serial.print("Device found at 0x");
            Serial.println(addr, HEX);
        }
    }

    myWire.beginTransmission(0x6B);
    myWire.write(0x0F);
    myWire.endTransmission(false);
    myWire.requestFrom((uint8_t)0x6B, (uint8_t)1);
    Serial.println(myWire.available());
    Serial.println(myWire.read(), HEX);

    // uint8_t who;
    // imuRead(0x0F, &who, 1);
    // Serial.print("WHO_AM_I = 0x");
    // Serial.println(who, HEX);
    // if (who != 0x71) {
    //     Serial.println("IMU not found");
    //     while (1);
    // }
    // Serial.println("IMU found");

    imuWrite(0x03, 0x00);  // IF_CFG - I2C enabled, active-high interrupts, push-pull, no internal pull-ups
    imuWrite(0x10, 0x30);  // CTRL1 - Accel: 120 Hz, high-performance (OP_MODE_XL=000)
    imuWrite(0x11, 0x30);  // CTRL2 - Gyro: 120 Hz, high-performance (OP_MODE_G=000)
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