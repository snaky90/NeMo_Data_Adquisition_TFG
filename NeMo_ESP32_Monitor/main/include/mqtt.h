#ifndef __MQTT_H__
#define __MQTT_H__

#include "common.h"
#include "datastructures.h"
#include "mqtt_client.h"

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
void mqtt_event_handler_cb(esp_mqtt_event_handle_t event);
void mqtt_send_message(char* topic, char* msg);

#endif