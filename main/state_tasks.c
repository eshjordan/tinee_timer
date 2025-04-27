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

#include "config.h"
#include "driver/gptimer.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "statemachine.h"
#include <stdint.h>

static uint16_t get_remaining_minutes(gptimer_handle_t timer) {
  uint32_t resolution = 0;
  ESP_ERROR_CHECK(gptimer_get_resolution(timer, &resolution));

  uint64_t timer_value = 0;
  ESP_ERROR_CHECK(gptimer_get_raw_count(timer, &timer_value));

  uint32_t remaining_seconds = (uint32_t)(timer_value / resolution);
  uint16_t remaining_minutes = remaining_seconds / 60;

  // Ceiling the minutes value
  if (remaining_seconds > 0) {
    remaining_minutes += 1;
  }

  return remaining_minutes;
}

void task_state_none(void *pvParameters) {
  for (;;) {
    // Display working timer config duration (in minutes) on 7-segment display
    uint16_t minutes = config_work.timer_duration.tv_sec / 60;

    vTaskSuspendAll();
    tm1637_set_number(config_7seg, minutes);
    xTaskResumeAll();

    vTaskSuspend(NULL);
  }
}

void task_state_working(void *pvParameters) {
  for (;;) {
    // Display working timer value (in minutes) on 7-segment display
    uint16_t remaining_minutes =
        get_remaining_minutes(config_work.timer_handle);

    vTaskSuspendAll();
    tm1637_set_number(config_7seg, remaining_minutes);
    xTaskResumeAll();

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void task_state_resting(void *pvParameters) {
  for (;;) {
    // Display resting timer value (in minutes) on 7-segment display
    uint16_t remaining_minutes =
        get_remaining_minutes(config_rest.timer_handle);

    vTaskSuspendAll();
    tm1637_set_number(config_7seg, remaining_minutes);
    xTaskResumeAll();

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void task_state_paused_working(void *pvParameters) {
  for (;;) {
    // Flash 7-segment display with working timer value (in minutes)
    uint16_t remaining_minutes =
        get_remaining_minutes(config_work.timer_handle);

    vTaskSuspendAll();
    tm1637_set_number(config_7seg, remaining_minutes);
    xTaskResumeAll();

    vTaskDelay(pdMS_TO_TICKS(500));

    vTaskSuspendAll();
    tm1637_set_segment_raw(config_7seg, 0, 0x00);
    tm1637_set_segment_raw(config_7seg, 1, 0x00);
    tm1637_set_segment_raw(config_7seg, 2, 0x00);
    tm1637_set_segment_raw(config_7seg, 3, 0x00);
    xTaskResumeAll();

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void task_state_paused_resting(void *pvParameters) {
  for (;;) {
    // Flash 7-segment display with resting timer value (in minutes)
    uint16_t remaining_minutes =
        get_remaining_minutes(config_rest.timer_handle);

    vTaskSuspendAll();
    tm1637_set_number(config_7seg, remaining_minutes);
    xTaskResumeAll();

    vTaskDelay(pdMS_TO_TICKS(500));

    vTaskSuspendAll();
    tm1637_set_segment_raw(config_7seg, 0, 0x00);
    tm1637_set_segment_raw(config_7seg, 1, 0x00);
    tm1637_set_segment_raw(config_7seg, 2, 0x00);
    tm1637_set_segment_raw(config_7seg, 3, 0x00);
    xTaskResumeAll();

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void task_state_finished_working(void *pvParameters) {
  for (;;) {
    // Show 0 on 7-segment display
    vTaskSuspendAll();
    tm1637_set_number(config_7seg, 0);
    xTaskResumeAll();

    // Beep buzzer

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void task_state_finished_resting(void *pvParameters) {
  for (;;) {
    // Show 0 on 7-segment display
    vTaskSuspendAll();
    tm1637_set_number(config_7seg, 0);
    xTaskResumeAll();

    // Beep buzzer

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void task_state_set_working(void *pvParameters) {
  uint8_t show_dot = 1;
  const uint8_t loop_period_ms = 10;
  const uint16_t dot_flash_period_ms = 500;
  uint8_t counter = dot_flash_period_ms / loop_period_ms;

  for (;;) {
    // Display working timer config duration (in minutes) on 7-segment display
    uint16_t minutes = config_work.timer_duration.tv_sec / 60;

    if (counter == 0) {
      // Flash the dot
      show_dot ^= 1;
      counter = dot_flash_period_ms / loop_period_ms;
    }

    vTaskSuspendAll();
    tm1637_set_number_lead_dot(config_7seg, minutes, true, 0b0001 & show_dot);
    xTaskResumeAll();

    counter--;

    vTaskDelay(pdMS_TO_TICKS(loop_period_ms));
  }
}

void task_state_set_resting(void *pvParameters) {
  uint8_t show_dot = 1;
  const uint8_t loop_period_ms = 10;
  const uint16_t dot_flash_period_ms = 500;
  uint8_t counter = dot_flash_period_ms / loop_period_ms;

  for (;;) {
    // Display resting timer config duration (in minutes) on 7-segment display
    uint16_t minutes = config_rest.timer_duration.tv_sec / 60;

    if (counter == 0) {
      // Flash the dot
      show_dot ^= 1;
      counter = dot_flash_period_ms / loop_period_ms;
    }

    vTaskSuspendAll();
    tm1637_set_number_lead_dot(config_7seg, minutes, true, 0b0001 & show_dot);
    xTaskResumeAll();

    counter--;

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void task_state_reset(void *pvParameters) {
  for (;;) {
    // Do nothing, immediately transition to STATE_NONE (suspends this task)
    transition_to_state(STATE_NONE);
  }
}
