/**
 * esp8266_gen_keypad.ino
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

#include <map>
#include <Arduino.h>
#include "secrets.h"
#include "wifi.h"
#include "mqtt.h"
#include "domoticz.h"
#include "http.h"
#include "keypad_logic.h"
#include "led_logic.h"
#include "utils.h"

static String protocol;
static String full_topic_format
    = "%s/%s/%s"; // this is the default, may be overridden by config file

void setup()
{
    Serial.begin(115200);
    delay(10);
    Serial.println("Generic 4x4 keypad for MQTT and Domoticz");
    String device_name;

    setup_utils();
    setup_wifi(SSID_NAME, SSID_PASSWORD);
    device_name = default_device_name(DEVICE_NAME_PREFIX, mac_addr);
    setup_keypad(http_server, http_port, http_user, http_password, http_uri,
                 mac_addr, device_name, protocol, full_topic_format);
    if (protocol == "MQTT") {
        setup_mqtt(device_name, MQTT_SERVER, MQTT_PORT, MQTT_USER,
                   MQTT_PASSWORD);
        setup_led(full_topic_format, device_name);
    } else if (protocol == "Domoticz") {
        domoticz_setup(DOMOTICZ_SERVER, DOMOTICZ_PORT, DOMOTICZ_USER,
                       DOMOTICZ_PASSWORD);
    } else {
        Serial.printf("Unrecognized protocol \"%s\"\n", protocol.c_str());
    }
}

void loop()
{
    if (protocol == "MQTT") {
        loop_mqtt();
        loop_led();
    }
    loop_keypad(protocol);

    // save some power
    delay(1);
}
