#include <Arduino.h>
#include <unity.h>

#include "DS18B20.h"

#ifdef UNIT_TEST

using namespace g3rb3n;

#define PIN D1

void isConnected() 
{
  DS18B20 ds(PIN);
  Return<float> t = ds.temperature();
  TEST_ASSERT_EQUAL(true, t.valid());
  TEST_ASSERT_EQUAL(0, t.code());
}

void readsProperValue() 
{
  DS18B20 ds(PIN);
  Return<float> t = ds.temperature();
  TEST_ASSERT_EQUAL(true, t.valid());
  TEST_ASSERT_EQUAL(0, t.code());
  TEST_ASSERT_FLOAT_WITHIN(40, 21, t);
}

void setup() 
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);
  UNITY_BEGIN();

  RUN_TEST(isConnected);
}

void loop() 
{
  uint8_t count = 100;
  while(--count)
  {
    RUN_TEST(readsProperValue);
  }
  UNITY_END();
}

#endif
