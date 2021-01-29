
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <inttypes.h>
#include "rotatory_encoder.h"

/**
 * Rotatory encoder parameters
 */

#define GPIO_INPUT_IO_CLK 32 // CLK
#define GPIO_INPUT_IO_DT 33 // DT



void app_main(void)
{
 

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());


    re_conf_t re_conf = {0};

    ESP_ERROR_CHECK(re_init(&re_conf, GPIO_INPUT_IO_CLK,GPIO_INPUT_IO_DT));
  
    vTaskDelete(NULL);// END OF TASK. A task function may not simply "end".
}
