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

#include "esp_err.h"
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

  if (new_state &
      (STATE_FINISHED_WORKING | STATE_PAUSED_WORKING | STATE_RESET)) {
    // Pause working timer
    ESP_ERROR_CHECK(gptimer_stop(config_work.timer_handle));
  }

  if (new_state & (STATE_FINISHED_WORKING | STATE_RESET)) {
    // Stop working timer
    ESP_ERROR_CHECK(gptimer_disable(config_work.timer_handle));
    ESP_ERROR_CHECK(gptimer_del_timer(config_work.timer_handle));
    config_work.timer_handle = NULL;
  }
}

void on_exit_state_resting(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: RESTING");

  if (new_state &
      (STATE_FINISHED_RESTING | STATE_PAUSED_RESTING | STATE_RESET)) {
    // Pause resting timer
    ESP_ERROR_CHECK(gptimer_stop(config_rest.timer_handle));
  }

  if (new_state & (STATE_FINISHED_RESTING | STATE_RESET)) {
    // Stop resting timer
    ESP_ERROR_CHECK(gptimer_disable(config_rest.timer_handle));
    ESP_ERROR_CHECK(gptimer_del_timer(config_rest.timer_handle));
    config_rest.timer_handle = NULL;
  }
}

void on_exit_state_paused_working(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: PAUSED WORKING");

  if (new_state & STATE_WORKING) {
    // Do nothing
  }

  if (new_state & STATE_RESET) {
    // Stop working timer
    ESP_ERROR_CHECK(gptimer_disable(config_work.timer_handle));
    ESP_ERROR_CHECK(gptimer_del_timer(config_work.timer_handle));
    config_work.timer_handle = NULL;
  }
}

void on_exit_state_paused_resting(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: PAUSED RESTING");

  if (new_state & STATE_RESTING) {
    // Do nothing
  }

  if (new_state & STATE_RESET) {
    // Stop resting timer
    ESP_ERROR_CHECK(gptimer_disable(config_rest.timer_handle));
    ESP_ERROR_CHECK(gptimer_del_timer(config_rest.timer_handle));
    config_rest.timer_handle = NULL;
  }
}

void on_exit_state_finished_working(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: FINISHED WORKING");

  if (new_state & (STATE_RESTING | STATE_RESET)) {
    // Stop and disable alarm (can't determine if it was running)
    ESP_ERROR_CHECK(gptimer_stop(config_finished_working.timer_handle));
    ESP_ERROR_CHECK(gptimer_disable(config_finished_working.timer_handle));
    ESP_ERROR_CHECK(gptimer_del_timer(config_finished_working.timer_handle));
    config_finished_working.timer_handle = NULL;
  }
}

void on_exit_state_finished_resting(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: FINISHED RESTING");

  if (new_state & (STATE_WORKING | STATE_RESET)) {
    ESP_ERROR_CHECK(gptimer_stop(config_finished_resting.timer_handle));
    ESP_ERROR_CHECK(gptimer_disable(config_finished_resting.timer_handle));
    ESP_ERROR_CHECK(gptimer_del_timer(config_finished_resting.timer_handle));
    config_finished_resting.timer_handle = NULL;
  }
}

void on_exit_state_set_working(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: SET WORKING");

  // Save the working timer configuration to NVM
  nvs_handle_t timer_nvs_handle;
  ESP_ERROR_CHECK(nvs_open("timer", NVS_READWRITE, &timer_nvs_handle));
  ESP_ERROR_CHECK(nvs_set_blob(timer_nvs_handle, "work", &config_work,
                               sizeof(config_work)));
  ESP_ERROR_CHECK(nvs_commit(timer_nvs_handle));
  nvs_close(timer_nvs_handle);
}

void on_exit_state_set_resting(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: SET RESTING");

  // Save the resting timer configuration to NVM
  nvs_handle_t timer_nvs_handle;
  ESP_ERROR_CHECK(nvs_open("timer", NVS_READWRITE, &timer_nvs_handle));
  ESP_ERROR_CHECK(nvs_set_blob(timer_nvs_handle, "rest", &config_rest,
                               sizeof(config_rest)));
  ESP_ERROR_CHECK(nvs_commit(timer_nvs_handle));
  nvs_close(timer_nvs_handle);
}

void on_exit_state_reset(state_t new_state) {
  ESP_DRAM_LOGI(TAG, "State: RESET");

  // Do nothing
}
