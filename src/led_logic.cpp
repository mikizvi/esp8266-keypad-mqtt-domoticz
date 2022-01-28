/**
 * led_logic.cpp
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
#include "mqtt.h"
#include "utils.h"
#include "led_logic.h"

static String command;
static String status;
static String result;
int led_state = LED_OFF;

void operate_led(int to_state)
{
    led_state = to_state;
    digitalWrite(LED, led_state);
}

static void led_mqtt_callback(void* user_data, const char* topic, byte* payload,
                              unsigned int length)
{
    char msg[256];
    String message;
    const char* res;

    if (length >= 256) {
        length = 255;
    }
    strncpy(msg, (const char*)payload, length);
    msg[length] = '\0';
    message = String(msg);
    dbg_printf("Received \"%s\" \"%s\"\n", topic, message.c_str());

    message.toLowerCase();
    if ((message == "on") || (message == "1")) {
        led_state = LED_ON;
    } else if ((message == "off") || (message == "0")) {
        led_state = LED_OFF;
    } else if (message == "toggle") {
        led_state = (led_state == LED_ON) ? LED_OFF : LED_ON;
    }
    digitalWrite(LED, led_state);
    res = (led_state == LED_ON) ? "ON" : "OFF";
    mqtt_publish(status.c_str(), res);
    sprintf(msg, "{\"POWER\":\"%s\"}", res);
    mqtt_publish(result.c_str(), msg);
}

static bool led_mqtt_subscribe(void* user_data)
{
    dbg_printf("Subscribing to \"%s\"\n", command.c_str());
    return mqtt_subscribe(command.c_str());
}

void setup_led(String full_topic_format, String device_name)
{
    command = mqtt_construct_topic(full_topic_format, "cmnd",
                                   device_name.c_str(), "power");
    status = mqtt_construct_topic(full_topic_format, "stat",
                                  device_name.c_str(), "POWER");
    result = mqtt_construct_topic(full_topic_format, "stat",
                                  device_name.c_str(), "RESULT");

    // dbg_printf("Setting up LED MQTT \"%s\" \"%s\" \"%s\"\n", command.c_str(),
    // status.c_str(), result.c_str());
    mqtt_add_callback(led_mqtt_callback, NULL);
    mqtt_add_subscribe(led_mqtt_subscribe, NULL);
    led_state = LED_OFF;
}

void loop_led() { }
