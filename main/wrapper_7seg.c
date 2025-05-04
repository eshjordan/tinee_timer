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

#include "wrapper_7seg.h"
#include "config.h"
#include "esp_log_level.h"

static tm1637_led_t *config_7seg;

void init_7seg() { config_7seg = tm1637_init(GPIO_NUM_5, GPIO_NUM_18); }

void set_number_7seg(uint16_t number) {
  taskENTER_CRITICAL(&app_spinlock);
  esp_log_level_t gpio_log_level = esp_log_level_get("gpio");
  esp_log_level_set("gpio", ESP_LOG_NONE);
  tm1637_set_number(config_7seg, number);
  esp_log_level_set("gpio", gpio_log_level);
  taskEXIT_CRITICAL(&app_spinlock);
}

void set_number_lead_dot_7seg(uint16_t number, const bool lead_zero,
                              const uint8_t dot_mask) {
  taskENTER_CRITICAL(&app_spinlock);
  esp_log_level_t gpio_log_level = esp_log_level_get("gpio");
  esp_log_level_set("gpio", ESP_LOG_NONE);
  tm1637_set_number_lead_dot(config_7seg, number, lead_zero, dot_mask);
  esp_log_level_set("gpio", gpio_log_level);
  taskEXIT_CRITICAL(&app_spinlock);
}

void set_segment_raw_7seg(uint8_t segment_idx, uint8_t data) {
  taskENTER_CRITICAL(&app_spinlock);
  esp_log_level_t gpio_log_level = esp_log_level_get("gpio");
  esp_log_level_set("gpio", ESP_LOG_NONE);
  tm1637_set_segment_raw(config_7seg, segment_idx, data);
  esp_log_level_set("gpio", gpio_log_level);
  taskEXIT_CRITICAL(&app_spinlock);
}
