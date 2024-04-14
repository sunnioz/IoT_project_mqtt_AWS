#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
    // DHT 11
#define lamp1 13
#define lamp2 12
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"
 
float h ;
float t;
 
 
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
 
void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println("Connecting to Wi-Fi");
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
 
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
 
  // Create a message handler
  client.setCallback(messageHandler);
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }
 
  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }
 
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  Serial.println("AWS IoT Connected!");
}
 
 
void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);
  String received_msg = "";
  for (int i = 0; i < length; i++) 
  {
    received_msg += (char)payload[i];
    Serial.print((char)payload[i]);
  }
  Serial.println();
 
  // Control the lamps based on the received message
  // Check for commands and actuate corresponding lamp
  if (received_msg == "ON1") 
  {
    digitalWrite(lamp1, HIGH); // Turn on lamp 1
    Serial.println("Lamp1 turned on");
  }
  else if (received_msg == "OFF1") 
  {
    digitalWrite(lamp1, LOW); // Turn off lamp 1
    Serial.println("Lamp1 turned off");
  }
    else if (received_msg == "ON2") 
  {
    digitalWrite(lamp2, HIGH);
    Serial.println("Lamp2 turned on");
  }
  else if (received_msg == "OFF2") 
  {
    digitalWrite(lamp2, LOW);
    Serial.println("Lamp2 turned off");
  }
  // StaticJsonDocument<200> doc;
  // deserializeJson(doc, payload);
  // const char* message = doc["message"];
  // Serial.println(message);
}
 
void setup()
{
  Serial.begin(115200);
  connectAWS();
  pinMode(lamp1, OUTPUT);
  digitalWrite(lamp1, LOW);
  pinMode(lamp2, OUTPUT);
  digitalWrite(lamp2, LOW);
}
 
void loop()
{
  client.loop();
}