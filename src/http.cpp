/**
 * http.cpp
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
#include "secrets.h"
#include "wifi.h"
#include "http.h"

// HTTP connection (for config file)
String http_server(HTTP_SERVER);
const uint16_t http_port = HTTP_PORT;
String http_user(HTTP_USER);
String http_password(HTTP_PASSWORD);
String http_uri(HTTP_URI);

#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#define HTTP_CLIENT HTTPClient
#else // !ESP8266
#include <HttpClient.h>
#include <WiFiEsp.h>
#define HTTP_CLIENT HttpClient
#endif // ESP8266

String http_get(String http_server, uint16_t http_port, String http_user,
                String http_password, String http_url, int& http_code)
{
    String payload;
    HTTP_CLIENT http; // Declare an object of class HTTPClient

    http.begin(wifi_client, http_server, http_port,
               http_url); // Specify request destination
    if ((http_user != "") || (http_password != "")) {
        http.setAuthorization(http_user.c_str(), http_password.c_str());
    }
    http_code = http.GET();
    if (http_code > 0) {
        payload = http.getString();
    }

    http.end();

    return payload;
}
