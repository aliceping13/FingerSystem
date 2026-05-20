#pragma once
#include <stdlib.h>
#include <Arduino.h>
#include <Wire.h>

void imuWrite(uint8_t reg, uint8_t data);

void imuRead(uint8_t reg, uint8_t *buf, uint8_t len);