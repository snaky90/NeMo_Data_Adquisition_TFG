
#include "mqtt.h"

char *TAG = "MQTT";
char *TAG2 = "SENDER";
esp_mqtt_client_handle_t client = NULL;

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    switch (event_id){
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            xSemaphoreGive(mqtt_semaphore);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        case MQTT_EVENT_BEFORE_CONNECT:
            ESP_LOGE(TAG, "MQTT_EVENT_BEFORE_CONNECT");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

/*static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    mqtt_event_handler_cb(event);
}*/

void mqtt_setup(){
    ESP_LOGI(TAG, "MQTT INIT CLIENT");
        esp_mqtt_client_config_t mqttConfig = {
        //.uri = "mqtt://broker.emqx.io:1883"
        //.uri = CONFIG_MQTT_URI,
        .host = CONFIG_MQTT_HOST,
        .port = atoi(CONFIG_MQTT_PORT),
    };

    ESP_LOGI(TAG, "MQTT init message send");
    client = esp_mqtt_client_init(&mqttConfig);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
    return (void)client;
}

void mqtt_send_message(char* topic, char* msg) {
    esp_mqtt_client_publish(client, topic, msg, 0, 0, false);
}

void message_sender(void *args){
    //Funcion que unifica y genera le mensaje a enviar al broker
    struct telemetry_message message;

	mqtt_setup();
	mqtt_send_message(TAG2, "esp32 connected");

	while(true) {
		if (xQueueReceive(telemetry_q, &message, portMAX_DELAY)) {
		    char buffer[500];
		    switch (message.tm_message_type) {
			case TM_METEO:
                sprintf(buffer, "{ \"datetime\": %lld, \"temp_c\": %.2f, \"atmpres_pa\": %zu }",
                        message.timestamp,
                        message.payload.tm_meteo.temperature_celsius,
                        message.payload.tm_meteo.atm_pressure_hpas
                );

		        mqtt_send_message("calma/nemo/meteo", buffer);
		        ESP_LOGI(TAG2, "Publishing METEO");
			    break;

			case TM_PULSE_COUNT:
                sprintf(buffer, "{ \"datetime\": %lld, \"ch01\": %zu, \"ch02\": %zu, \"ch03\": %zu, \"Interval_s\": %zu }",
                    message.timestamp,
                    message.payload.tm_pcnt.channel[0],
                    message.payload.tm_pcnt.channel[1],
                    message.payload.tm_pcnt.channel[2],
                    message.payload.tm_pcnt.integration_time_sec
                );

		        mqtt_send_message("calma/nemo/pulses", buffer);
		        ESP_LOGI(TAG2, "Publishing PULSECOUNT");
			    break;

            case TM_PULSE_DETECTION:
                sprintf(buffer, "{ \"datetime\": %lld, \"ch01\": %zu, \"ch02\": %zu, \"ch03\": %zu }",
                    message.timestamp,
                    message.payload.tm_detect.channel[0],
                    message.payload.tm_detect.channel[1],
                    message.payload.tm_detect.channel[2]
                );

		        mqtt_send_message("calma/nemo/detection", buffer);
		        //ESP_LOGI(TAG2, "Publishing DETECTOR");
			    break;


            case TM_TIME_SYNC:
                sprintf(buffer, "{ \"datetime\": %lld, \"cpu_lnd\": %zu }",
                    message.timestamp,
                    message.payload.tm_sync.cpu_count
                );
                mqtt_send_message("calma/nemo/sync", buffer);
                ESP_LOGI(TAG2, "sYNC TIME");
                break;

			default:
			    break;
		    }
		}
	}
}

