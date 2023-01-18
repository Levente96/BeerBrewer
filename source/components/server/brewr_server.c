#include "brewr_server.h"

const char           *TAG                    = "WIFI";
char                 *s_connection_name      = "OnePlus 7 Pro";
char                 *s_connection_passwd    = "almafa37";
httpd_handle_t       server                  = NULL;

httpd_uri_t version_uri = {
    .uri       = "/version",
    .method    = HTTP_GET,
    .handler   = version_get_handler,
    .user_ctx  = "VERSION 0.2"
};

httpd_uri_t setup_uri = {
    .uri       = "/set",
    .method    = HTTP_GET,
    .handler   = setup_get_handler,
    .user_ctx  = "OK"
};

void BREWR_SERVER_init(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Connect to wifi
    s_connect_event_group = xEventGroupCreate();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = { 0 };

    strncpy((char *)&wifi_config.sta.ssid, s_connection_name, 14);
    strncpy((char *)&wifi_config.sta.password, s_connection_passwd, 9);

    ESP_LOGI(TAG, "Connecting to %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());

    xEventGroupWaitBits(s_connect_event_group, CONNECTED_BITS, true, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Connected to %s", s_connection_name);
    ESP_LOGI(TAG, "IPv4 address: " IPSTR, IP2STR(&s_ip_addr));


    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

    server = start_webserver();

}

void on_wifi_disconnect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    system_event_sta_disconnected_t *event = (system_event_sta_disconnected_t *)event_data;

    ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
    if (event->reason == WIFI_REASON_BASIC_RATE_NOT_SUPPORT) {
        /*Switch to 802.11 bgn mode */
        esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
    }
    ESP_ERROR_CHECK(esp_wifi_connect());
}

void on_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    memcpy(&s_ip_addr, &event->ip_info.ip, sizeof(s_ip_addr));
    xEventGroupSetBits(s_connect_event_group, GOT_IPV4_BIT);
}

void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

void disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &version_uri);
        httpd_register_uri_handler(server, &setup_uri);
        /*
        httpd_register_uri_handler(server, &hello);
        httpd_register_uri_handler(server, &echo);
        httpd_register_uri_handler(server, &ctrl);*/
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

esp_err_t version_get_handler(httpd_req_t *req)
{
    const char* resp_str = (const char*) req->user_ctx;
    httpd_resp_send(req, resp_str, strlen(resp_str));
    return ESP_OK;
}

esp_err_t setup_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;
    uint8_t interval_index = 11;
    uint8_t active_index = 11;
    interval temp;
    temp.temp = 0;
    temp.length = 0;
    temp.duration = 0;

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[32];
            if (httpd_query_key_value(buf, "index", param, sizeof(param)) == ESP_OK) {
                interval_index = (uint8_t)atoi(param);
            }
            if (httpd_query_key_value(buf, "temp", param, sizeof(param)) == ESP_OK) {
                temp.temp = (uint8_t)atoi(param);
            }
            if (httpd_query_key_value(buf, "length", param, sizeof(param)) == ESP_OK) {
                temp.length = (uint32_t)atoi(param);
                temp.duration = temp.length+1;
            }
            if (httpd_query_key_value(buf, "setactive", param, sizeof(param)) == ESP_OK) {
                active_index = (uint8_t)atoi(param);
            }
            if (httpd_query_key_value(buf, "start", param, sizeof(param)) == ESP_OK) {
                APP_MAIN_paused = 0u;
            }
        }
        free(buf);
    }
    if(interval_index < 10 )
    {
        APP_MAIN_intervals[interval_index] = temp;
        ESP_LOGI(TAG, "New temp: %d", temp.temp);
        ESP_LOGI(TAG, "New length: %d", temp.length);
        ESP_LOGI(TAG, "New index: %d", interval_index);
    }
    if(active_index < 10)
    {
        APP_MAIN_cIntIndex = active_index;
    }

    const char* resp_str = (const char*) req->user_ctx;
    httpd_resp_send(req, resp_str, strlen(resp_str));
    return ESP_OK;
}
