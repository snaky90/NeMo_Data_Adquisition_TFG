#ifndef __PCNT_H__
#define __PCNT_H__

#include "common.h"
#include "driver/periph_ctrl.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "driver/pcnt.h"
#include "sys/time.h"

#define PIN_PULSE_IN_CH1 2
#define PIN_PULSE_IN_CH2 0
#define PIN_PULSE_IN_CH3 4
#define ESP_INTR_FLAG_DEFAULT 0

void task_pcnt(void *args);
void pulse_counter_init(pcnt_unit_t unit, int pulse_gpio_num);
int get_and_clear(pcnt_unit_t unit);

#endif
