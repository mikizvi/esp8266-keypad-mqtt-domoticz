# Generic ESP8266 MQTT/Domoticz Keypad (PlatformIO version)
Michael Hirsch  
January 2022

![Completed project](https://github.com/mikizvi/esp8266-keypad-mqtt-domoticz/blob/main/doc/final-project-small.jpg)

Use a keypad to control multiple devices.  Each key sends a different command.  I have a few installed at home, for example, at an entrance.  A few clicks when you enter/leave to switch different lights on or off.  Generally, the buttons send "toggle" commands so each button controls a different light, for example.

This project sets up an ESP8266 with a keypad to send a different Tasmota-style MQTT message or Domoticz http url command for each key pressed.

At startup, the code downloads its key assignments in the form of a json config file from a local http server.  This permits reprogramming by modifying the json config file.  If the config file is not understood for any reason, the configuration defaults to sending MQTT commands with a topic based on the last 6 hex digits of the MAC address (details below).

There are a few examples of techniques in this project that may make it interesting beyond just the keypad and the dynamic configuration:
1. How to connect to WiFi.  The red LED flashes on and off until connected.
2. How to get a file from an HTTP server
3. How to decode json
4. How to use MQTT as a publisher to send topics and messages
5. How to use MQTT as a subscriber, with a simple mechanism to re-subscribe after an MQTT reconnect
6. How to send http url commands to a Domoticz server
7. And of course, how to use a keypad with a ESP 8266.  The LED is turned on while the button is pressed.

The files are organized in a way that you should be able to pick out the relevant .h and .c file and use it in your project.  Of course, there are dependencies: look at the files included by the .cpp files.

## Deploying

The `secrets.h` idea is adopted from one of the projects below.  Your site configuration is in `secrets.h`. You will need to copy the `secrets.h.dist` to `secrets.h` and fill in your details.  If you plan to use only MQTT commands, then you don't need to fill in the Domoticz details and vice versa.

You may need to edit/change the way that MQTT topics are constructed depending on your site customizations.  See `mqtt_construct_topic()` in `mqtt.cpp`.

This project has been adapted for PlatformIO in VSCode.  This excellent [tutorial](https://www.electronicshub.org/programming-esp8266-using-vs-code-and-platformio/) was my guide to installing and first steps.
  I use the "D1 mini" board.  There are a few libraries that you will need to install:
1. [ArduinoJson](https://arduinojson.org/?utm_source=meta&utm_medium=library.properties) by Benoit Blanchon (6.18.5)
2. [Keypad](https://playground.arduino.cc/Code/Keypad/) by Mark Stanley, Alexander Brevig (3.1.1)
3. [PubSubClient](https://pubsubclient.knolleary.net/) by Nick O'Leary (2.8.0)

The configuration file is fetched from a http server.  You can disable this behavior by commenting out the `#define FETCH_CONFIG` in your secrets.h file.  My HTTP server runs on my internal LAN using a simple `/usr/bin/python3 -m http.server 8008`.  The code looks for the configuration files in the subdir `config`, named `config/\<MAC-ADDR>.json`, where the *MAC-ADDR* has all its colon (":") characters replaced by dashes ("-") because some file systems do not allow or do not like file names with colons.

There are example MQTT and Domoticz config files in the distribution.  The config file contains fields:
* **device**: a name for the device
  + The topic of this device uses this as the prefix and the IP address as the suffix
* **protocol**: "MQTT" or "Domoticz"
* **full-topic-format**: required for MQTT.  Usually `<operation>/<topic>/<command>` (see `mqtt_construct_topic()` in `mqtt.ccp`) where
  + *operation* is something like cmnd or stat
  + *topic* is usually the target device
  + *command* is something like power or power2.
* **buttons**: button assignements: a friendly name (for debug prints to serial), and (for MQTT) the destination topic and command or (for Domoticz) the idx

```
{
  "device": "keypad_test",
  "protocol": "MQTT",
  "full-topic-format": "%s/%s/%s",
  "buttons" : {
    "1": {
      "friendly-name": "Lounge Main",
      "topic": "lounge_main",
      "cmnd": "power"
    },
    "2": {
...
```

If there is no HTTP server for the configuration or if the configuration isn't valid json, and so on ... the device uses a default configuration.  In this case, the default topic is `keypad_<mac suffix>`, where *mac suffix* is the last 6 digits of the MAC address.  Each key press sends a `cmnd/<topic>/<key> toggle` command.  In the case of a "#" key, the character "P" is sent because '#' is an MQTT wildcard character.

This version subscribes to the `power` command (default full topic `cmnd/<topic>/power`) and accepts messages `on`, `off` and `toggle`.  These operate on the same LED that toggles on and off when a key is pressed.  Operations are acknowledged by publishing `stat/<topic>/POWER` and `stat/<topic>/RESULT`, copying Tasmota behavior.

## Schematics

![Schematics](https://github.com/mikizvi/esp8266-keypad-mqtt-domoticz/blob/main/doc/esp8266-mqtt-keypad.png)

As always, the values for these resistors are recommendations only.  Tune these to get your LEDs to the brightness you want.  The green power LED is optional anyway.

## Prototype

![Working prototype](https://github.com/mikizvi/esp8266-keypad-mqtt-domoticz/blob/main/doc/prototype-small.jpg)


## License

```
/**
 * README.md
 *
 * Created on: 2021-12-30
 *
 * Copyright (c) 2021, 2022 Michael Hirsch. All rights reserved.
 * This file is part of the "Generic ESP8266 Keypad" by Michael Hirsch.
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
```

## Similar projects

There are a few projects that do similar things:
1. [Tasmota](https://github.com/tasmota)
2. [Folientastatur/Keypad mit Tasmota](https://forum.iobroker.net/topic/35911/folientastatur-keypad-mit-tasmota)
3. [f0rdprefect/esp8266-keypad-mqtt](https://github.com/f0rdprefect/esp8266-keypad-mqtt)
4. [jaredsylvia/esp8266-keypad-mqtt](https://github.com/jaredsylvia/esp8266-keypad-mqtt)

## Keywords

ESP8266, keypad, MQTT, Domoticz, home automation, smart home, IOT, PlatformIO
