
#include "sntp.h"

void show_current_time(long time, char *message){
    char strftime_buf[64];
    struct tm *timeinfo = localtime(&time);

    setenv("TZ","CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00",1); //Madrid TZ
    tzset(); 
    strftime(strftime_buf, sizeof(strftime_buf), "%c", timeinfo);
    ESP_LOGI("SNTP", "Message: %s : %s", message,strftime_buf);
}

//Recogemos el tiempo desde el servidor SNTP y almacenamos en tv (param)
void get_time_from_sntp(struct timeval *tv){
    printf("seconds %ld\n", tv-> tv_sec);
    xSemaphoreGive(sntp_semaphore);
    ESP_LOGI("SNTP","sntp_semaphore open");
}

void sntp_setup(void){
    ESP_LOGI("SNTP","Initializing configuration...");
    sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED); //Se mantiene el modo por defecto
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
    sntp_set_time_sync_notification_cb(get_time_from_sntp);
    ESP_LOGI("SNTP","Setup OK");
    xSemaphoreTake(sntp_semaphore, portMAX_DELAY);
}