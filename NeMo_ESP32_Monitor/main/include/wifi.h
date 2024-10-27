#ifndef __WIFI_H__
#define __WIFI_H__

#include "common.h"
#include "esp_wifi.h"

// Definimos segun el valor recomendado en la documentacion
#define time_to_retry 10

/* Se define una fincion de 'event handler' segun la API
    void run_on_event(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data) */

static void wifi_event_handler(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data);

#endif