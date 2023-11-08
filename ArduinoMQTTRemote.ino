#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// constant
const char* ssid = "WiFi's Name";
const char* password = "WiFi's Password";
const char* mqtt_server = "betterlight.local";
const int mqtt_port = 1883;
const char* mqtt_username = "";
const char* mqtt_password = "";
const char* client_name = "esp32";
const char* main_topic = "betterlight";
WiFiClient client;
PubSubClient mqtt(client);

const unsigned int debounceDelay = 50;
const unsigned int analogDelay = 50;
const unsigned int analogIgnoreRange = 100;
const int cap = JSON_ARRAY_SIZE(7) + 7*JSON_OBJECT_SIZE(5);

// variable
StaticJsonDocument<cap> doc;

unsigned int button1_pressTime = 0;
unsigned int button2_pressTime = 0;
unsigned int button3_pressTime = 0;
unsigned int button4_pressTime = 0;
unsigned int slider1_slideTime = 0;
unsigned int slider2_slideTime = 0;
unsigned int slider3_slideTime = 0;

void setup() {
  Serial.begin(115200);

  // Set up JSON
  doc[0]["type"] = "Button 1";
  doc[0]["pin"] = 18;
  doc[0]["current"] = 0;
  doc[0]["prev"] = 0;
  doc[0]["mode"] = 0;

  doc[1]["type"] = "Button 2";
  doc[1]["pin"] = 23;
  doc[1]["current"] = 0;
  doc[1]["prev"] = 0;
  doc[1]["mode"] = 0;

  doc[2]["type"] = "Button 3";
  doc[2]["pin"] = 19;
  doc[2]["current"] = 0;
  doc[2]["prev"] = 0;
  doc[2]["mode"] = 0;

  doc[3]["type"] = "Button 4";
  doc[3]["pin"] = 22;
  doc[3]["current"] = 0;
  doc[3]["prev"] = 0;
  doc[3]["mode"] = 0;

  doc[4]["type"] = "Slider 1";
  doc[4]["pin"] = 25;
  doc[4]["current"] = 0;
  doc[4]["prev"] = 0;
  doc[4]["mode"] = 0;

  doc[5]["type"] = "Slider 2";
  doc[5]["pin"] = 33;
  doc[5]["current"] = 0;
  doc[5]["prev"] = 0;
  doc[5]["mode"] = 0;

  doc[6]["type"] = "Slider 3";
  doc[6]["pin"] = 32;
  doc[6]["current"] = 0;
  doc[6]["prev"] = 0;
  doc[6]["mode"] = 0;
  
  // Set up PIN
  pinMode(doc[0]["pin"].as<int>(), INPUT_PULLUP);
  pinMode(doc[1]["pin"].as<int>(), INPUT_PULLUP);
  pinMode(doc[2]["pin"].as<int>(), INPUT_PULLUP);
  pinMode(doc[3]["pin"].as<int>(), INPUT_PULLUP);
  pinMode(doc[4]["pin"].as<int>(), INPUT);
  pinMode(doc[5]["pin"].as<int>(), INPUT);
  pinMode(doc[6]["pin"].as<int>(), INPUT);

  // Set up Time
  button1_pressTime = millis();
  button2_pressTime = millis();
  button3_pressTime = millis();
  button4_pressTime = millis();
  slider1_slideTime = millis();
  slider2_slideTime = millis();
  slider3_slideTime = millis();
  
  WiFi_connect();
  MQTTBroker_connect();
}

void WiFi_connect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void MQTTBroker_connect() {
  Serial.println("Start MQTT Connection Procedure.");
  mqtt.setServer(mqtt_server, mqtt_port);
  mqtt.setCallback(callback);
  mqtt.connect(client_name, mqtt_username, mqtt_password);

  if (mqtt.connected()) {
    Serial.println("Connect to MQTT Broker.");
  }
  while (!mqtt.connected()) {
    Serial.println("Connecting to MQTT Broker...");
    if (mqtt.connect(client_name)) {
      Serial.println("Connect to MQTT Broker.");
    }
    else {
      Serial.print("Failed with state ");
      Serial.print(mqtt.state());
      Serial.print(" ");
      delay(2000);
    }
  }

  for (int i = 0; i < 7; i++) {
    mqtt.subscribe(
      (String(main_topic) + "/" + String(doc[i]["type"].as<const char*>())).c_str()
    );
  }
  Serial.println("End MQTT Connection Procedure.");
}

void callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<cap> json_doc;
  deserializeJson(json_doc, payload, length);
}

