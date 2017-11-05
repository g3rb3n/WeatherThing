#include <Thing.h>
#include <BlinkPattern.h>
#include <DS18B20.h>
#include <DHT11.h>

using namespace g3rb3n;

typedef unsigned int uint;
typedef unsigned long ulong;

#define DTH11_PIN D2
#define ONE_WIRE_PIN D1
#define TEMPERATURE_PRECISION 12 // Lower resolution

ulong buff[100];
byte bits[50];
byte bytes[5];


Thing thing;
BlinkPattern led(BUILTIN_LED);
DS18B20 ds(ONE_WIRE_PIN);
DHT11 dht11(DTH11_PIN);

BlinkPattern::Pattern<2> panic{{1,1},25};
BlinkPattern::Pattern<2> start{{1,9},25};
BlinkPattern::Pattern<2> normal{{1,39},25};

void setup()
{
  Serial.begin(230400);
  Serial.println();
  Serial.println("ClientID:" + thing.clientId());

  led.setPattern(start);

  thing.onStateChange([](const String& msg){
    Serial.println(msg);
  });

  thing.begin();

  thing.addSensor(thing.clientId() + "/ws/dht11/humidity", 5000, [](Value& value){
    value = dht11.humidity();
    Serial.println(String("dht11 humidity ") + (float)value);
  });

  thing.addSensor(thing.clientId() + "/ws/dht11/temperature", 5000, [](Value& value){
    value = dht11.temperature();
    Serial.println(String("dht11 temperature ") + (float)value);
  });

  thing.addSensor(thing.clientId() + "/ws/ds18b20/temperature", 5000, [](Value& value){
    Return<float> temperature = ds.temperature();
    if (!temperature.valid())
    {
      Serial.println(String("Error ") + temperature.code());
      led.setPattern(panic);
      return;
    }
    Serial.println(String("ds18b20 temperature ") + (float)temperature);
    led.setPattern(normal);
    value = (float)temperature;
  });

  thing.addActuator(thing.clientId() + "/ws/display", [](Value& value){
    Serial.println("Got " + String(value));
  });

  ds.begin();
}

void loop()
{
  thing.handle();
  led.handle();
}
