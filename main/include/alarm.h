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
#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/FreeRTOS.h"
#include <stdint.h>

typedef struct {
  uint16_t num_cycles;
  uint16_t num_beeps;
  float cycle_freq;
  float beep_freq;
  uint32_t tone_freq;
} alarm_config_t;

typedef enum {
  ALARM_FINISHED_WORKING = 0,
  ALARM_FINISHED_RESTING,
  ALARM_BEEP,
  ALARM_MAX,
} alarm_t;

extern TaskHandle_t task_alarm_handle;

void init_alarm();

#ifdef __cplusplus
}
#endif
