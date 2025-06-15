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
#include "config.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "iot_button.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "statemachine.h"
#include "tm1637.h"
#include "wrapper_7seg.h"
#include <stdint.h>

static button_handle_t btn_array[BSP_BUTTON_NUM];

void io_init(void);

void app_main(void) {
  // Allow other core to finish initialization
  vTaskDelay(pdMS_TO_TICKS(100));

  config_work.timer_duration.tv_sec = 1 * 60;        // 1 minute
  config_rest.timer_duration.tv_sec = 1 * 60;        // 1 minute
  config_finished_working.timer_duration.tv_sec = 5; // 5 seconds
  config_finished_resting.timer_duration.tv_sec = 5; // 5 seconds

  io_init();

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

void io_init(void) {
  const uint8_t num_btns = (sizeof(btn_array) / sizeof(btn_array[0]));
  bsp_iot_button_create(btn_array, NULL, num_btns);

  config_io.btn_mode = btn_array[BSP_BUTTON_1];
  config_io.btn_plus = btn_array[BSP_BUTTON_2];
  config_io.btn_minus = btn_array[BSP_BUTTON_3];
  config_io.btn_play = btn_array[BSP_BUTTON_4];

  init_7seg();
  // Check error
  ESP_ERROR_CHECK(nvs_flash_init());
  nvs_handle_t timer_nvs_handle;
  ESP_ERROR_CHECK(nvs_open("timer", NVS_READWRITE, &timer_nvs_handle));

  struct stored_configs {
    void *ptr;
    size_t size;
    char *name;
  };

  struct stored_configs something[] = {
      {&config_work, sizeof(config_work), "work"},
      {&config_rest, sizeof(config_rest), "rest"},
  };

  for (size_t i = 0; i < (sizeof(something) / sizeof(something[0])); i++) {
    nvs_type_t type;
    esp_err_t found = nvs_find_key(timer_nvs_handle, something[i].name, &type);
    size_t length = 0;

    if (ESP_OK == found && NVS_TYPE_BLOB == type) {
      ESP_ERROR_CHECK(
          nvs_get_blob(timer_nvs_handle, something[i].name, NULL, &length));
      if (something[i].size == length) {
        // Load stored config from NVS
        ESP_ERROR_CHECK(nvs_get_blob(timer_nvs_handle, something[i].name,
                                     something[i].ptr, &length));
        ESP_LOGI("NVM", "Config %s loaded from NVS", something[i].name);
        ESP_LOGI(
            "NVM", "%s - %llu seconds", something[i].name,
            i == 0
                ? ((config_work_t *)something[i].ptr)->timer_duration.tv_sec
                : ((config_rest_t *)something[i].ptr)->timer_duration.tv_sec);
      }
    }

    if (ESP_ERR_NVS_NOT_FOUND == found || NVS_TYPE_BLOB != type ||
        something[i].size != length) {
      // Set default config in NVS
      ESP_LOGW("NVM", "Config %s not found or size mismatch, setting default",
               something[i].name);
      ESP_ERROR_CHECK(nvs_set_blob(timer_nvs_handle, something[i].name,
                                   something[i].ptr, something[i].size));
      ESP_ERROR_CHECK(nvs_commit(timer_nvs_handle));
    }
  }
  nvs_close(timer_nvs_handle);
}
