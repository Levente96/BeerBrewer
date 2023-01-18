 
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"

// Components
#include "app_main.h"
#include "temp_sense.h"
#include "brewr_server.h"
#include "brewr_ota_service.h"


void app_main()
{
    APP_MAIN_init();
    TEMP_SENSE_init();
    BREWR_SERVER_init();


    xTaskCreate(&APP_MAIN_start,   "main_app", 2048, NULL, 10, &APP_MAIN_th);
    xTaskCreate(      &ota_task, "ota_thread", 8192, NULL,  5,         NULL);
    //portTICK_RATE_MS);	
}
