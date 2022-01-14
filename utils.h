/**
 * utils.h
 *
 * Created on: 2021-12-24
 *
 * Copyright (c) 2021, 2022 Michael Hirsch. All rights reserved.
 * This file is part of the "Generic ESP8266 Keypad" project by Michael Hirsch.
 *
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef UTILS_H
#define UTILS_H

#include "device-specific.h"

#ifdef DEBUG
#define dbg_printf Serial.printf
#else // DEBUG
#define dbg_printf(...)
#endif // DEBUG

// LED that works (usually to give feedback)
// used by MQTT connection code to show connecting
#ifdef ESP8266
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
#ifndef LED
#define LED D4
#endif // LED
#define LED_ON  HIGH
#define LED_OFF LOW
#else
#define LED BUILTIN_LED
#define LED_ON  LOW
#define LED_OFF HIGH
#endif
#else // ESP8266
#ifndef LED
#define LED BUILTIN_LED
#endif // LED
#define LED_ON  HIGH
#define LED_OFF LOW
#endif // ESP8266

void setup_utils();

String default_device_name(String prefix, String mac_addr);

#endif // UTILS_H
