/**
 * wifi.cpp
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

#include <vector>
#include <Arduino.h>
#include <IPAddress.h>
#include <PubSubClient.h>
#include "utils.h"
#include "wifi.h"

// values set after connection made
String ip_addr;
String mac_addr;

#ifdef ESP8266
#define WIFI_INIT WiFi.printDiag(Serial);  WiFi.mode(WIFI_STA);
#define MAC_ADDR WiFi.macAddress()
#else // !ESP8266
#define WIFI_INIT Serial1.begin(115200);  WiFi.init(&Serial1);
#define MAC_ADDR get_mac_addr()
static String get_mac_addr()
{
  char s[32];
  uint8_t mac_addr[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(mac_addr);
  sprintf(s, "%0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x\n",
          mac_addr[5], mac_addr[4], mac_addr[3], mac_addr[2], mac_addr[1], mac_addr[0]);
  return s;
}
#endif // ESP8266

// Presumably wifi_client is set magically by the WiFi library
WIFI_CLIENT wifi_client;

#define IpAddress2String(ipAddress) String(ipAddress[0]) + String(".") + \
  String(ipAddress[1]) + String(".") + \
  String(ipAddress[2]) + String(".") + \
  String(ipAddress[3])

void setup_wifi(const char *ssid, const char *password)
{
  int state = LED_ON;
  WIFI_INIT;
  dbg_printf("Connecting to %s\n", ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED, state);
    if (state == LED_ON) {
      state = LED_OFF;
    }
    else {
      state = LED_ON;
    }
    delay(500);
    Serial.print(".");
  }
  digitalWrite(LED, LED_OFF);

  ip_addr = IpAddress2String(WiFi.localIP());
  mac_addr = MAC_ADDR;

  Serial.printf("\nWiFi connected.  IP \"%s\" MAC \"%s\"\n", ip_addr.c_str(), mac_addr.c_str());
}
