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

#include "face.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "portmacro.h"
#include <limits.h>
#include <stdio.h>

#define FACE_DUTY ((1 << 13) >> 1) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define FACE_FREQUENCY (50)        // Frequency in Hz.
#define FACE_GPIO (GPIO_NUM_14)    // GPIO pin for the LEDC channel.
#define FACE_SPEED_MODE LEDC_LOW_SPEED_MODE
#define FACE_TIMER LEDC_TIMER_1
#define FACE_CHANNEL LEDC_CHANNEL_1

static face_config_t face_configs[] = {
    {
        /* config_finished_working */
        .percentage = 50.0,
    },
};

TaskHandle_t task_face_handle = NULL;

static void task_face(void *pvParameters);

void init_face() {
  // Prepare and then apply the LEDC PWM timer configuration
  ledc_timer_config_t ledc_timer = {.speed_mode = FACE_SPEED_MODE,
                                    .duty_resolution = LEDC_TIMER_13_BIT,
                                    .timer_num = FACE_TIMER,
                                    .freq_hz = FACE_FREQUENCY,
                                    .clk_cfg = LEDC_AUTO_CLK};

  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  // Prepare and then apply the LEDC PWM channel configuration
  ledc_channel_config_t ledc_channel = {.speed_mode = FACE_SPEED_MODE,
                                        .channel = FACE_CHANNEL,
                                        .timer_sel = FACE_TIMER,
                                        .intr_type = LEDC_INTR_DISABLE,
                                        .gpio_num = FACE_GPIO,
                                        .duty = FACE_DUTY, // Set duty to 0%
                                        .hpoint = 0};

  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

  // ESP_ERROR_CHECK(ledc_fade_func_install(0));

  // Create tasks for alarms
  ESP_LOGI("FACE", "Init called");

  xTaskCreate(task_face, "task_alarm_work", 4096, NULL, 5, &task_face_handle);
}

static void task_face(void *pvParameters) {
  for (;;) {
    uint32_t notificationValue = FACE_MAX;
    if (pdPASS ==
            xTaskNotifyWait(0, ULONG_MAX, &notificationValue, portMAX_DELAY) &&
        FACE_MAX != (face_t)notificationValue) {
      face_config_t config = face_configs[(face_t)notificationValue];
      ESP_LOGI("FACE", "Running face with %.2f percentage", config.percentage);
      // Set the LEDC channel frequency
      // ESP_ERROR_CHECK(ledc_set_freq(FACE_SPEED_MODE, FACE_TIMER,
      // FACE_FREQUENCY));

      const uint32_t hundred_percent = (1 << 13);
      const uint32_t ten_percent = hundred_percent / 10;
      uint32_t duty = FACE_DUTY;
      const bool up = true; // Start with increasing duty cycle
      const bool down = false; // Decreasing duty cycle
      bool direction = up; // Start with increasing duty cycle
      for (;;) {
        ESP_LOGI("FACE", "Running face with %lu duty", duty);
        ESP_ERROR_CHECK(
            ledc_set_duty_and_update(FACE_SPEED_MODE, FACE_CHANNEL, duty, 0));
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
        if (direction == up) {
          duty += ten_percent; // Increase duty cycle by 10%
          if (duty >= hundred_percent) {
            direction = down; // Switch to decreasing duty cycle
          }
        } else {
          duty -= ten_percent; // Decrease duty cycle by 10%
          if (duty == 0) {
            direction = up; // Switch to increasing duty cycle
          }
        }
      }
    }
  }
}
