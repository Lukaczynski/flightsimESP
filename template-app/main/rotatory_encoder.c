
#include "rotatory_encoder.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <inttypes.h>

#define RE_TAG "rotary_encoder"

#define RE_EVENT_QUEUE_LENGTH 1
#define RE_TABLE_ROWS 7

#define RE_DIR_NONE 0x0 // No complete step yet.
#define RE_DIR_LS 0x10  // Left.
#define RE_DIR_RS 0x20  // Right.

// Create the half-step state table (emits a code at 00 and 11)
#define RE_H_START 0x0
#define RE_H_RS_BEGIN 0x1
#define RE_H_LS_BEGIN 0x2
#define RE_H_START_M 0x3
#define RE_H_LS_BEGIN_M 0x4
#define RE_H_RS_BEGIN_M 0x5

// Create the full-step state table (emits a code at 00 only)
#define RE_V_LS_FINAL 0x1
#define RE_V_LS_BEGIN 0x2
#define RE_V_LS_NEXT 0x3
#define RE_V_RS_BEGIN 0x4
#define RE_V_RS_FINAL 0x5
#define RE_V_RS_NEXT 0x6

static void _process_task(void *arg)
{
    printf("task creado\n");
    re_conf_t * config = (re_conf_t *)arg;
    printf("task -> %d \n",config->pin_DT);
    re_event_t event;
    for (;;)
    {
        if (xQueueReceive(config->queue, &event, 10000))
        {
            printf("recibido evento de interrupcion\n");
            uint8_t t  = (uint8_t) event.input_state;
            printf("table -> %d \n", t);
        }
    }

    vTaskDelay(1000 / portTICK_RATE_MS);
    printf("task finalizado\n");
    vTaskDelete(NULL);// END OF TASK. A task function may not simply "end".
}

static void IRAM_ATTR _isr_handler(void *arg)
{
    re_conf_t *conf = (re_conf_t *)arg;
    uint8_t input_state =(gpio_get_level(conf->pin_DT) << 1) | gpio_get_level(conf->pin_CLK);
    re_event_t event = {
        .input_state = input_state};
    xQueueSendFromISR(conf->queue, &event, NULL);
}

esp_err_t re_init(re_conf_t *conf, gpio_num_t pin_CLK, gpio_num_t pin_DT)
{
    esp_err_t err = ESP_OK;
    printf("iniciando RE ...");
    if (conf)
    {
        gpio_config_t io_conf;
        //disable interrupt
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.intr_type = GPIO_INTR_ANYEDGE;
        //bit mask of the pins, use GPIO here
        io_conf.pin_bit_mask = ((1ULL << pin_CLK) | (1ULL << pin_DT));
        //set as input mode
        io_conf.mode = GPIO_MODE_INPUT;
        //enable pull-up mode
        io_conf.pull_up_en = 1;

        ESP_ERROR_CHECK(gpio_config(&io_conf));

        conf->pin_CLK = pin_CLK;
        conf->pin_DT = pin_DT;
        //create a queue to handle gpio event from isr
        //TODO: instantiate the queue from outside
        conf->queue = xQueueCreate(RE_EVENT_QUEUE_LENGTH, sizeof(re_event_t));
        

        //start gpio task
        xTaskCreate(_process_task, "_process_task", 2048, conf, 10, NULL);

        //install gpio isr service
        ESP_ERROR_CHECK(gpio_install_isr_service(0));
        //hook isr handler for specific gpio pin
        ESP_ERROR_CHECK(gpio_isr_handler_add(conf->pin_CLK, _isr_handler, conf));
        //hook isr handler for specific gpio pin
        ESP_ERROR_CHECK(gpio_isr_handler_add(conf->pin_DT, _isr_handler, conf)); //TODO: GIPO PORTS
    }
    else
    {
        err = ESP_ERR_INVALID_ARG;
    }
    return err;
}
