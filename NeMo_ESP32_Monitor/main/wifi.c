/* Codifo fuente para la configuracion del wifi de la placa
Basado en station_example_main.c de Espressif */

#include "wifi.h"

static void wifi_event_handler(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data){
    switch (id)
    {
    case WIFI_EVENT_STA_START:{
        ESP_LOGI("WRL-WIFI","Iniciado");
        esp_wifi_connect();
        break;
    }
    case WIFI_EVENT_STA_CONNECTED:{
        ESP_LOGI("WRL-WIFI","Conectado");
        break;
    }
    case IP_EVENT_STA_GOT_IP:{
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI("WRL-WIFI","IP(" IPSTR ")", IP2STR(&event->ip_info.ip));
        xSemaphoreGive(wifi_semaphore);
        ESP_LOGI("WRL-WIFI","IP Recibida. Semaforo abierto.");
        break;
    }
    case WIFI_EVENT_STA_DISCONNECTED:{
        // Se reiniciara la placa para volver a conectar
        ESP_LOGI("WRL-WIFI","Intentando reconectar...");
        vTaskDelay(time_to_retry*1000/portTICK_RATE_MS);
        esp_restart();
        break;
    }
    default:{
        printf("id: %d", id);
        break;
    }
}
    
    //Realizamos las acciones en funcion del evento
    /*if (id == WIFI_EVENT_STA_START){
        ESP_LOGI("WRL-WIFI","Iniciado");
        esp_wifi_connect();
    }else if (id == WIFI_EVENT_STA_CONNECTED){
        ESP_LOGI("WRL-WIFI","Conectado");
    }else if(id == IP_EVENT_STA_GOT_IP){
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI("WRL-WIFI","IP(" IPSTR ")", IP2STR(&event->ip_info.ip));
        xSemaphoreGive(wifi_semaphore);
        ESP_LOGI("WRL-WIFI","IP Recibida. Semaforo abierto.");
    }else if (id == WIFI_EVENT_STA_DISCONNECTED){
        // Se reiniciara la placa para volver a conectar
        ESP_LOGI("WRL-WIFI","Intentando reconectar...");
        vTaskDelay(time_to_retry*1000/portTICK_RATE_MS);
        esp_restart();
    }else{
        printf("id: %d", id);
    }*/
}

void wifi_setup(void){
    ESP_LOGI("WRL-WIFI","Iniciando configuracion...");

    //Inicialización segun se declara en la API para crear la estacion wifi
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT,ESP_EVENT_ANY_ID,wifi_event_handler,NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT,IP_EVENT_STA_GOT_IP,wifi_event_handler,NULL));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        }
	};

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
	ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI("WRL-WIFI", "Conexión wifi configurada con exito.");
    xSemaphoreTake(wifi_semaphore, portMAX_DELAY);
}