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

#include "bsp/esp-bsp.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "iot_button.h"
#include "sdkconfig.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

static button_handle_t btn_array[BSP_BUTTON_NUM];

void io_init(void);

void app_main(void) {
  // Allow other core to finish initialization
  vTaskDelay(pdMS_TO_TICKS(100));

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

static void btn_handler(void *button_handle, void *usr_data)
{
    int button_pressed = (int)usr_data;

    ESP_LOGI("TAG", "Button pressed: %d", button_pressed);
}

void io_init(void) {
  const uint8_t num_btns = (sizeof(btn_array) / sizeof(btn_array[0]));
  bsp_iot_button_create(btn_array, NULL, num_btns);

  for (int i = 0; i < num_btns; i++) {

    ESP_ERROR_CHECK(iot_button_register_cb(btn_array[i], BUTTON_PRESS_DOWN,
                                           btn_handler, (void *)i));
  }
}
