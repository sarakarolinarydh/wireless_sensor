#include <math.h>
#include <stdio.h>
#include <string>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h> // PubSubClient 2.8.0 by Nick O'Leary - https://pubsubclient.knolleary.net/
#include "DHT.h" // Adafruit DHT sensor library 1.4.6 - https://github.com/adafruit/DHT-sensor-library

#define DHTPIN D2 // DHT11
#define DHTTYPE DHT11 // Sensor instance
#define JSON_STACK 100
#define JSON_BUF 100
#define PUBLIC_TOPIC "sensor/DHT11"

// Internet connection
const char *ssid = "my_ssid";
const char *password = "my_internet_password";

// MQTT
const char *mqttServer = "192.168.1....";
const char *mqttUser = "my_mqtt_username";
const char *mqttPassword = "my_mqtt_password";
const char *sender_ID = "DHT11_sensor_1";
bool sendMQTTon = false;

// Read from sensor variables
unsigned long timeLastRead = 0;
unsigned long readInterval = 10000;
unsigned long currentMillis = 0;

// WiFi
WiFiClient espClient;
PubSubClient client(espClient);

// DHT sensor instance
DHT dht(DHTPIN, DHTTYPE);

// Set up WiFi
void setup_wifi()
{
  delay(100);
  Serial.println();
  Serial.println(sender_ID);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
}

// Connect to MQTT server
void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...\n");
    // Attempt to connect
    if (client.connect(sender_ID, mqttUser, mqttPassword))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc = ");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Main setup
void setup()
{
  Serial.begin(9600);
  Serial.setTimeout(2000);

  setup_wifi();
  client.setServer(mqttServer, 1883);

  // Sensor, DHT11
  dht.begin();
}

// Main loop
void loop()
{
  currentMillis = millis();

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  // Report every 30 seconds
  if (currentMillis - timeLastRead >= readInterval)
  {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again)
    if (isnan(h) || isnan(t) || isnan(f))
    {
      Serial.println("Failed to read from DHT sensor!");
      timeLastRead += 100; // Wait 100 ms and try to read from the sensor again
      return;
    }
    timeLastRead = currentMillis;

    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");

    StaticJsonDocument<JSON_STACK> JSONdoc;
    JsonArray values = JSONdoc.createNestedArray("values");

    values.add(h);
    values.add(t);
    values.add(f);
    values.add(hic);
    values.add(hif);

    char JSONmessageBuffer[JSON_BUF];
    serializeJson(JSONdoc, JSONmessageBuffer);
    Serial.println("Sending message to MQTT topic..");
    Serial.println(JSONmessageBuffer);
    client.publish(PUBLIC_TOPIC, JSONmessageBuffer);
  }
}
