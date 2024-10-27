
#include "pcnt.h"

char *TAG_PULSE_COUNTER= "PULSE_COUNTER";
int32_t count[3]={0};

void pulse_counter_init(pcnt_unit_t unit, int pulse_gpio_num) {
    /* Prepare configuration for the PCNT unit */
    pcnt_config_t pcnt_config = {
        // Set PCNT input signal and control GPIOs
        .pulse_gpio_num = pulse_gpio_num,
        .ctrl_gpio_num = PCNT_PIN_NOT_USED,
        .channel = PCNT_CHANNEL_0,
        .unit = unit,
        // What to do on the positive / negative edge of pulse input?
        .pos_mode = PCNT_COUNT_DIS,   // Count up on the positive edge
        .neg_mode = PCNT_COUNT_INC,   // Keep the counter value on the negative edge
        // What to do when control input is low or high?
        .lctrl_mode = PCNT_MODE_KEEP, // Keep counting direction if low
        .hctrl_mode = PCNT_MODE_KEEP, // Keep the primary counter mode if high
    };

    /* Initialize PCNT unit */
    pcnt_unit_config(&pcnt_config);

    /* Configure and enable the input filter */
    /* Units are in APB_CLOCK 80Mhz ticks. Each tick is aprox 13ns */
    pcnt_set_filter_value(PCNT_UNIT_0, 100);
    pcnt_filter_enable(PCNT_UNIT_0);

    /* Initialize PCNT's counter */
    pcnt_counter_pause(PCNT_UNIT_0);
    pcnt_counter_clear(PCNT_UNIT_0);

    /* Everything is set up, now go to counting */
    pcnt_counter_resume(PCNT_UNIT_0);
}

int get_and_clear(pcnt_unit_t unit) {
    int16_t count=0;
    pcnt_get_counter_value(unit, &count);
    pcnt_counter_clear(unit);
    return count;
}

void task_pcnt(void *args) {

    //int16_t count=0;
    int integration_time_secs = 10;
    time_t now, time_to_sleep;
    struct telemetry_message message;
    struct timeval tv_now;

    pulse_counter_init(PCNT_UNIT_0, PIN_PULSE_IN_CH1);
    pulse_counter_init(PCNT_UNIT_1, PIN_PULSE_IN_CH2);
    pulse_counter_init(PCNT_UNIT_2, PIN_PULSE_IN_CH3);

    message.tm_message_type = TM_PULSE_COUNT;

    while (true)
    {
        count[0] = get_and_clear(PCNT_UNIT_0);
        ESP_LOGI(TAG_PULSE_COUNTER, "CH1: %d pulses per %d secs", count[0], integration_time_secs);
        message.payload.tm_pcnt.channel[0] = count[0];
        count[1] = get_and_clear(PCNT_UNIT_1);
        ESP_LOGI(TAG_PULSE_COUNTER, "CH2: %d pulses per %d secs", count[1], integration_time_secs);
        message.payload.tm_pcnt.channel[1] = count[1];
        count[2] = get_and_clear(PCNT_UNIT_2);
        ESP_LOGI(TAG_PULSE_COUNTER, "CH3: %d pulses per %d secs", count[2], integration_time_secs);
        message.payload.tm_pcnt.channel[2] = count[2];

        message.payload.tm_pcnt.integration_time_sec = integration_time_secs;

        // Calculo del tiempo actual para agregarlo al mensaje
        gettimeofday(&tv_now, NULL);
        int64_t time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
        message.timestamp = time_us;
        xQueueSend(telemetry_q, &message, portMAX_DELAY);

        time(&now);
        time_to_sleep = integration_time_secs - now % integration_time_secs;
        ESP_LOGI(TAG_PULSE_COUNTER, "Sleeping for %ld s", time_to_sleep);
        vTaskDelay(time_to_sleep * 1000 / portTICK_RATE_MS);
    }
}

