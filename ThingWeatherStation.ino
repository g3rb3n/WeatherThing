#define MQTT_CLIENT "WeatherStation"
#define MQTT_SERVER "10.0.0.66"
#define MQTT_PORT 1883
#define MQTT_USER "gerbenvaneerten@gmail.com"
#define MQTT_PASSWORD "Dwe57Kj8"

#include <Thing.h>

Thing thing;

#include <OneWire.h>
#include <DallasTemperature.h>

//#define ONE_WIRE_BUS 2  // DS18B20 pin
#define ONE_WIRE_BUS D1
#define TEMPERATURE_PRECISION 12 // Lower resolution

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

#define pinDHT11 D2
typedef unsigned int uint;
typedef unsigned long ulong;

ulong buff[100];
byte bits[50];
byte bytes[5];

#include <DHT11.h>
DHT11 dht11(D2);

std::string client;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(230400);

  char chipid[40];
  sprintf(chipid, "%08X", ESP.getChipId());
  client += chipid;
  Serial.printf("Client:%s\n", client.c_str());

  thing.onActivity(&onActivity);

  int interval = 10000;
  thing.addWiFi("woonkamer", "ongratis");
  thing.addWiFi("central", "ongratis");
  thing.addWiFi("zuid", "ongratis");
  thing.addWiFi("rfln", "ongratis");
  thing.setupWiFi();
  thing.setMQTT(MQTT_SERVER, MQTT_PORT, client.c_str(), MQTT_USER, MQTT_PASSWORD);
  thing.addTopic("sensors/weather/dht11/humidity", interval, dht11_humidity);
  thing.addTopic("sensors/weather/dht11/temperature", interval, dht11_temperature);
  thing.addTopic("sensors/weather/ds18b20/temperature", interval, ds18b20_temperature);
  thing.subscribe("displays/weather", callback);

  DS18B20.begin();
  DS18B20.getAddress(tempDeviceAddress, 0);
  DS18B20.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
  digitalWrite(BUILTIN_LED, HIGH);  
}

void ds18b20_temperature(float &value){
  float temp = -127;
  uint count = 0;
//  while(temp == -127 && count++ < 10){
    DS18B20.requestTemperatures(); 
    temp = DS18B20.getTempCByIndex(0);
    Serial.printf("DS18B20 temp %f\n", temp);
//  }
  value = temp;
}

void dht11_humidity(float &value){
  value = dht11.humidity();
  Serial.printf("DHT11   hum  %f\n", value);
}

void dht11_temperature(float &value){
  value = dht11.temperature();
  Serial.printf("DHT11   temp %f\n", value);
}

void onActivity(bool active){
  digitalWrite(BUILTIN_LED, active ? LOW : HIGH);  
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char msg[length+1];
  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
  }
  msg[length] = 0;
//  display(msg);
  Serial.println(msg);
}

void loop() {
  thing.loop();
}


