/**
 * mqtt.h
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

#ifndef MQTT_H
#define MQTT_H

/*
Subscribe mechanism

The MQTT library loses all subscribers every reconnect.
This code maintains a list of functions to do the subcribes after every
reconnect. Use mqtt_add_subscribe() to add a subscribe_t function to this list.
Your subscribe_t function must call mqtt_subscribe() to do the actual
subscription.

Example:

static bool do_subscribe(void *user_data)
{
  return mqtt_subscribe(((my_struct_t *) user_data)->my_topic);
}

void my_setup()
{
  mqtt_add_subscribe(do_subscribe, p_my_struct);
}
*/

/*
Callback mechanism

This code maintains a list of functions that are called whenever a subscribed
topic triggers a callback. Use mqtt_add_callback() to add a callback_t function
to this list.
*/

typedef void (*callback_t)(void* user_data, const char* topic, uint8_t* payload,
                           unsigned int length);
typedef bool (*subscribe_t)(void* user_data);

void setup_mqtt(String device_name, const char* mqtt_server, int mqtt_port,
                const char* mqtt_user, const char* mqtt_password);
void loop_mqtt();
bool mqtt_publish(const char* topic, const char* payload);
void mqtt_add_callback(callback_t callback, void* user_data);
void mqtt_add_subscribe(subscribe_t subscribe, void* user_data);
String mqtt_construct_topic(String full_topic_format, const char* action,
                            const char* destination_device,
                            const char* command);

// Call this function from inside the subscribe() function added using
// mqtt_add_subscribe()
bool mqtt_subscribe(const char* topic);

#endif // MQTT_H
