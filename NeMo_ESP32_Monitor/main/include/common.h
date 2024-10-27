#ifndef __COMMON_H__
#define __COMMON_H__

#include "esp32.h"
#include "datastructures.h"

//Semaforos de FreeRTOS
xSemaphoreHandle sntp_semaphore;
xSemaphoreHandle wifi_semaphore;
xSemaphoreHandle mqtt_semaphore;

//Cola para la telemetria
xQueueHandle telemetry_q;

//Task
void task_meteo(void *args);
void message_sender(void *args);
void task_pcnt(void *args);
//void task_ota(void *args);


//Funciones de inicio de caracteristicas de la placa
void GPIO_init(void);
void sntp_setup(void);
void wifi_setup(void);

#endif