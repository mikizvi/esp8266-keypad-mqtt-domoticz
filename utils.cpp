/**
 * utils.cpp
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

#include <Arduino.h>
#include "utils.h"

void setup_utils()
{
    pinMode(LED, OUTPUT);
}

String default_device_name(String prefix, String mac_addr)
{
  // MAC address           1
  // MAC address 01:34:67:90:23:56
  
  String ret;
  char mac_tail[7];
  const char *mac_str = mac_addr.c_str();
  
  if (strlen(mac_str) != 17) {
    Serial.printf("MAC address isn't 17 chars \"%s\"\n", mac_str);
    return prefix;
  }
  mac_tail[0] = mac_str[9];
  mac_tail[1] = mac_str[10];
  mac_tail[2] = mac_str[12];
  mac_tail[3] = mac_str[13];
  mac_tail[4] = mac_str[15];
  mac_tail[5] = mac_str[16];
  mac_tail[6] = '\0';
  ret = prefix + "_" + mac_tail;
  dbg_printf("MAC \"%s\", tail \"%s\", ret \"%s\"\n", mac_str, mac_tail, ret.c_str());
  return ret;
}
