
#include "pcnt.h"

void IRAM_ATTR detection_isr_handler(void* args) {
    struct telemetry_message message;

    // Generacion de valores para el mensaje
    message.timestamp = xthal_get_ccount();
    message.tm_message_type = TM_PULSE_DETECTION;
    message.payload.tm_detect.channel[0] = gpio_get_level(PIN_PULSE_IN_CH1);
    message.payload.tm_detect.channel[1] = gpio_get_level(PIN_PULSE_IN_CH2);
    message.payload.tm_detect.channel[2] = gpio_get_level(PIN_PULSE_IN_CH3);

    xQueueSendFromISR(telemetry_q, &message, NULL);
}

void GPIO_init(){
    gpio_pad_select_gpio(PIN_PULSE_IN_CH1);
    gpio_pad_select_gpio(PIN_PULSE_IN_CH2);
    gpio_pad_select_gpio(PIN_PULSE_IN_CH3);

    gpio_set_direction(PIN_PULSE_IN_CH1, GPIO_MODE_INPUT);
    gpio_set_direction(PIN_PULSE_IN_CH2, GPIO_MODE_INPUT);
    gpio_set_direction(PIN_PULSE_IN_CH3, GPIO_MODE_INPUT);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    gpio_isr_handler_add(PIN_PULSE_IN_CH1, detection_isr_handler, NULL);
    gpio_isr_handler_add(PIN_PULSE_IN_CH2, detection_isr_handler, NULL);
    gpio_isr_handler_add(PIN_PULSE_IN_CH3, detection_isr_handler, NULL);

    gpio_set_intr_type(PIN_PULSE_IN_CH1, GPIO_INTR_NEGEDGE);
    gpio_set_intr_type(PIN_PULSE_IN_CH2, GPIO_INTR_NEGEDGE);
    gpio_set_intr_type(PIN_PULSE_IN_CH3, GPIO_INTR_NEGEDGE);
}

void time_sync(void *args){
    struct telemetry_message message;
    struct timeval tv_now;
    message.tm_message_type = TM_TIME_SYNC;

    while(true){
        // Calculo del tiempo actual para agregarlo al mensaje
        gettimeofday(&tv_now, NULL);
        int64_t time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
        message.payload.tm_sync.cpu_count = xthal_get_ccount();
        message.timestamp = time_us;
        xQueueSend(telemetry_q, &message, portMAX_DELAY);
    }
}