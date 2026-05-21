#include "imu.h"

#define IMU_ADDR 0x6B  

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
