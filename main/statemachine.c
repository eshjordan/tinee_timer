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

#include "statemachine.h"
#include "FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include <string.h>

static state_t current_state = STATE_RESET;

typedef void (*on_entry_func_t)(state_t);
typedef void (*task_func_t)(void *);
typedef void (*on_exit_func_t)(state_t);

static on_entry_func_t on_entry_funcs[STATE_MAX] = {
    on_entry_state_none,
    on_entry_state_working,
    on_entry_state_resting,
    on_entry_state_paused_working,
    on_entry_state_paused_resting,
    on_entry_state_finished_working,
    on_entry_state_finished_resting,
    on_entry_state_set_working,
    on_entry_state_set_resting,
    on_entry_state_reset,
};

static task_func_t task_funcs[STATE_MAX] = {
    task_state_none,
    task_state_working,
    task_state_resting,
    task_state_paused_working,
    task_state_paused_resting,
    task_state_finished_working,
    task_state_finished_resting,
    task_state_set_working,
    task_state_set_resting,
    task_state_reset,
};

static TaskHandle_t task_func_handles[STATE_MAX] = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};

static on_exit_func_t on_exit_funcs[STATE_MAX] = {
    on_exit_state_none,
    on_exit_state_working,
    on_exit_state_resting,
    on_exit_state_paused_working,
    on_exit_state_paused_resting,
    on_exit_state_finished_working,
    on_exit_state_finished_resting,
    on_exit_state_set_working,
    on_exit_state_set_resting,
    on_exit_state_reset,
};

void init_statemachine() {
  static bool initialised = false;
  if (initialised) {
    return;
  }

  initialised = true;

  current_state = STATE_RESET;

  vTaskSuspendAll();
  for (int i = 0; i < STATE_MAX; i++) {
    char task_name[configMAX_TASK_NAME_LEN] = "";
    snprintf(task_name, configMAX_TASK_NAME_LEN, "%s", get_state_name(i));
    xTaskCreate(task_funcs[i], task_name, 2048, NULL, 1, &task_func_handles[i]);
    vTaskSuspend(task_func_handles[i]);
  }
  xTaskResumeAll();

  transition_to_state(STATE_NONE);
}

state_t get_current_state() { return current_state; }

bool can_transition_to_state(state_t new_state) {
  switch (current_state) {
  case STATE_NONE:
    return new_state == STATE_WORKING || new_state == STATE_SET_WORKING ||
           new_state == STATE_RESET;
  case STATE_WORKING:
    return new_state == STATE_PAUSED_WORKING ||
           new_state == STATE_FINISHED_WORKING || new_state == STATE_RESET;
  case STATE_PAUSED_WORKING:
    return new_state == STATE_WORKING || new_state == STATE_RESET;
  case STATE_FINISHED_WORKING:
    return new_state == STATE_RESTING || new_state == STATE_RESET;
  case STATE_RESTING:
    return new_state == STATE_PAUSED_RESTING ||
           new_state == STATE_FINISHED_RESTING || new_state == STATE_RESET;
  case STATE_PAUSED_RESTING:
    return new_state == STATE_RESTING || new_state == STATE_RESET;
  case STATE_FINISHED_RESTING:
    return new_state == STATE_WORKING || new_state == STATE_RESET;
  case STATE_SET_WORKING:
    return new_state == STATE_SET_RESTING;
  case STATE_SET_RESTING:
    return new_state == STATE_NONE;
  case STATE_RESET:
    return new_state == STATE_NONE;
  case STATE_MAX:
  default:
    return false;
  }
}

bool transition_to_state(state_t new_state) {
  if (can_transition_to_state(new_state)) {
    // Enter critical section
    vTaskSuspendAll();

    bool called_by_current_task =
        xTaskGetCurrentTaskHandle() == task_func_handles[current_state];

    if (!called_by_current_task) {
      vTaskSuspend(task_func_handles[current_state]);
    }

    on_exit_funcs[current_state](new_state);
    state_t old_state = current_state;
    current_state = new_state;
    on_entry_funcs[current_state](old_state);
    vTaskResume(task_func_handles[current_state]);

    // Exit critical section
    xTaskResumeAll();

    if (called_by_current_task) {
      vTaskSuspend(NULL);
    }

    return true;
  }
  return false;
}
