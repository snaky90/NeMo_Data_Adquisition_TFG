#ifndef __SNTP_H__
#define __SNTP_H__

#include "common.h"
#include "esp_sntp.h"

void get_time_from_sntp(struct timeval *tv);
void show_current_time(long time, char *message);

#endif