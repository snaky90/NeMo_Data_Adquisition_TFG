#ifndef __METEO_H_
#define __METEO_H_

#include "common.h"
#include "bmp180.h"
#include "datastructures.h"

#define I2C_PIN_SDA 21
#define I2C_PIN_SCL 22

void meteo_init();

#endif