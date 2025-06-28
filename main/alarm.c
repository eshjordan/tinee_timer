/* Copyright (C) 2025  Jordan Esh

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "alarm.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "portmacro.h"
#include <limits.h>
#include <stdio.h>

#define ALARM_DUTY ((1 << 13) >> 1) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define ALARM_FREQUENCY (392)       // Frequency in Hz.
#define ALARM_BEEP_FREQUENCY (1000) // Frequency in Hz.
#define ALARM_GPIO (GPIO_NUM_13)    // GPIO pin for the LEDC channel.
#define ALARM_SPEED_MODE LEDC_LOW_SPEED_MODE
#define ALARM_TIMER LEDC_TIMER_0
#define ALARM_CHANNEL LEDC_CHANNEL_0

static alarm_config_t alarm_configs[] = {
    {
        /* config_finished_working */
        .num_cycles = 4,
        .num_beeps = 3,
        .cycle_freq = 1.0,
        .beep_freq = 10.0,
        .tone_freq = ALARM_FREQUENCY,
    },
    {
        /* config_finished_resting */
        .num_cycles = 3,
        .num_beeps = 1,
        .cycle_freq = 1.0,
        .beep_freq = 2.0,
        .tone_freq = ALARM_FREQUENCY,
    },
    {
        /* config_beep */
        .num_cycles = 1,
        .num_beeps = 1,
        .cycle_freq = 10.0,
        .beep_freq = 10.0,
        .tone_freq = ALARM_BEEP_FREQUENCY,

    },
};

TaskHandle_t task_alarm_handle = NULL;

static void task_alarm(void *pvParameters);

void init_alarm() {
  // Prepare and then apply the LEDC PWM timer configuration
  ledc_timer_config_t ledc_timer = {.speed_mode = ALARM_SPEED_MODE,
                                    .duty_resolution = LEDC_TIMER_13_BIT,
                                    .timer_num = ALARM_TIMER,
                                    .freq_hz = ALARM_FREQUENCY,
                                    .clk_cfg = LEDC_AUTO_CLK};

  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  // Prepare and then apply the LEDC PWM channel configuration
  ledc_channel_config_t ledc_channel = {.speed_mode = ALARM_SPEED_MODE,
                                        .channel = ALARM_CHANNEL,
                                        .timer_sel = ALARM_TIMER,
                                        .intr_type = LEDC_INTR_DISABLE,
                                        .gpio_num = ALARM_GPIO,
                                        .duty = 0, // Set duty to 0%
                                        .hpoint = 0};

  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

  ESP_ERROR_CHECK(ledc_fade_func_install(0));

  // Create tasks for alarms
  xTaskCreate(task_alarm, "task_alarm_work", 4096, NULL, 5, &task_alarm_handle);
}

static void task_alarm(void *pvParameters) {
  for (;;) {
    uint32_t notificationValue = ALARM_MAX;
    if (pdPASS ==
        xTaskNotifyWait(0, ULONG_MAX, &notificationValue, portMAX_DELAY) && ALARM_MAX != (alarm_t)notificationValue) {
      alarm_config_t config = alarm_configs[(alarm_t)notificationValue];
      ESP_LOGI("ALARM",
               "Running alarm with %hu cycles, %hu beeps, "
               "cycle frequency: %.2f Hz, beep frequency: %.2f Hz, "
               "tone frequency: %lu Hz", config.num_cycles, config.num_beeps,
               config.cycle_freq, config.beep_freq, config.tone_freq);
      // Set the LEDC channel frequency
      ESP_ERROR_CHECK(
          ledc_set_freq(ALARM_SPEED_MODE, ALARM_TIMER, config.tone_freq));
      TickType_t last_cycle_time = xTaskGetTickCount();
      for (uint16_t i = 0; i < config.num_cycles; i++) {
        TickType_t last_beep_time = xTaskGetTickCount();
        for (uint16_t j = 0; j < config.num_beeps; j++) {
          ESP_ERROR_CHECK(ledc_set_duty_and_update(
              ALARM_SPEED_MODE, ALARM_CHANNEL, ALARM_DUTY, 0));
          vTaskDelayUntil(&last_beep_time,
                          pdMS_TO_TICKS(1000 * (1.0 / config.beep_freq)) / 2);
          ESP_ERROR_CHECK(
              ledc_set_duty_and_update(ALARM_SPEED_MODE, ALARM_CHANNEL, 0, 0));
          vTaskDelayUntil(&last_beep_time,
                          pdMS_TO_TICKS(1000 * (1.0 / config.beep_freq)) / 2);
        }
        if (i < config.num_cycles - 1) {
          vTaskDelayUntil(&last_cycle_time,
                          pdMS_TO_TICKS(1000 * (1.0 / config.cycle_freq)));
        }
      }
    }
  }
}
