/**
 * Simple example with STS3x sensor.
 *
 * It shows different user task implementations in *single shot mode* and
 * *periodic mode*. In *single shot* mode either low level or high level
 * functions are used.
 */

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <sts3x.h>
#include <string.h>
#include <esp_err.h>

/* float is used in printf(). you need non-default configuration in
 * sdkconfig for ESP8266, which is enabled by default for this
 * example. see sdkconfig.defaults.esp8266
 */

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

static sts3x_t dev;

#if defined(CONFIG_EXAMPLE_STS3X_DEMO_HL)

/*
 * User task that triggers a measurement every 5 seconds. Due to power
 * efficiency reasons it uses *single shot* mode. In this example it uses the
 * high level function *sts3x_measure* to perform one measurement in each cycle.
 */
void task(void *pvParameters)
{
    float temperature;

    TickType_t last_wakeup = xTaskGetTickCount();

    while (1)
    {
        // perform one measurement and do something with the results
        ESP_ERROR_CHECK(sts3x_measure(&dev, &temperature));
        printf("STS3x Sensor: %.2f °C\n", temperature);

        // wait until 5 seconds are over
        vTaskDelayUntil(&last_wakeup, pdMS_TO_TICKS(5000));
    }
}

#elif defined(CONFIG_EXAMPLE_STS3X_DEMO_LL)

/*
 * User task that triggers a measurement every 5 seconds. Due to power
 * efficiency reasons it uses *single shot* mode. In this example it starts the
 * measurement, waits for the results and fetches the results using separate
 * functions
 */
void task(void *pvParameters)
{
    float temperature;

    TickType_t last_wakeup = xTaskGetTickCount();

    // get the measurement duration for high repeatability;
    uint8_t duration = sts3x_get_measurement_duration(STS3X_HIGH);

    while (1)
    {
        // Trigger one measurement in single shot mode with high repeatability.
        ESP_ERROR_CHECK(sts3x_start_measurement(&dev, STS3X_SINGLE_SHOT, STS3X_HIGH));

        // Wait until measurement is ready (constant time of at least 30 ms
        // or the duration returned from *sts3x_get_measurement_duration*).
        vTaskDelay(duration);

        // retrieve the values and do something with them
        ESP_ERROR_CHECK(sts3x_get_results(&dev, &temperature));
        printf("STS3x Sensor: %.2f °C\n", temperature);

        // wait until 5 seconds are over
        vTaskDelayUntil(&last_wakeup, pdMS_TO_TICKS(5000));
    }
}

#else // CONFIG_STS3X_DEMO_PERIODIC
/*
 * User task that fetches latest measurement results of sensor every 2
 * seconds. It starts the STS3x in periodic mode with 1 measurements per
 * second (*STS3X_PERIODIC_1MPS*).
 */
void task(void *pvParameters)
{
    float temperature;
    esp_err_t res;

    // Start periodic measurements with 1 measurement per second.
    ESP_ERROR_CHECK(sts3x_start_measurement(&dev, STS3X_PERIODIC_1MPS, STS3X_HIGH));

    // Wait until first measurement is ready (constant time of at least 30 ms
    // or the duration returned from *sts3x_get_measurement_duration*).
    vTaskDelay(sts3x_get_measurement_duration(STS3X_HIGH));

    TickType_t last_wakeup = xTaskGetTickCount();

    while (1)
    {
        // Get the values and do something with them.
        if ((res = sts3x_get_results(&dev, &temperature)) == ESP_OK)
            printf("STS3x Sensor: %.2f °C\n", temperature);
        else
            printf("Could not get results: %d (%s)", res, esp_err_to_name(res));

        // Wait until 2 seconds (cycle time) are over.
        vTaskDelayUntil(&last_wakeup, pdMS_TO_TICKS(2000));
    }
}

#endif

void app_main()
{
    ESP_ERROR_CHECK(i2cdev_init());
    memset(&dev, 0, sizeof(sts3x_t));

    ESP_ERROR_CHECK(sts3x_init_desc(&dev, CONFIG_EXAMPLE_STS3X_ADDR, 0, CONFIG_EXAMPLE_I2C_MASTER_SDA, CONFIG_EXAMPLE_I2C_MASTER_SCL));
    ESP_ERROR_CHECK(sts3x_init(&dev));

    xTaskCreatePinnedToCore(task, "sts31_test", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);
}
