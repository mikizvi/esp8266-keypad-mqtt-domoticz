/**
 * wifi.h
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

#ifndef WIFI_H
#define WIFI_H

#ifdef ESP8266
#include <ESP8266WiFi.h>
#define WIFI_CLIENT WiFiClient
#else // !ESP8266
#include <WiFiEsp.h>
#define WIFI_CLIENT WiFiEspClient
#endif // ESP8266

// Wifi connection
extern WIFI_CLIENT wifi_client;

// values set after connection made

extern String ip_addr;
extern String mac_addr;

void setup_wifi(const char* ssid, const char* password);

#endif // WIFI_H
