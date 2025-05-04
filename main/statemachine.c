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
#include "esp_log.h"
#include "iot_button.h"
#include "statemachine.h"
#include <string.h>

static state_t current_state = STATE_RESET;

TaskHandle_t task_statemachine_handle;

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

void task_statemachine(void *pvParameters);

void init_statemachine() {
  static bool initialised = false;
  if (initialised) {
    return;
  }

  initialised = true;

  current_state = STATE_RESET;

  vTaskSuspendAll();
  xTaskCreate(task_statemachine, "statemachine", 2048, NULL, 1,
              &task_statemachine_handle);
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

static void buttons_register(state_t state) {
  iot_button_register_cb(config_io.btn_mode, BUTTON_SINGLE_CLICK, NULL,
                         btn_mode_short_press_func, (void *)state);
  iot_button_register_cb(config_io.btn_mode, BUTTON_LONG_PRESS_START, NULL,
                         btn_mode_long_press_func, (void *)state);
  iot_button_register_cb(config_io.btn_plus, BUTTON_SINGLE_CLICK, NULL,
                         btn_plus_short_press_func, (void *)state);
  iot_button_register_cb(config_io.btn_plus, BUTTON_LONG_PRESS_START, NULL,
                         btn_plus_long_press_func, (void *)state);
  iot_button_register_cb(config_io.btn_minus, BUTTON_SINGLE_CLICK, NULL,
                         btn_minus_short_press_func, (void *)state);
  iot_button_register_cb(config_io.btn_minus, BUTTON_LONG_PRESS_START, NULL,
                         btn_minus_long_press_func, (void *)state);
  iot_button_register_cb(config_io.btn_play, BUTTON_SINGLE_CLICK, NULL,
                         btn_play_short_press_func, (void *)state);
  iot_button_register_cb(config_io.btn_play, BUTTON_LONG_PRESS_START, NULL,
                         btn_play_long_press_func, (void *)state);
}

static void buttons_unregister(state_t state) {
  iot_button_unregister_cb(config_io.btn_mode, BUTTON_SINGLE_CLICK, NULL);
  iot_button_unregister_cb(config_io.btn_mode, BUTTON_LONG_PRESS_START, NULL);
  iot_button_unregister_cb(config_io.btn_plus, BUTTON_SINGLE_CLICK, NULL);
  iot_button_unregister_cb(config_io.btn_plus, BUTTON_LONG_PRESS_START, NULL);
  iot_button_unregister_cb(config_io.btn_minus, BUTTON_SINGLE_CLICK, NULL);
  iot_button_unregister_cb(config_io.btn_minus, BUTTON_LONG_PRESS_START, NULL);
  iot_button_unregister_cb(config_io.btn_play, BUTTON_SINGLE_CLICK, NULL);
  iot_button_unregister_cb(config_io.btn_play, BUTTON_LONG_PRESS_START, NULL);
}

bool transition_to_state(state_t new_state) {
  return pdPASS == xTaskNotify(task_statemachine_handle, (uint32_t)new_state,
                               eSetValueWithoutOverwrite);
}

bool transition_to_state_isr(state_t new_state, bool *higherPriorityTaskWoken) {
  BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
  BaseType_t result =
      xTaskNotifyFromISR(task_statemachine_handle, (uint32_t)new_state,
                         eSetValueWithoutOverwrite, &pxHigherPriorityTaskWoken);
  if (NULL != higherPriorityTaskWoken) {
    *higherPriorityTaskWoken = (pxHigherPriorityTaskWoken == pdTRUE);
  }
  return pdPASS == result;
}

int do_transition_to_state(state_t new_state) {
  if (can_transition_to_state(new_state)) {
    bool called_by_current_task =
        xTaskGetCurrentTaskHandle() == task_func_handles[current_state];

    if (!called_by_current_task) {
      vTaskSuspend(task_func_handles[current_state]);
    }

    state_t old_state = current_state;
    buttons_unregister(old_state);
    on_exit_funcs[old_state](new_state);
    current_state = new_state;
    on_entry_funcs[new_state](old_state);
    buttons_register(new_state);
    vTaskResume(task_func_handles[new_state]);

    if (called_by_current_task) {
      vTaskSuspend(NULL);
    }

    return 0;
  }
  return 1;
}

void task_statemachine(void *pvParameters) {
  uint32_t pulNotificationValue;
  for (;;) {
    if (pdPASS == xTaskNotifyWait(0, 0, &pulNotificationValue, portMAX_DELAY)) {
      ESP_ERROR_CHECK(do_transition_to_state((state_t)pulNotificationValue));
    }
  }
}
