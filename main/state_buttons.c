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
#include "statemachine.h"

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

void btn_mode_long_press_func(void *button_handle, void *usr_data) {
  state_t state = (state_t)usr_data;
  ESP_LOGI(TAG, "Button MODE long press: %s", get_state_name(state));

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
