 
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"

// Components
#include "brewr_hal.h"
//#include "temp_regulator.h"

void app_main()
{
    hal_sys_init();

    tmp_reg_state_machine();
    
    for (;;)
    {
    	printf(".\n");
        vTaskDelay(1000 / portTICK_RATE_MS);
    }	
}
