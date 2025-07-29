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

#include "esp_log.h"
#include "face.h"
#include "freertos/FreeRTOS.h"

#include "config.h"
#include "driver/gptimer.h"
#include "esp_err.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "statemachine.h"
#include "wrapper_7seg.h"
#include <stdint.h>

static const char *TAG = "STATE TASKS";

static uint32_t get_remaining_seconds(gptimer_handle_t timer) {
  uint32_t resolution = 0;
  ESP_ERROR_CHECK(gptimer_get_resolution(timer, &resolution));

  uint64_t timer_value = 0;
  ESP_ERROR_CHECK(gptimer_get_raw_count(timer, &timer_value));

  return (uint32_t)(timer_value / resolution);
}

static uint16_t get_remaining_minutes(gptimer_handle_t timer) {
  return get_remaining_seconds(timer) / 60;
}

static uint16_t get_remaining_minutes_ceil(gptimer_handle_t timer) {
  uint32_t remaining_seconds = get_remaining_seconds(timer);
  uint16_t remaining_minutes = remaining_seconds / 60;

  // Ceiling the minutes value
  if (remaining_seconds > 0) {
    remaining_minutes += 1;
  }

  return remaining_minutes;
}

static void set_face_time(config_timer_t timer_config) {
  double max_seconds = (double)timer_config.timer_duration.tv_sec;
  double remaining_seconds =
      (double)get_remaining_seconds(timer_config.timer_handle);

  float percentage = (max_seconds - remaining_seconds) / max_seconds * 100.0;
  float angle = (percentage / 100.0) * 360.0;

  if (timer_config.count_direction == COUNT_DIRECTION_UP) {
    angle = 360.0 - angle; // Invert angle for counting up
  }

  ESP_LOGI("TASKS", "Requesting face at %f degrees, max_secs: %lf, remaining_secs: %lf", angle, max_seconds, remaining_seconds);

  ESP_ERROR_CHECK(pdPASS == set_face_angle(angle) ? ESP_OK : ESP_FAIL);
}

void task_state_none(void *pvParameters) {
  vTaskDelay(pdMS_TO_TICKS(500));
  for (;;) {
    // Display working timer config duration (in minutes) on 7-segment display
    uint16_t minutes = config_work.timer_duration.tv_sec / 60;

    set_number_7seg(minutes);

    ESP_ERROR_CHECK(pdPASS == set_face_angle(0.0) ? ESP_OK : ESP_FAIL);

    vTaskSuspend(NULL);
  }
}

void task_state_working(void *pvParameters) {
  for (;;) {
    // Display working timer value (in minutes) on 7-segment display
    uint16_t remaining_minutes =
        get_remaining_minutes_ceil(config_work.timer_handle);

    set_number_7seg(remaining_minutes);

    set_face_time(config_work);

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void task_state_resting(void *pvParameters) {
  for (;;) {
    // Display resting timer value (in minutes) on 7-segment display
    uint16_t remaining_minutes =
        get_remaining_minutes_ceil(config_rest.timer_handle);

    set_number_7seg(remaining_minutes);

    set_face_time(config_rest);

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void task_state_paused_working(void *pvParameters) {
  for (;;) {
    // Flash 7-segment display with working timer value (in minutes)
    uint16_t remaining_minutes =
        get_remaining_minutes_ceil(config_work.timer_handle);

    set_number_7seg(remaining_minutes);

    set_face_time(config_work);

    vTaskDelay(pdMS_TO_TICKS(500));

    set_segment_raw_7seg(0, 0x00);
    set_segment_raw_7seg(1, 0x00);
    set_segment_raw_7seg(2, 0x00);
    set_segment_raw_7seg(3, 0x00);

    set_face_time(config_work);

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void task_state_paused_resting(void *pvParameters) {
  for (;;) {
    // Flash 7-segment display with resting timer value (in minutes)
    uint16_t remaining_minutes =
        get_remaining_minutes_ceil(config_rest.timer_handle);

    set_number_7seg(remaining_minutes);

    set_face_time(config_rest);

    vTaskDelay(pdMS_TO_TICKS(500));

    set_segment_raw_7seg(0, 0x00);
    set_segment_raw_7seg(1, 0x00);
    set_segment_raw_7seg(2, 0x00);
    set_segment_raw_7seg(3, 0x00);

    set_face_time(config_rest);

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void task_state_finished_working(void *pvParameters) {
  for (;;) {
    // Show 0 on 7-segment display
    set_number_7seg(0);

    // Beep buzzer

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void task_state_finished_resting(void *pvParameters) {
  for (;;) {
    // Show 0 on 7-segment display
    set_number_7seg(0);

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

    set_number_lead_dot_7seg(minutes, true, 0b0001 & show_dot);

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

    set_number_lead_dot_7seg(minutes, true, 0b0001 & show_dot);

    counter--;

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void task_state_reset(void *pvParameters) {
  for (;;) {
    // Do nothing, immediately transition to STATE_NONE
    transition_to_state(STATE_NONE);
    vTaskSuspend(NULL);
  }
}
