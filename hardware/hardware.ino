//##################################################################################################################
//##                                      ELET2415 DATA ACQUISITION SYSTEM CODE                                   ##
//##################################################################################################################

// LIBRARY IMPORTS
#include <rom/rtc.h>
#include <math.h>
#include <ctype.h>
#include <PubSubClient.h>
#include <FastLED.h>
#include "DHT.h"
#include <WiFiClient.h>
#include <WiFi.h>
#include <stdlib.h>
#include <stdio.h>
#include <Arduino.h>
#include <ArduinoJson.h>

// GLOBAL OBJECTS
WiFiClient espClient;
PubSubClient mqtt(espClient);

// DEFINE VARIABLES
#define ARDUINOJSON_USE_DOUBLE  1
#define NUM_LEDS 7
#define DATA_PIN 27

// DHT22
#define DHTPIN 26
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// MQTT CONFIG
static const char* pubtopic      = "620141171";
static const char* subtopic[]    = {"620141171_sub","/elet2415"};
// Keep broker in sync with backend MQTT client
static const char* mqtt_server   = "www.yanacreations.com";
static uint16_t mqtt_port        = 1883;

// WIFI CREDENTIALS
const char* ssid       = "troy's Galaxy A12";
const char* password   = "pgsh1846";

// TASK HANDLES
TaskHandle_t xMQTT_Connect       = NULL;
TaskHandle_t xNTPHandle          = NULL;
TaskHandle_t xLOOPHandle         = NULL;
TaskHandle_t xUpdateHandle       = NULL;
TaskHandle_t xButtonCheckeHandle = NULL;

// FUNCTION DECLARATIONS
void checkHEAP(const char* Name);
void initMQTT(void);
unsigned long getTimeStamp(void);
void callback(char* topic, byte* payload, unsigned int length);
void initialize(void);
bool publish(const char *topic, const char *payload);
void vButtonCheck(void * pvParameters);
void vUpdate(void * pvParameters);
bool isNumber(double number);
double convert_Celsius_to_fahrenheit(double c);
double convert_fahrenheit_to_Celsius(double f);
double calcHeatIndex(double Temp, double Humid);

// LED array
CRGB ledArray[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  // mqtt.setServer("broker.hivemq.com", 1883);

  // LED init
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(ledArray, NUM_LEDS);
  dht.begin();

  for(int x=0; x<7; x++){
    ledArray[x] = CRGB(240, 0, 240);
    FastLED.setBrightness(200);
    FastLED.show();
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
  for(int x=0; x<NUM_LEDS; x++){
    ledArray[x] = CRGB::Black;
    FastLED.setBrightness(200);
    FastLED.show();
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }

  initialize(); // WiFi, MQTT, tasks
}

void loop() {
  // Keep MQTT alive
  mqtt.loop();
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

//####################################################################
//#                          UTIL FUNCTIONS                          #
//####################################################################
void vButtonCheck(void * pvParameters) {
  configASSERT(((uint32_t) pvParameters) == 1);
  for(;;) {
    // Add button logic here
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void vUpdate(void * pvParameters) {
  configASSERT(((uint32_t) pvParameters) == 1);
  for(;;) {
    double h = dht.readHumidity();
    double t = dht.readTemperature();
    Serial.printf("Humidity: %.2f, Temp: %.2f\n", h, t);

    if(isNumber(t)){
      JsonDocument doc;
      char message[1100] = {0};
      doc["id"]        = "620141171";
      doc["timestamp"] = getTimeStamp();
      doc["temperature"] = t;
      doc["humidity"]    = h;
      doc["heatindex"]   = convert_fahrenheit_to_Celsius(
                              calcHeatIndex(convert_Celsius_to_fahrenheit(t), h));

      serializeJson(doc, message);

      if(mqtt.connected()){
        publish(pubtopic, message);
      }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

unsigned long getTimeStamp(void) {
  time_t now;
  time(&now);
  return now;
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("\nMessage received : ( topic: %s ) \n",topic);
  char *received = new char[length + 1]{0};
  for (int i = 0; i < length; i++) {
    received[i] = (char)payload[i];
  }
  Serial.printf("Payload : %s \n",received);

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, received);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  const char* type = doc["type"];
  if (strcmp(type, "controls") == 0){
    const int brightness = doc["brightness"];
    const int leds = doc["leds"];
    const int red = doc["color"]["r"];
    const int green = doc["color"]["g"];
    const int blue = doc["color"]["b"];

    for(int x=0; x<leds; x++){
      ledArray[x] = CRGB(red, green, blue);
      FastLED.setBrightness(brightness);
      FastLED.show();
      vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    for(int x=leds; x<NUM_LEDS; x++){
      ledArray[x] = CRGB::Black;
      FastLED.setBrightness(brightness);
      FastLED.show();
      vTaskDelay(50 / portTICK_PERIOD_MS);
    }
  }
}

bool publish(const char *topic, const char *payload){
  bool res = false;
  try{
    res = mqtt.publish(topic,payload);
    if(!res){
      throw false;
    }
  }
  catch(...){
    Serial.printf("\nError (%d) >> Unable to publish message\n", res);
  }
  return res;
}

double convert_Celsius_to_fahrenheit(double c){
  return (9.0/5.0)*c+32;
}

double convert_fahrenheit_to_Celsius(double f){
  return (f-32)*(5.0/9.0);
}

double calcHeatIndex(double Temp, double Humid){
  return -42.379 + (2.04901523 * Temp) + (10.14333127 * Humid)
         - (0.22475541 * Temp * Humid) - (0.00683783 * Temp * Temp)
         - (0.05481717 * Humid * Humid) + (0.00122874 * Temp * Temp * Humid)
         + (0.00085282 * Temp * Humid * Humid)
         - (0.00000199 * Temp * Temp * Humid * Humid);
}

bool isNumber(double number){
  char item[20];
  snprintf(item, sizeof(item), "%f\n", number);
  return isdigit(item[0]);
}

//####################################################################
//#                          INITIALIZE FUNCTION                     #
//####################################################################
void initialize(void) {
  // Connect WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());

  // Configure MQTT
  mqtt.setServer(mqtt_server, mqtt_port);
  mqtt.setCallback(callback);

  // Connect MQTT
  while (!mqtt.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqtt.connect("ESP32Client")) {
      Serial.println("connected");
      for (int i = 0; i < sizeof(subtopic)/sizeof(subtopic[0]); i++) {
        mqtt.subscribe(subtopic[i]);
      }
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" retry in 5 seconds");
      delay(5000);
    }
  }

  // Start FreeRTOS tasks
  xTaskCreatePinnedToCore(vUpdate, "UpdateTask", 4096, (void*)1, 1, &xUpdateHandle, 1);
  xTaskCreatePinnedToCore(vButtonCheck, "ButtonTask", 2048, (void*)1, 1, &xButtonCheckeHandle, 1);
}
