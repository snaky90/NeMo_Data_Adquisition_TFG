#ifndef __ESP32_H__
#define __ESP32_H__

// Genericas de C
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "nvs_flash.h"

//Especificas del sistema FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_https_ota.h"
#include "esp_http_client.h"

//Especificas para componentes ESP32
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"

//Especifica de la configuracion del proyecto IDF
#include "sdkconfig.h"

#endif