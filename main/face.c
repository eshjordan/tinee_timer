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
#include "config.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include <limits.h>

#define FACE_FREQUENCY (100) // Frequency in Hz.
#define FACE_GPIO (GPIO_NUM_14)
#define FACE_SPEED_MODE LEDC_LOW_SPEED_MODE
#define FACE_TIMER LEDC_TIMER_1
#define FACE_CHANNEL LEDC_CHANNEL_1
#define FACE_DUTY_RESOLUTION LEDC_TIMER_13_BIT
#define FACE_HPOINT (0) // hpoint is the counter value where the PWM goes high

static uint32_t map_servo_duty(config_servo_t servo, float angle) {
  // Ensure angle is within bounds
  if (angle < 0) {
    angle = 0;
  } else if (angle > 360) {
    angle = 360;
  }

  // Map the angle to a duty cycle, taking into account the servo's duty range
  float percent =
      servo.min_duty + (servo.max_duty - servo.min_duty) * (angle / 360.0);

  // Convert percentage to duty cycle value
  uint32_t duty_cycle =
      (uint32_t)((percent / 100.0) * ((1 << FACE_DUTY_RESOLUTION) - 1));
  return duty_cycle;
}

TaskHandle_t task_face_handle = NULL;

static void task_face(void *pvParameters);

void init_face() {
  // Prepare and then apply the LEDC PWM timer configuration
  ledc_timer_config_t ledc_timer = {.speed_mode = FACE_SPEED_MODE,
                                    .duty_resolution = FACE_DUTY_RESOLUTION,
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
                                        .duty = 0, // Set duty to 0%
                                        .hpoint = FACE_HPOINT};

  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

  // ESP_ERROR_CHECK(ledc_fade_func_install(0));

  // Create tasks for alarms
  ESP_LOGI("FACE", "Init called");

  xTaskCreate(task_face, "task_alarm_work", 4096, NULL, 5, &task_face_handle);
}

bool set_face_angle(float angle) {
  return pdPASS ==
         xTaskNotify(task_face_handle, angle, eSetValueWithoutOverwrite);
}

bool set_face_angle_isr(float angle, bool *higherPriorityTaskWoken) {
  BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
  BaseType_t result =
      xTaskNotifyFromISR(task_face_handle, angle, eSetValueWithoutOverwrite,
                         &pxHigherPriorityTaskWoken);
  if (NULL != higherPriorityTaskWoken) {
    *higherPriorityTaskWoken = (pxHigherPriorityTaskWoken == pdTRUE);
  }
  return pdPASS == result;
}

static void task_face(void *pvParameters) {
  for (;;) {
    uint32_t notificationValue = ULONG_MAX;
    if (pdPASS ==
            xTaskNotifyWait(0, ULONG_MAX, &notificationValue, portMAX_DELAY) &&
        ULONG_MAX != (face_t)notificationValue) {
      float angle = (float)notificationValue;
      ESP_LOGI("FACE", "Running face at %f degrees", angle);
      uint32_t duty_cycle = map_servo_duty(config_servo, angle);
      // ESP_LOGI("FACE", "Running face with %lu duty", (uint32_t)duty_cycle);
      ESP_ERROR_CHECK(ledc_set_duty_and_update(
          FACE_SPEED_MODE, FACE_CHANNEL, (uint32_t)duty_cycle, FACE_HPOINT));
    }
  }
}
