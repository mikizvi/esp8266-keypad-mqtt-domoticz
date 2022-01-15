/**
 * keypad_logic.cpp
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
#include <ArduinoJson.h>
#include <Keypad.h>
#include "mqtt.h"
#include "http.h"
#include "domoticz.h"
#include "utils.h"
#include "keypad_logic.h"

static const byte ROWS = 4; // define row 4
static const byte COLS = 4; // define column 4
static char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// connect row ports of the button to corresponding IO ports on the board
static byte rowPins[ROWS] = {D7, D6, D5, D0};

// connect column ports of the button to corresponding IO ports on the board
static byte colPins[COLS] = {D1, D2, D3, D4};

// call class library performance function of Keypad
static Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

static const char *cmnd_message = "toggle";

typedef std::map<char, String> MapKeyToString;
static MapKeyToString mapKeyToCmnd;
static MapKeyToString mapKeyToDevice;

typedef std::map<char, int> MapKeyToIdx;
static MapKeyToIdx mapKeyToIdx;

static String construct_mqtt_cmnd(const char *full_topic_format, const char *destination_device, const char *command)
{
  char str[256];
  snprintf(str, sizeof(str), full_topic_format, "cmnd", destination_device, command);
  return String(str);
}

static void create_default_mqtt_config(String device_name, String &protocol)
{
  // Note that '#' is generally not allowed as a topic because it is a wildcard
  int row, col;
  char key;
  char key_s[2] = {'\0', '\0'};
  const char *full_topic_format = "%s/%s/%s";
  const char *topic = device_name.c_str();
  protocol = "MQTT";
  for (row = 0; row < ROWS; row++) {
    for (col = 0; col < COLS; col++) {
      key = keys[row][col];
      key_s[0] = key;
      if (key == '#') {
        key_s[0] = 'P';
      }
      mapKeyToDevice[key] = key_s;
      String full_command = construct_mqtt_cmnd(full_topic_format, topic, key_s);
      dbg_printf("key: \"%c\", cmnd: \"%s\"\n", key, full_command.c_str());
      mapKeyToCmnd[key] = full_command;
    }
  }
}

static bool get_json_config(
  String http_server,
  uint16_t http_port,
  String http_user,
  String http_password,
  String http_uri,
  String mac_addr,
  String &device_name,
  String &protocol)
{
#ifndef FETCH_CONFIG
  return false;
#else // FETCH_CONFIG
  uint32_t i;
  String url = http_uri + mac_addr + ".json";
  int http_code;
  String payload;

  dbg_printf("Http json URL: %s\n", (const char *) url.c_str());

  payload = http_get(http_server, http_port, http_user, http_password, url, http_code);  //Specify request destination
  if (http_code <= 0) {
    dbg_printf("HTTP failed with %d\n", http_code);
    return false;
  }
  else {
    DynamicJsonDocument jsonDoc(8196);
    auto error = deserializeJson(jsonDoc, payload);
    if (error) {
      Serial.printf("deserializeJson() failed with code \"%s\" \"%s\"\n", error.c_str(), payload.c_str());
      return false;
    }
    JsonObject jsonRoot = jsonDoc.as<JsonObject>();;

    // Test if parsing succeeds.
    if (jsonRoot.isNull()) {
      Serial.println("parseObject() failed");
      Serial.println(payload);
      return false;
    }
    const char *full_topic_format = "";
    device_name = String(jsonRoot["device"]);
    protocol = String(jsonRoot["protocol"]);
    if (protocol == "MQTT") {
       full_topic_format = jsonRoot["full-topic-format"];
    }

    dbg_printf("Protocol = \"%s\", Full topic format = \"%s\"\n", protocol.c_str(), full_topic_format);
    JsonObject buttons = jsonRoot["buttons"];
    i = 0;
    for (JsonObject::iterator it = buttons.begin(); it != buttons.end(); ++it) {
      i++;
      JsonObject button = it->value();
      const char *device = button["friendly-name"];

      char key = it->key().c_str()[0];
      mapKeyToDevice[key] = device;
      if (protocol == "MQTT") {
        const char *topic = button["topic"];
        const char *cmnd = button["cmnd"];
        String full_command = construct_mqtt_cmnd(full_topic_format, topic, cmnd);
        dbg_printf("key: \"%c\", device: \"%s\", topic: \"%s\", cmnd: \"%s\", \"%s\"\n",
          key, device, topic, cmnd, full_command.c_str());
        mapKeyToCmnd[key] = full_command;
      }
      else if (protocol == "Domoticz") {
        const int idx = button["idx"];
        dbg_printf("key: \"%c\", device: \"%s\", idx: %d\n", key, device, idx);
        mapKeyToIdx[key] = idx;
      }
      else {
        dbg_printf("Unknown protocol %s\n", protocol.c_str());
      }
    }
    dbg_printf("%u devices\n", i);
  }
  return true;
#endif // FETCH_CONFIG
}

void setup_keypad(
  String http_server,
  uint16_t http_port,
  String http_user,
  String http_password,
  String http_uri,
  String mac_addr,
  String &device_name,
  String &protocol)
{
  bool config_done;
  config_done = get_json_config(http_server, http_port, http_user, http_password, http_uri, mac_addr, device_name, protocol);

  if (!config_done) {
    create_default_mqtt_config(device_name, protocol);
  }
}

void loop_keypad(String protocol)
{
  if (keypad.keyStateChanged()) {
    KeyState keyState = keypad.getState();
    //    dbg_printf("state changed to %d\n", keyState);
    if (keyState == IDLE) {
      digitalWrite(D8, LOW);
    }
    else {
      digitalWrite(D8, HIGH);
    }
  }

  // This is where the actual work gets done
  char key = keypad.getKey();
  if (key != NO_KEY) {
    String device = mapKeyToDevice[key];
    if (protocol == "MQTT") {
      String mqtt_cmnd = mapKeyToCmnd[key];
      dbg_printf("Key: %c, device: \"%s\", mqtt_cmnd: \"%s\"\n", key, device.c_str(), mqtt_cmnd.c_str());
      if (mqtt_cmnd != "") {
        mqtt_publish(mqtt_cmnd.c_str(), cmnd_message);
      }
    }
    else if (protocol == "Domoticz") {
      int idx = mapKeyToIdx[key];
      int http_code;
      String res = domoticz_toggle(idx, http_code);
      dbg_printf("DTZ %c \"%s\" %d %d %s\n", key, device.c_str(), idx, http_code, res.c_str());
    }
    else {
      dbg_printf("Unknown protocol \"%s\"\n", protocol.c_str());
    }
  }
}
