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
#include "esp_err.h"
#include "esp_log.h"
#include "iot_button.h"
#include "statemachine.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

static const char *TAG = "STATE BUTTONS";

void btn_mode_short_press_func(void *button_handle, void *usr_data) {
  state_t state = (state_t)usr_data;
  ESP_LOGI(TAG, "Button MODE short press: %s", get_state_name(state));

  bool yield = false;
  switch (state) {
  case STATE_NONE: {
    transition_to_state_isr(STATE_SET_WORKING, &yield);
    break;
  }
  case STATE_WORKING: {
    break;
  }
  case STATE_RESTING: {
    break;
  }
  case STATE_PAUSED_WORKING: {
    break;
  }
  case STATE_PAUSED_RESTING: {
    break;
  }
  case STATE_FINISHED_WORKING: {
    break;
  }
  case STATE_FINISHED_RESTING: {
    break;
  }
  case STATE_SET_WORKING: {
    transition_to_state_isr(STATE_SET_RESTING, &yield);
    break;
  }
  case STATE_SET_RESTING: {
    transition_to_state_isr(STATE_NONE, &yield);
    break;
  }
  case STATE_RESET: {
    break;
  }
  default: {
    break;
  }
  }

  if (yield) {
    taskYIELD();
  }
}

void btn_mode_long_press_func(void *button_handle, void *usr_data) {
  state_t state = (state_t)usr_data;
  ESP_LOGI(TAG, "Button MODE long press: %s", get_state_name(state));

  bool yield = false;
  switch (state) {
  case STATE_NONE: {
    config_work.count_direction ^= 1;
    config_rest.count_direction ^= 1;
    break;
  }
  case STATE_WORKING: {
    break;
  }
  case STATE_RESTING: {
    break;
  }
  case STATE_PAUSED_WORKING: {
    break;
  }
  case STATE_PAUSED_RESTING: {
    break;
  }
  case STATE_FINISHED_WORKING: {
    break;
  }
  case STATE_FINISHED_RESTING: {
    break;
  }
  case STATE_SET_WORKING: {
    config_work.count_direction ^= 1;
    config_rest.count_direction ^= 1;
    break;
  }
  case STATE_SET_RESTING: {
    config_work.count_direction ^= 1;
    config_rest.count_direction ^= 1;
    break;
  }
  case STATE_RESET: {
    break;
  }
  default: {
    break;
  }
  }

  if (yield) {
    taskYIELD();
  }
}

void btn_plus_short_press_func(void *button_handle, void *usr_data) {
  state_t state = (state_t)usr_data;
  ESP_LOGI(TAG, "Button PLUS short press: %s", get_state_name(state));

  bool yield = false;
  switch (state) {
  case STATE_NONE: {
    break;
  }
  case STATE_WORKING: {
    break;
  }
  case STATE_RESTING: {
    break;
  }
  case STATE_PAUSED_WORKING: {
    break;
  }
  case STATE_PAUSED_RESTING: {
    break;
  }
  case STATE_FINISHED_WORKING: {
    break;
  }
  case STATE_FINISHED_RESTING: {
    break;
  }
  case STATE_SET_WORKING: {
    config_work.timer_duration.tv_sec =
        min(config_work.timer_duration.tv_sec + 60, 9999 * 60);
    break;
  }
  case STATE_SET_RESTING: {
    config_rest.timer_duration.tv_sec =
        min(config_rest.timer_duration.tv_sec + 60, 9999 * 60);
    break;
  }
  case STATE_RESET: {
    break;
  }
  default: {
    break;
  }
  }

  if (yield) {
    taskYIELD();
  }
}

void btn_plus_long_press_func(void *button_handle, void *usr_data) {
  state_t state = (state_t)usr_data;
  ESP_LOGI(TAG, "Button PLUS long press: %s", get_state_name(state));

  bool yield = false;
  switch (state) {
  case STATE_NONE: {
    break;
  }
  case STATE_WORKING: {
    break;
  }
  case STATE_RESTING: {
    break;
  }
  case STATE_PAUSED_WORKING: {
    break;
  }
  case STATE_PAUSED_RESTING: {
    break;
  }
  case STATE_FINISHED_WORKING: {
    break;
  }
  case STATE_FINISHED_RESTING: {
    break;
  }
  case STATE_SET_WORKING: {
    config_work.timer_duration.tv_sec =
        min(config_work.timer_duration.tv_sec + (10 * 60), 9999 * 60);

    button_event_t btn_minus_event = iot_button_get_event(config_io.btn_minus);
    if (BUTTON_LONG_PRESS_START == btn_minus_event ||
        BUTTON_LONG_PRESS_HOLD == btn_minus_event) {
      config_work.timer_duration.tv_sec = 60;
    }
    break;
  }
  case STATE_SET_RESTING: {
    config_rest.timer_duration.tv_sec =
        min(config_rest.timer_duration.tv_sec + (10 * 60), 9999 * 60);

    button_event_t btn_minus_event = iot_button_get_event(config_io.btn_minus);
    if (BUTTON_LONG_PRESS_START == btn_minus_event ||
        BUTTON_LONG_PRESS_HOLD == btn_minus_event) {
      config_rest.timer_duration.tv_sec = 60;
    }
    break;
  }
  case STATE_RESET: {
    break;
  }
  default: {
    break;
  }
  }

  if (yield) {
    taskYIELD();
  }
}

