/**
 * domoticz.cpp
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
#include "wifi.h"
#include "domoticz.h"

static String domoticz_save_server;
static uint16_t domoticz_save_port;
static String domoticz_save_user;
static String domoticz_save_password;

void domoticz_setup(
  String domoticz_server,
  const uint16_t domoticz_port,
  String domoticz_user,
  String domoticz_password)
{
  domoticz_save_server = domoticz_server;
  domoticz_save_port = domoticz_port;
  domoticz_save_user = domoticz_user;
  domoticz_save_password = domoticz_password;
}

// HTTP connection (for config file)


#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#define HTTP_CLIENT HTTPClient
#else // !ESP8266
#include <WiFiEsp.h>
#include <HttpClient.h>
#define HTTP_CLIENT HttpClient
#endif // ESP8266

String domoticz_toggle(unsigned int idx, int &http_code)
{
  String payload;
  HTTP_CLIENT http_client;  //Declare an object of class HTTPClient
  String domoticz_url = String("/json.htm?type=command&param=switchlight&idx=") + idx + "&switchcmd=Toggle";

  http_client.begin(wifi_client, domoticz_save_server, domoticz_save_port, domoticz_url);  //Specify request destination
  if ((domoticz_save_user != "") || (domoticz_save_password != "")) {
    http_client.setAuthorization(domoticz_save_user.c_str(), domoticz_save_password.c_str());
  }
  http_code = http_client.GET();
  if (http_code > 0) {
    payload = http_client.getString();
  }

  http_client.end();

  return payload;
}
