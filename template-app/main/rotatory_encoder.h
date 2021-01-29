
#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <stdbool.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "driver/gpio.h"

#define TABLE_COLS 4
//typedef uint8_t table_row_t[TABLE_COLS];


typedef int32_t re_position_t;

typedef enum
{
    RE_DIRECTION_NOT_SET = 0,
    RE_DIRECTION_RIGHT,
    RE_DIRECTION_LEFT,
} re_direction_t;

typedef struct
{
    gpio_num_t pin_CLK;                       
    gpio_num_t pin_DT;                       
    QueueHandle_t queue;
    uint8_t input_state;                     
} re_conf_t;

typedef struct
{
    uint8_t input_state;  
} re_event_t;


esp_err_t re_init(re_conf_t * conf, gpio_num_t pin_CLK, gpio_num_t pin_DT);
#endif  // ROTARY_ENCODER_H