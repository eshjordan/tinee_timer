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

void on_entry_state_none(state_t old_state);
void on_entry_state_working(state_t old_state);
void on_entry_state_resting(state_t old_state);
void on_entry_state_paused_working(state_t old_state);
void on_entry_state_paused_resting(state_t old_state);
void on_entry_state_finished_working(state_t old_state);
void on_entry_state_finished_resting(state_t old_state);
void on_entry_state_set_working(state_t old_state);
void on_entry_state_set_resting(state_t old_state);
void on_entry_state_reset(state_t old_state);

void task_state_none(void *);
void task_state_working(void *);
void task_state_resting(void *);
void task_state_paused_working(void *);
void task_state_paused_resting(void *);
void task_state_finished_working(void *);
void task_state_finished_resting(void *);
void task_state_set_working(void *);
void task_state_set_resting(void *);
void task_state_reset(void *);

void on_exit_state_none(state_t new_state);
void on_exit_state_working(state_t new_state);
void on_exit_state_resting(state_t new_state);
void on_exit_state_paused_working(state_t new_state);
void on_exit_state_paused_resting(state_t new_state);
void on_exit_state_finished_working(state_t new_state);
void on_exit_state_finished_resting(state_t new_state);
void on_exit_state_set_working(state_t new_state);
void on_exit_state_set_resting(state_t new_state);
void on_exit_state_reset(state_t new_state);

#ifdef __cplusplus
}
#endif
