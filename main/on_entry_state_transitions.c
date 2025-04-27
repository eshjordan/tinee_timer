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
#include "driver/gptimer_types.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "statemachine.h"
#include <time.h>

static void setup_alarm_timer(gptimer_handle_t *timer, struct timespec duration,
                              gptimer_alarm_cb_t alarm_cb) {

  gptimer_config_t gptimer_config = {
      .clk_src = GPTIMER_CLK_SRC_DEFAULT,
      .direction = GPTIMER_COUNT_DOWN,
      .resolution_hz = 1000,
  };

  gptimer_alarm_config_t gptimer_alarm_config = {
      .alarm_count = 0,
      .flags = {.auto_reload_on_alarm = false},
  };

  ESP_ERROR_CHECK(gptimer_new_timer(&gptimer_config, timer));

  ESP_ERROR_CHECK(gptimer_set_alarm_action(*timer, &gptimer_alarm_config));

  gptimer_event_callbacks_t gptimer_event_callbacks = {
      .on_alarm = alarm_cb,
  };

  ESP_ERROR_CHECK(
      gptimer_register_event_callbacks(*timer, &gptimer_event_callbacks, NULL));

  uint32_t resolution = 0;
  ESP_ERROR_CHECK(gptimer_get_resolution(*timer, &resolution));

  resolution = resolution ?: (gptimer_config.resolution_hz ?: 1000);

  uint32_t duration_ticks =
      (uint32_t)((duration.tv_sec * resolution) +
                 (duration.tv_nsec / (1000 * resolution)));

  ESP_ERROR_CHECK(gptimer_set_raw_count(*timer, duration_ticks));
  ESP_ERROR_CHECK(gptimer_enable(*timer));
  ESP_ERROR_CHECK(gptimer_start(*timer));
}

void on_entry_state_none(state_t old_state) {
  // Display working timer value on 7-segment display
}

static bool work_alarm_cb(gptimer_handle_t timer,
                          const gptimer_alarm_event_data_t *edata,
                          void *user_ctx) {
  transition_to_state(STATE_FINISHED_WORKING);
  return true;
}

void on_entry_state_working(state_t old_state) {
  if (old_state == STATE_NONE || old_state == STATE_FINISHED_RESTING) {
    // Start working timer
    setup_alarm_timer(&config_work.timer_handle, config_work.timer_duration,
                      work_alarm_cb);

  } else if (old_state == STATE_PAUSED_WORKING) {
    // Resume working timer
    ESP_ERROR_CHECK(gptimer_start(config_work.timer_handle));
  }
}

static bool rest_alarm_cb(gptimer_handle_t timer,
                          const gptimer_alarm_event_data_t *edata,
                          void *user_ctx) {
  transition_to_state(STATE_FINISHED_RESTING);
  return true;
}

void on_entry_state_resting(state_t old_state) {
  if (old_state == STATE_FINISHED_WORKING) {
    // Start resting timer
    setup_alarm_timer(&config_rest.timer_handle, config_rest.timer_duration,
                      rest_alarm_cb);

  } else if (old_state == STATE_PAUSED_RESTING) {
    // Resume resting timer
    ESP_ERROR_CHECK(gptimer_start(config_rest.timer_handle));
  }
}

void on_entry_state_paused_working(state_t old_state) {
  // Do nothing
}

void on_entry_state_paused_resting(state_t old_state) {
  // Do nothing
}

static bool finished_working_alarm_cb(gptimer_handle_t timer,
                                      const gptimer_alarm_event_data_t *edata,
                                      void *user_ctx) {
  transition_to_state(STATE_RESTING);
  return true;
}

void on_entry_state_finished_working(state_t old_state) {
  // Start alarm, start 5s timer
  setup_alarm_timer(&config_finished_working.timer_handle,
                    config_finished_working.timer_duration,
                    finished_working_alarm_cb);
}

void on_entry_state_finished_resting(state_t old_state) {
  // Start alarm, start 5s timer
  setup_alarm_timer(&config_finished_resting.timer_handle,
                    config_finished_resting.timer_duration,
                    finished_working_alarm_cb);
}

void on_entry_state_set_working(state_t old_state) {
  // Do nothing
}

void on_entry_state_set_resting(state_t old_state) {
  // Do nothing
}

void on_entry_state_reset(state_t old_state) {
  // Do nothing
}
