/**
 * mqtt.cpp
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
#include "mqtt.h"

// MQTT creds
static const char* mqtt_save_user;
static const char* mqtt_save_password;

// values set after connection made
static String mqtt_client_id;

static PubSubClient mqtt_client(wifi_client);

#define IpAddress2String(ipAddress)                                            \
    String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".")    \
        + String(ipAddress[2]) + String(".") + String(ipAddress[3])

typedef std::pair<callback_t, void*> callback_record;
std::vector<callback_record> cbk_callbacks;

typedef std::pair<subscribe_t, void*> subscribe_record;
std::vector<subscribe_record> cbk_subscribes;

void mqtt_add_callback(callback_t callback, void* user_data)
{
    cbk_callbacks.push_back(callback_record(callback, user_data));
}

void mqtt_add_subscribe(subscribe_t subscribe, void* user_data)
{
    cbk_subscribes.push_back(subscribe_record(subscribe, user_data));
}

static void callback(const char* topic, byte* payload, unsigned int length)
{
    for (auto const& cbk : cbk_callbacks) {
        cbk.first(cbk.second, topic, payload, length);
    }
}

static void subscribe()
{
    for (auto const& cbk : cbk_subscribes) {
        cbk.first(cbk.second);
    }
}

void setup_mqtt(String device_name, const char* mqtt_server, int mqtt_port,
                const char* mqtt_user, const char* mqtt_password)
{
    mqtt_save_user = mqtt_user;
    mqtt_save_password = mqtt_password;
    mqtt_client_id = device_name + "-" + ip_addr;
    dbg_printf("Using MQTT client id \"%s\"\n", mqtt_client_id.c_str());

    mqtt_client.setServer(mqtt_server, mqtt_port);
    mqtt_client.setCallback(callback);
}

static void reconnect()
{
    // Loop until we're reconnected
    while (!mqtt_client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqtt_client.connect(mqtt_client_id.c_str(), mqtt_save_user,
                                mqtt_save_password)) {
            Serial.println("connected");
            subscribe();
        } else {
            Serial.printf("failed, rc=%d", mqtt_client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void loop_mqtt()
{
    if (!mqtt_client.connected()) {
        reconnect();
    }
    mqtt_client.loop();
}

bool mqtt_publish(const char* topic, const char* payload)
{
    dbg_printf("\"%s\": \"%s\"\n", topic, payload);
    return mqtt_client.publish(topic, payload);
}

bool mqtt_subscribe(const char* topic) { return mqtt_client.subscribe(topic); }

String mqtt_construct_topic(String full_topic_format, const char* action,
                            const char* destination_device, const char* command)
{
    char str[256];
    snprintf(str, sizeof(str), full_topic_format.c_str(), action,
             destination_device, command);
    return String(str);
}
