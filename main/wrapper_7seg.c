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
#include "esp_log.h"
#include "esp_log_level.h"
#include "esp_timer.h"
#include <string.h>
#include <time.h>

static tm1637_led_t *config_7seg;

static const int8_t tm1637_symbols[] = {
    // XGFEDCBA
    0x3f, // 0b00111111,    // 0
    0x06, // 0b00000110,    // 1
    0x5b, // 0b01011011,    // 2
    0x4f, // 0b01001111,    // 3
    0x66, // 0b01100110,    // 4
    0x6d, // 0b01101101,    // 5
    0x7d, // 0b01111101,    // 6
    0x07, // 0b00000111,    // 7
    0x7f, // 0b01111111,    // 8
    0x6f, // 0b01101111,    // 9
    0x77, // 0b01110111,    // A
    0x7c, // 0b01111100,    // b
    0x39, // 0b00111001,    // C
    0x5e, // 0b01011110,    // d
    0x79, // 0b01111001,    // E
    0x71, // 0b01110001     // F
    0x40, // 0b01000000     // minus sign
};

static const char *TAG = "7SEG";
static char str[] = "0000.";

void init_7seg() { config_7seg = tm1637_init(GPIO_NUM_5, GPIO_NUM_18); }

void set_number_7seg(uint16_t number) {
  static int64_t prev_time = 0;
  snprintf(str, sizeof(str), "%04d", number);

  int64_t new_time = esp_timer_get_time();
  if (new_time - prev_time > 500000) {
    ESP_LOGI(TAG, "%s", str);
    prev_time = new_time;
  }

  taskENTER_CRITICAL(&app_spinlock);
  esp_log_level_t gpio_log_level = esp_log_level_get("gpio");
  esp_log_level_set("gpio", ESP_LOG_NONE);
  tm1637_set_number(config_7seg, number);
  esp_log_level_set("gpio", gpio_log_level);
  taskEXIT_CRITICAL(&app_spinlock);
}

void set_number_lead_dot_7seg(uint16_t number, const bool lead_zero,
                              const uint8_t dot_mask) {
  static int64_t prev_time = 0;
  snprintf(str, sizeof(str), "%04d", number);
  str[4] = (dot_mask & 0b0001) ? '.' : '\0';

  int64_t new_time = esp_timer_get_time();
  if (new_time - prev_time > 500000) {
    ESP_LOGI(TAG, "%s", str);
    prev_time = new_time;
  }

  taskENTER_CRITICAL(&app_spinlock);
  esp_log_level_t gpio_log_level = esp_log_level_get("gpio");
  esp_log_level_set("gpio", ESP_LOG_NONE);
  tm1637_set_number_lead_dot(config_7seg, number, lead_zero, dot_mask);
  esp_log_level_set("gpio", gpio_log_level);
  taskEXIT_CRITICAL(&app_spinlock);
}

void set_segment_raw_7seg(uint8_t segment_idx, uint8_t data) {
  static int64_t prev_time = 0;
  static char tmp_chr[] = "";
  int found_idx = 0;
  for (found_idx = 0;
       found_idx < sizeof(tm1637_symbols) / sizeof(tm1637_symbols[0]);
       found_idx++) {
    if (data == tm1637_symbols[found_idx]) {
      break;
    }
  }
  if (found_idx < (sizeof(tm1637_symbols) / sizeof(tm1637_symbols[0])) - 1) {
    itoa(found_idx, tmp_chr, 16);
    str[segment_idx - 1] = tmp_chr[0];
  } else if (found_idx ==
             (sizeof(tm1637_symbols) / sizeof(tm1637_symbols[0])) - 1) {
    str[segment_idx - 1] = '-';
  } else {
    str[segment_idx - 1] = ' ';
  }

  int64_t new_time = esp_timer_get_time();
  if (new_time - prev_time > 500000) {
    ESP_LOGI(TAG, "%s", str);
    prev_time = new_time;
  }

  taskENTER_CRITICAL(&app_spinlock);
  esp_log_level_t gpio_log_level = esp_log_level_get("gpio");
  esp_log_level_set("gpio", ESP_LOG_NONE);
  tm1637_set_segment_raw(config_7seg, segment_idx, data);
  esp_log_level_set("gpio", gpio_log_level);
  taskEXIT_CRITICAL(&app_spinlock);
}
