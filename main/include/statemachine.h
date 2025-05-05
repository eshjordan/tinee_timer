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

#pragma once

#include "freertos/idf_additions.h"
#include "iot_button.h"
#include "portmacro.h"
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef enum {
  STATE_NONE = 0,
  STATE_WORKING,
  STATE_RESTING,
  STATE_PAUSED_WORKING,
  STATE_PAUSED_RESTING,
  STATE_FINISHED_WORKING,
  STATE_FINISHED_RESTING,
  STATE_SET_WORKING,
  STATE_SET_RESTING,
  STATE_RESET,
  STATE_MAX,
} state_t;

extern state_t current_state;

void init_statemachine();

state_t get_current_state();

static inline const char *get_state_name(state_t state) {
  switch (state) {
  case STATE_NONE:
    return "NONE";
  case STATE_WORKING:
    return "WORKING";
  case STATE_RESTING:
    return "RESTING";
  case STATE_PAUSED_WORKING:
    return "PAUSED_WORKING";
  case STATE_PAUSED_RESTING:
    return "PAUSED_RESTING";
  case STATE_FINISHED_WORKING:
    return "FINISHED_WORKING";
  case STATE_FINISHED_RESTING:
    return "FINISHED_RESTING";
  case STATE_SET_WORKING:
    return "SET_WORKING";
  case STATE_SET_RESTING:
    return "SET_RESTING";
  case STATE_RESET:
    return "RESET";
  default:
    return "UNKNOWN";
  }
}

bool can_transition_to_state(state_t new_state);

bool transition_to_state(state_t new_state);

bool transition_to_state_isr(state_t new_state, bool *higherPriorityTaskWoken);

extern void on_entry_state_none(state_t old_state);
extern void on_entry_state_working(state_t old_state);
extern void on_entry_state_resting(state_t old_state);
extern void on_entry_state_paused_working(state_t old_state);
extern void on_entry_state_paused_resting(state_t old_state);
extern void on_entry_state_finished_working(state_t old_state);
extern void on_entry_state_finished_resting(state_t old_state);
extern void on_entry_state_set_working(state_t old_state);
extern void on_entry_state_set_resting(state_t old_state);
extern void on_entry_state_reset(state_t old_state);

extern void task_state_none(void *);
extern void task_state_working(void *);
extern void task_state_resting(void *);
extern void task_state_paused_working(void *);
extern void task_state_paused_resting(void *);
extern void task_state_finished_working(void *);
extern void task_state_finished_resting(void *);
extern void task_state_set_working(void *);
extern void task_state_set_resting(void *);
extern void task_state_reset(void *);

extern void on_exit_state_none(state_t new_state);
extern void on_exit_state_working(state_t new_state);
extern void on_exit_state_resting(state_t new_state);
extern void on_exit_state_paused_working(state_t new_state);
extern void on_exit_state_paused_resting(state_t new_state);
extern void on_exit_state_finished_working(state_t new_state);
extern void on_exit_state_finished_resting(state_t new_state);
extern void on_exit_state_set_working(state_t new_state);
extern void on_exit_state_set_resting(state_t new_state);
extern void on_exit_state_reset(state_t new_state);

extern void btn_mode_short_press_func(void *button_handle, void *usr_data);
extern void btn_mode_long_press_func(void *button_handle, void *usr_data);
extern void btn_plus_short_press_func(void *button_handle, void *usr_data);
extern void btn_plus_long_press_func(void *button_handle, void *usr_data);
extern void btn_minus_short_press_func(void *button_handle, void *usr_data);
extern void btn_minus_long_press_func(void *button_handle, void *usr_data);
extern void btn_play_short_press_func(void *button_handle, void *usr_data);
extern void btn_play_long_press_func(void *button_handle, void *usr_data);

extern TaskHandle_t task_statemachine_handle;

#ifdef __cplusplus
}
#endif
