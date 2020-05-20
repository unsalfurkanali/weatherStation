#include "Adafruit_Si7021.h"
#include <ThingerESP32.h>
#include <Arduino.h>
#include <Wire.h>
#include <BMP180I2C.h>

#define USERNAME "furkanunsal"
#define DEVICE_ID "1864"
#define DEVICE_CREDENTIAL "ceydali"

#define SSID "TurkTelekom_ZRCVH"
#define SSID_PASSWORD "06dAd8354B7B8"

ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

Adafruit_Si7021 sensor = Adafruit_Si7021();

BMP180I2C bmp180(0x77);

float temperature = 0;
float humidity = 0;
float pressure = 0;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  thing.add_wifi(SSID, SSID_PASSWORD);
  // wait for serial port to open
  while (!Serial) {
    delay(10);
  }
  if (!bmp180.begin())
  {
    Serial.println("begin() failed. check your BMP180 Interface and I2C Address.");
    while (1);
  }
  Serial.println("Si7021 test!");

  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    while (true)
      ;
  }

  Serial.print("Found model ");
  switch(sensor.getModel()) {
    case SI_Engineering_Samples:
      Serial.print("SI engineering samples"); break;
    case SI_7013:
      Serial.print("Si7013"); break;
    case SI_7020:
      Serial.print("Si7020"); break;
    case SI_7021:
      Serial.print("Si7021"); break;
    case SI_UNKNOWN:
    default:
      Serial.print("Unknown");
  }
  Serial.print(" Rev(");
  Serial.print(sensor.getRevision());
  Serial.print(")");
  Serial.print(" Serial #"); Serial.print(sensor.sernum_a, HEX); Serial.println(sensor.sernum_b, HEX);

  bmp180.resetToDefaults();
  bmp180.setSamplingMode(BMP180MI::MODE_UHR);
  thing["weather"] >> [](pson& out){     
    out["temperature"] = temperature;
    out["humidity"] = humidity;
    out["pressure"] = pressure;
  };
}

void loop() {
  if (!bmp180.measurePressure())
    {
    Serial.println("could not start perssure measurement, is a measurement already running?");
    return;
    }
    do
    {
    delay(100);
    } while (!bmp180.hasValue());
  temperature = sensor.readTemperature();
  delay(100);
  humidity = sensor.readHumidity();
  delay(100);
  pressure = bmp180.getPressure() /100;
  Serial.print("Humidity:    ");
  Serial.print(humidity, 2);
  Serial.print("\tTemperature: ");
  Serial.println(temperature, 2);
  delay(10);
  thing.handle();
  thing.stream(thing["weather"]);
  delay(1000);
  
}
