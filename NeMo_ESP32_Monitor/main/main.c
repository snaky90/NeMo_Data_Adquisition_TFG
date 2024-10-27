
#include "common.h"

void app_main(void)
{

  telemetry_q = xQueueCreate(100, sizeof(struct telemetry_message));

  //Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  wifi_semaphore = xSemaphoreCreateBinary();
  sntp_semaphore = xSemaphoreCreateBinary();
  mqtt_semaphore = xSemaphoreCreateBinary();

  wifi_setup();
  sntp_setup();
  GPIO_init();

  xTaskCreatePinnedToCore(&message_sender, "Message Sender", 1024 * 3, NULL, 5, NULL, 0);
  xTaskCreatePinnedToCore(&task_pcnt, "Pulse Counter", 1024 * 3, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(&task_meteo, "Meteo Data", 1024 * 3, NULL, 5, NULL, 0);
  //xTaskCreatePinnedToCore(&task_ota, "OTA handling", 1024 * 8, NULL, 5, NULL, 0);
}