void loop() {
  if (!mqtt.connected()) {
    MQTTBroker_connect();
  }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi_connect();
  }

  // button1
  doc[0]["current"] = digitalRead(doc[0]["pin"].as<int>());
  if (doc[0]["current"].as<int>() != doc[0]["prev"].as<int>()) {
    if (millis() - button1_pressTime > debounceDelay) {
      if (doc[0]["current"].as<int>() == 1 and doc[0]["prev"].as<int>() == 0) {
        doc[0]["mode"] = 1 - doc[0]["mode"].as<int>();
      }
      serializeJson(doc[0], Serial);
      if (mqtt.connect(client_name, mqtt_username, mqtt_password)) {
        char buffer[256];
        size_t n = serializeJson(doc[0], buffer);
        mqtt.publish(
          (String(main_topic) + "/" + String(doc[0]["type"].as<const char*>())).c_str(), buffer, n
        );
      }
      Serial.print("\n");
      doc[0]["prev"] = doc[0]["current"].as<int>();
      button1_pressTime = millis();
    }
  }

  // button2
  doc[1]["current"] = digitalRead(doc[1]["pin"].as<int>());
  if (doc[1]["current"].as<int>() != doc[1]["prev"].as<int>()) {
    if (millis() - button2_pressTime > debounceDelay) {
      if (doc[1]["current"].as<int>() == 1 and doc[1]["prev"].as<int>() == 0) {
        doc[1]["mode"] = 1 - doc[1]["mode"].as<int>();
      }
      serializeJson(doc[1], Serial);
      if (mqtt.connect(client_name, mqtt_username, mqtt_password)) {
        char buffer[256];
        size_t n = serializeJson(doc[1], buffer);
        mqtt.publish(
          (String(main_topic) + "/" + String(doc[1]["type"].as<const char*>())).c_str(), buffer, n
        );
      }
      Serial.print("\n");
      doc[1]["prev"] = doc[1]["current"].as<int>();
      button2_pressTime = millis();
    }
  }

  // button3
  doc[2]["current"] = digitalRead(doc[2]["pin"].as<int>());
  if (doc[2]["current"].as<int>() != doc[2]["prev"].as<int>()) {
    if (millis() - button3_pressTime > debounceDelay) {
      if (doc[2]["current"].as<int>() == 1 and doc[2]["prev"].as<int>() == 0) {
        doc[2]["mode"] = 1 - doc[2]["mode"].as<int>();
      }
      serializeJson(doc[2], Serial);
      if (mqtt.connect(client_name, mqtt_username, mqtt_password)) {
        char buffer[256];
        size_t n = serializeJson(doc[2], buffer);
        mqtt.publish(
          (String(main_topic) + "/" + String(doc[2]["type"].as<const char*>())).c_str(), buffer, n
        );
      }
      Serial.print("\n");
      doc[2]["prev"] = doc[2]["current"].as<int>();
      button3_pressTime = millis();
    }
  }

  // button4
  doc[3]["current"] = digitalRead(doc[3]["pin"].as<int>());
  if (doc[3]["current"].as<int>() != doc[3]["prev"].as<int>()) {
    if (millis() - button4_pressTime > debounceDelay) {
      if (doc[3]["current"].as<int>() == 1 and doc[3]["prev"].as<int>() == 0) {
        doc[3]["mode"] = 1 - doc[3]["mode"].as<int>();
      }
      serializeJson(doc[3], Serial);
      if (mqtt.connect(client_name, mqtt_username, mqtt_password)) {
        char buffer[256];
        size_t n = serializeJson(doc[3], buffer);
        mqtt.publish(
          (String(main_topic) + "/" + String(doc[3]["type"].as<const char*>())).c_str(), buffer, n
        );
      }
      Serial.print("\n");
      doc[3]["prev"] = doc[3]["current"].as<int>();
      button4_pressTime = millis();
    }
  }

  //slider1
  doc[4]["current"] = analogRead(doc[4]["pin"].as<int>());
  if (abs(doc[4]["current"].as<int>() - doc[4]["prev"].as<int>()) > analogIgnoreRange) {
    if (millis() - slider1_slideTime > analogDelay) {
      if (doc[4]["current"].as<int>() - doc[4]["prev"].as<int>() > 0) {
        doc[4]["mode"] = 1;
      }
      else {
        doc[4]["mode"] = 0;
      }
      serializeJson(doc[4], Serial);
      if (mqtt.connect(client_name, mqtt_username, mqtt_password)) {
        char buffer[256];
        size_t n = serializeJson(doc[4], buffer);
        mqtt.publish(
          (String(main_topic) + "/" + String(doc[4]["type"].as<const char*>())).c_str(), buffer, n
        );
      }
      Serial.print("\n");
      doc[4]["prev"] = doc[4]["current"].as<int>();
      slider1_slideTime = millis();
    }
  }

  //slider2
  doc[5]["current"] = analogRead(doc[5]["pin"].as<int>());
  if (abs(doc[5]["current"].as<int>() - doc[5]["prev"].as<int>()) > analogIgnoreRange) {
    if (millis() - slider2_slideTime > analogDelay) {
      if (doc[5]["current"].as<int>() - doc[5]["prev"].as<int>() > 0) {
        doc[5]["mode"] = 1;
      }
      else {
        doc[5]["mode"] = 0;
      }
      serializeJson(doc[5], Serial);
      if (mqtt.connect(client_name, mqtt_username, mqtt_password)) {
        char buffer[256];
        size_t n = serializeJson(doc[5], buffer);
        mqtt.publish(
          (String(main_topic) + "/" + String(doc[5]["type"].as<const char*>())).c_str(), buffer, n
        );
      }
      Serial.print("\n");
      doc[5]["prev"] = doc[5]["current"].as<int>();
      slider2_slideTime = millis();
    }
  }

  //slider3
  doc[6]["current"] = analogRead(doc[6]["pin"].as<int>());
  if (abs(doc[6]["current"].as<int>() - doc[6]["prev"].as<int>()) > analogIgnoreRange) {
    if (millis() - slider3_slideTime > analogDelay) {
      if (doc[6]["current"].as<int>() - doc[6]["prev"].as<int>() > 0) {
        doc[6]["mode"] = 1;
      }
      else {
        doc[6]["mode"] = 0;
      }
      serializeJson(doc[6], Serial);
      if (mqtt.connect(client_name, mqtt_username, mqtt_password)) {
        char buffer[256];
        size_t n = serializeJson(doc[6], buffer);
        mqtt.publish(
          (String(main_topic) + "/" + String(doc[6]["type"].as<const char*>())).c_str(), buffer, n
        );
      }
      Serial.print("\n");
      doc[6]["prev"] = doc[6]["current"].as<int>();
      slider3_slideTime = millis();
    }
  }

  mqtt.loop();
}
