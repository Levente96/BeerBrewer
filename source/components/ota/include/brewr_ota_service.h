#ifndef _BREWR_OTA_SERVICE_H_
#define _BREWR_OTA_SERVICE_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"

extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

extern esp_err_t _http_event_handler(esp_http_client_event_t *evt);
void ota_task(void * pvParameter);

#endif // _BREWR_OTA_SERVICE_H_