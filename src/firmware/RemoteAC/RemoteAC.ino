#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "U+NetC760"
#define STAPSK "392J$F66C2"
#endif

#define RELAY_TRIGGER_PIN 0
#define STATUS_DSPLAY_LED_PIN 1

#define SERAIL_BAUD_RATE 9600

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);

void RootHandler()
{
    digitalWrite(STATUS_DSPLAY_LED_PIN, true);
    server.send(200, "text/plain", "System root page\r\n");
    digitalWrite(STATUS_DSPLAY_LED_PIN, false);
}

void handleNotFound()
{
    digitalWrite(STATUS_DSPLAY_LED_PIN, true);
    String message = "File Not Found\n\n";
    message += "\nURI: " + server.uri();
    message += "    - Method: " + (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "    - Arguments: " + server.args();
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i);
    }

    server.send(404, "text/plain", message);
    digitalWrite(STATUS_DSPLAY_LED_PIN, false);
}

void setup(void)
{
    pinMode(STATUS_DSPLAY_LED_PIN, OUTPUT);
    pinMode(RELAY_TRIGGER_PIN, OUTPUT);

    digitalWrite(STATUS_DSPLAY_LED_PIN, true);
    digitalWrite(RELAY_TRIGGER_PIN, true);

    WiFi.mode(WIFI_STA);

    Serial.begin(SERAIL_BAUD_RATE);
    WiFi.begin(ssid, password);

    Serial.print("Wait for connect");
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("esp8266"))
    {
        Serial.println("MDNS responder started");
    }

    server.on("/", RootHandler);
    server.on("/on", []()
    {
        digitalWrite(RELAY_TRIGGER_PIN, false);
        server.send(200, "text/plain", "on");
    });
    server.on("/off", []()
    {
        digitalWrite(RELAY_TRIGGER_PIN, true);
        server.send(200, "text/plain", "off");
    });

    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("HTTP server started");
}

void loop()
{
    server.handleClient();
    MDNS.update();
}