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

portMUX_TYPE app_spinlock = portMUX_INITIALIZER_UNLOCKED;

config_none_t config_none;
config_work_t config_work;
config_rest_t config_rest;
config_finished_working_t config_finished_working;
config_finished_resting_t config_finished_resting;
config_set_working_t config_set_working;
config_set_resting_t config_set_resting;
config_reset_t config_reset;
config_io_t config_io;
