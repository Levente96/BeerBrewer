/**
 * \file brewr_server.h
 * \author Levente Csoka (levente.csoka96@gmail.com)
 * \brief API server of the brewer
 * \version 0.1
 * \date 2022-02-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _BREWR_SERVER_H_
#define _BREWR_SERVER_H_

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_event_loop.h"
#include "tcpip_adapter.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "nvs.h"
#include "nvs_flash.h"

#include <esp_http_server.h>

#define GOT_IPV4_BIT BIT(0)
#define CONNECTED_BITS (GOT_IPV4_BIT)

extern const char           *TAG;
ip4_addr_t                  s_ip_addr;
EventGroupHandle_t          s_connect_event_group;
extern char                 *s_connection_name;
extern char                 *s_connection_passwd;
extern httpd_handle_t       server;


/**
 * \brief Server initialization
 * 
 */
void BREWR_SERVER_init(void);

// WIFI
void on_wifi_disconnect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void on_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

// SERVER
void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
extern httpd_handle_t start_webserver(void);
void stop_webserver(httpd_handle_t server);

// URI Handlers

esp_err_t version_get_handler(httpd_req_t *req);

extern httpd_uri_t version;


#endif