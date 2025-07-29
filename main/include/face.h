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

#include "freertos/FreeRTOS.h"

typedef struct {
  float percentage;
} face_config_t;

typedef enum {
  FACE_NORMAL = 0,
  FACE_MAX,
} face_t;

extern TaskHandle_t task_face_handle;

void init_face();

bool set_face_angle(float angle);

bool set_face_angle_isr(float angle, bool *higherPriorityTaskWoken);

#ifdef __cplusplus
}
#endif
