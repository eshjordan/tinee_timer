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

#include "freertos/FreeRTOS.h"

#include "button_types.h"
#include "esp_log_level.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/gptimer_types.h"
#include "tm1637.h"
#include <time.h>

extern portMUX_TYPE app_spinlock;

typedef struct {
} config_none_t;

extern config_none_t config_none;

typedef struct {
  gptimer_handle_t timer_handle;
  struct timespec timer_duration;
} config_work_t;

extern config_work_t config_work;

typedef struct {
  gptimer_handle_t timer_handle;
  struct timespec timer_duration;
} config_rest_t;

extern config_rest_t config_rest;

typedef struct {
  gptimer_handle_t timer_handle;
  struct timespec timer_duration;
} config_finished_working_t;

extern config_finished_working_t config_finished_working;

typedef struct {
  gptimer_handle_t timer_handle;
  struct timespec timer_duration;
} config_finished_resting_t;

extern config_finished_resting_t config_finished_resting;

typedef struct {
} config_set_working_t;

extern config_set_working_t config_set_working;

typedef struct {
} config_set_resting_t;

extern config_set_resting_t config_set_resting;

typedef struct {
} config_reset_t;

extern config_reset_t config_reset;

typedef struct {
  button_handle_t btn_mode;
  button_handle_t btn_plus;
  button_handle_t btn_minus;
  button_handle_t btn_play;
} config_io_t;

extern config_io_t config_io;

#ifdef __cplusplus
}
#endif
