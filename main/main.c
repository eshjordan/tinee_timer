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

#include "bsp/esp-bsp.h"
#include "config.h"
#include "esp_err.h"
#include "esp_log.h"
#include "iot_button.h"
#include "statemachine.h"
#include "tm1637.h"
#include "wrapper_7seg.h"
#include <stdint.h>

static button_handle_t btn_array[BSP_BUTTON_NUM];

void io_init(void);

void app_main(void) {
  // Allow other core to finish initialization
  vTaskDelay(pdMS_TO_TICKS(100));

  io_init();

  config_work.timer_duration.tv_sec = 2;
  config_rest.timer_duration.tv_sec = 2;
  config_finished_working.timer_duration.tv_sec = 2;
  config_finished_resting.timer_duration.tv_sec = 2;

  init_statemachine();

  vTaskSuspend(NULL);

  // //Create semaphores to synchronize
  // sync_spin_task = xSemaphoreCreateCounting(NUM_OF_SPIN_TASKS, 0);
  // sync_stats_task = xSemaphoreCreateBinary();

  // //Create spin tasks
  // for (int i = 0; i < NUM_OF_SPIN_TASKS; i++) {
  //     snprintf(task_names[i], configMAX_TASK_NAME_LEN, "spin%d", i);
  //     xTaskCreatePinnedToCore(spin_task, task_names[i], 1024, NULL,
  //     SPIN_TASK_PRIO, NULL, tskNO_AFFINITY);
  // }

  // //Create and start stats task
  // xTaskCreatePinnedToCore(stats_task, "stats", 4096, NULL, STATS_TASK_PRIO,
  // NULL, tskNO_AFFINITY); xSemaphoreGive(sync_stats_task);
}

static void btn_single_click_handler(void *button_handle, void *usr_data) {
  int button_pressed = (int)usr_data;

  ESP_LOGI("TAG", "Button pressed: %d", button_pressed);
}

static void btn_long_press_start_handler(void *button_handle, void *usr_data) {
  int button_pressed = (int)usr_data;

  ESP_LOGI("TAG", "Button long press start: %d", button_pressed);
}

void io_init(void) {
  const uint8_t num_btns = (sizeof(btn_array) / sizeof(btn_array[0]));
  bsp_iot_button_create(btn_array, NULL, num_btns);

  config_io.btn_mode = btn_array[BSP_BUTTON_1];
  config_io.btn_plus = btn_array[BSP_BUTTON_2];
  config_io.btn_minus = btn_array[BSP_BUTTON_3];
  config_io.btn_play = btn_array[BSP_BUTTON_4];

  init_7seg();
}