void btn_minus_short_press_func(void *button_handle, void *usr_data) {
  state_t state = (state_t)usr_data;
  ESP_LOGI(TAG, "Button MINUS short press: %s", get_state_name(state));

  bool yield = false;
  switch (state) {
  case STATE_NONE: {
    break;
  }
  case STATE_WORKING: {
    break;
  }
  case STATE_RESTING: {
    break;
  }
  case STATE_PAUSED_WORKING: {
    break;
  }
  case STATE_PAUSED_RESTING: {
    break;
  }
  case STATE_FINISHED_WORKING: {
    break;
  }
  case STATE_FINISHED_RESTING: {
    break;
  }
  case STATE_SET_WORKING: {
    config_work.timer_duration.tv_sec =
        max(config_work.timer_duration.tv_sec - 60, 60);
    break;
  }
  case STATE_SET_RESTING: {
    config_rest.timer_duration.tv_sec =
        max(config_rest.timer_duration.tv_sec - 60, 60);
    break;
  }
  case STATE_RESET: {
    break;
  }
  default: {
    break;
  }
  }

  if (yield) {
    taskYIELD();
  }
}

void btn_minus_long_press_func(void *button_handle, void *usr_data) {
  state_t state = (state_t)usr_data;
  ESP_LOGI(TAG, "Button MINUS long press: %s", get_state_name(state));

  bool yield = false;
  switch (state) {
  case STATE_NONE: {
    break;
  }
  case STATE_WORKING: {
    break;
  }
  case STATE_RESTING: {
    break;
  }
  case STATE_PAUSED_WORKING: {
    break;
  }
  case STATE_PAUSED_RESTING: {
    break;
  }
  case STATE_FINISHED_WORKING: {
    break;
  }
  case STATE_FINISHED_RESTING: {
    break;
  }
  case STATE_SET_WORKING: {
    config_work.timer_duration.tv_sec =
        max(config_work.timer_duration.tv_sec - (10 * 60), 60);

    button_event_t btn_plus_event = iot_button_get_event(config_io.btn_plus);
    if (BUTTON_LONG_PRESS_START == btn_plus_event ||
        BUTTON_LONG_PRESS_HOLD == btn_plus_event) {
      config_work.timer_duration.tv_sec = 60;
    }
    break;
  }
  case STATE_SET_RESTING: {
    config_rest.timer_duration.tv_sec =
        max(config_rest.timer_duration.tv_sec - (10 * 60), 60);

    button_event_t btn_plus_event = iot_button_get_event(config_io.btn_plus);
    if (BUTTON_LONG_PRESS_START == btn_plus_event ||
        BUTTON_LONG_PRESS_HOLD == btn_plus_event) {
      config_rest.timer_duration.tv_sec = 60;
    }
    break;
  }
  case STATE_RESET: {
    break;
  }
  default: {
    break;
  }
  }

  if (yield) {
    taskYIELD();
  }
}

void btn_play_short_press_func(void *button_handle, void *usr_data) {
  state_t state = (state_t)usr_data;
  ESP_LOGI(TAG, "Button PLAY short press: %s", get_state_name(state));

  bool yield = false;
  switch (state) {
  case STATE_NONE: {
    transition_to_state_isr(STATE_WORKING, &yield);
    break;
  }
  case STATE_WORKING: {
    transition_to_state_isr(STATE_PAUSED_WORKING, &yield);
    break;
  }
  case STATE_RESTING: {
    transition_to_state_isr(STATE_PAUSED_RESTING, &yield);
    break;
  }
  case STATE_PAUSED_WORKING: {
    transition_to_state_isr(STATE_WORKING, &yield);
    break;
  }
  case STATE_PAUSED_RESTING: {
    transition_to_state_isr(STATE_RESTING, &yield);
    break;
  }
  case STATE_FINISHED_WORKING: {
    transition_to_state_isr(STATE_RESTING, &yield);
    break;
  }
  case STATE_FINISHED_RESTING: {
    transition_to_state_isr(STATE_WORKING, &yield);
    break;
  }
  case STATE_SET_WORKING: {
    break;
  }
  case STATE_SET_RESTING: {
    break;
  }
  case STATE_RESET: {
    break;
  }
  default: {
    break;
  }
  }

  if (yield) {
    taskYIELD();
  }
}

void btn_play_long_press_func(void *button_handle, void *usr_data) {
  state_t state = (state_t)usr_data;
  ESP_LOGI(TAG, "Button PLAY long press: %s", get_state_name(state));

  bool yield = false;
  switch (state) {
  case STATE_NONE: {
    transition_to_state_isr(STATE_RESET, &yield);
    break;
  }
  case STATE_WORKING: {
    transition_to_state_isr(STATE_RESET, &yield);
    break;
  }
  case STATE_RESTING: {
    transition_to_state_isr(STATE_RESET, &yield);
    break;
  }
  case STATE_PAUSED_WORKING: {
    transition_to_state_isr(STATE_RESET, &yield);
    break;
  }
  case STATE_PAUSED_RESTING: {
    transition_to_state_isr(STATE_RESET, &yield);
    break;
  }
  case STATE_FINISHED_WORKING: {
    transition_to_state_isr(STATE_RESET, &yield);
    break;
  }
  case STATE_FINISHED_RESTING: {
    transition_to_state_isr(STATE_RESET, &yield);
    break;
  }
  case STATE_SET_WORKING: {
    break;
  }
  case STATE_SET_RESTING: {
    break;
  }
  case STATE_RESET: {
    break;
  }
  default: {
    break;
  }
  }

  if (yield) {
    taskYIELD();
  }
}
