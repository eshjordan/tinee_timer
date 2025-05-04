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

#include "freertos/FreeRTOS.h"

#include "config.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "esp_private/log_timestamp.h"
#include "statemachine.h"

static const char *TAG = "ON EXIT";

void on_exit_state_none(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: NONE");

  // Do nothing
}

void on_exit_state_working(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: WORKING");

  if (new_state == STATE_PAUSED_WORKING) {
    // Pause working timer
    ESP_ERROR_CHECK(gptimer_stop(config_work.timer_handle));
  } else if (new_state == STATE_FINISHED_WORKING || new_state == STATE_RESET) {
    // Stop working timer
    ESP_ERROR_CHECK(gptimer_stop(config_work.timer_handle));
    ESP_ERROR_CHECK(gptimer_disable(config_work.timer_handle));
    ESP_ERROR_CHECK(gptimer_del_timer(config_work.timer_handle));
    config_work.timer_handle = NULL;
  }
}

void on_exit_state_resting(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: RESTING");

  if (new_state == STATE_PAUSED_RESTING) {
    // Pause resting timer
    ESP_ERROR_CHECK(gptimer_stop(config_rest.timer_handle));
  } else if (new_state == STATE_FINISHED_RESTING || new_state == STATE_RESET) {
    // Stop resting timer
    ESP_ERROR_CHECK(gptimer_stop(config_rest.timer_handle));
    ESP_ERROR_CHECK(gptimer_disable(config_rest.timer_handle));
    ESP_ERROR_CHECK(gptimer_del_timer(config_rest.timer_handle));
    config_rest.timer_handle = NULL;
  }
}

void on_exit_state_paused_working(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: PAUSED WORKING");

  if (new_state == STATE_WORKING) {
    // Do nothing
  } else if (new_state == STATE_RESET) {
    // Stop working timer
    ESP_ERROR_CHECK(gptimer_stop(config_work.timer_handle));
    ESP_ERROR_CHECK(gptimer_disable(config_work.timer_handle));
    ESP_ERROR_CHECK(gptimer_del_timer(config_work.timer_handle));
    config_work.timer_handle = NULL;
  }
}

void on_exit_state_paused_resting(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: PAUSED RESTING");

  if (new_state == STATE_RESTING) {
    // Do nothing
  } else if (new_state == STATE_RESET) {
    // Stop resting timer
    ESP_ERROR_CHECK(gptimer_stop(config_rest.timer_handle));
    ESP_ERROR_CHECK(gptimer_disable(config_rest.timer_handle));
    ESP_ERROR_CHECK(gptimer_del_timer(config_rest.timer_handle));
    config_rest.timer_handle = NULL;
  }
}

void on_exit_state_finished_working(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: FINISHED WORKING");

  if (new_state == STATE_RESTING || new_state == STATE_RESET) {
    // Stop alarm, stop 5s timer
    ESP_ERROR_CHECK(gptimer_stop(config_finished_working.timer_handle));
    ESP_ERROR_CHECK(gptimer_disable(config_finished_working.timer_handle));
    ESP_ERROR_CHECK(gptimer_del_timer(config_finished_working.timer_handle));
    config_finished_working.timer_handle = NULL;
  }
}

void on_exit_state_finished_resting(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: FINISHED RESTING");

  if (new_state == STATE_WORKING || new_state == STATE_RESET) {
    // Stop alarm, stop 5s timer
    ESP_ERROR_CHECK(gptimer_stop(config_finished_resting.timer_handle));
    ESP_ERROR_CHECK(gptimer_disable(config_finished_resting.timer_handle));
    ESP_ERROR_CHECK(gptimer_del_timer(config_finished_resting.timer_handle));
    config_finished_resting.timer_handle = NULL;
  }
}

void on_exit_state_set_working(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: SET WORKING");

  // Do nothing
}

void on_exit_state_set_resting(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: SET RESTING");

  // Do nothing
}

void on_exit_state_reset(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: RESET");

  // Do nothing
}
