#include "ThingSpeak.h"
#include "secrets.h"
#include <WiFi.h>
#include "Adafruit_Si7021.h"
#include <Wire.h>
#include <BMP180I2C.h>

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  120        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

#define BMP_ADDRESS 0x77

char ssid[] = SECRET_SSID;  
char pass[] = SECRET_PASS;   
int keyIndex = 0;

//Creating req. objects
WiFiClient  client;
Adafruit_Si7021 si7021 = Adafruit_Si7021();
BMP180I2C bmp180(BMP_ADDRESS);

//Thingspeak login preferences are pulled from secret.h file
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

//Variables for weather values
float temperature = 0.0;
float humidity = 0.0;
float pressure = 0.0;

//Information string for Wifi Connection
String myStatus = "";

int i, j = 0;
void setup() {

  //Communication beginning
  Wire.begin();
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);
  delay(10);
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
  if (!bmp180.begin())
  {
    Serial.println("begin() failed. check your BMP180 Interface and I2C Address.");
    while (1);
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  Serial.println("Si7021 test!");
  
  
  //Si7021 settings
  if (!si7021.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    while (true)
      ;
  }
  Serial.print(" Rev(");
  Serial.print(si7021.getRevision());
  Serial.print(")");
  Serial.print(" Serial #"); Serial.print(si7021.sernum_a, HEX); Serial.println(si7021.sernum_b, HEX);
  /////////////////////////////////////////////////////////////////////////////////////////////////////


  //BMP180 Settings
  bmp180.resetToDefaults();
  bmp180.setSamplingMode(BMP180MI::MODE_UHR);
  /////////////////////////////////////////////////////////////////////////////////////////////////////



  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
}

void loop() {
  for(i = 0; i<3; i++){
    if (!bmp180.measurePressure()) {
      Serial.println("could not start perssure measurement, is a measurement already running?");
      return;
    }
    do {
      delay(100);
    } while (!bmp180.hasValue());
  
    temperature = si7021.readTemperature();
    delay(100);
    humidity = si7021.readHumidity();
    delay(100);
    pressure = bmp180.getPressure() /100;
    Serial.print("Humidity:    ");
    Serial.print(humidity, 2);
    Serial.print("\tTemperature: ");
    Serial.println(temperature, 2);
    delay(10);
    
    if(i == 3 &&  pressure > 2140) {
      Serial.println("Pressure sensor error!");
      i = 0;
      j++;
    }
    if(j == 5){
      Serial.println("Pressure sensor connection error!");
      break;
    }
  }
  
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, pressure);
  if(j == 5) {
    ThingSpeak.setStatus("Pressure Sensor Error!");
    j = 0;
  }
  else ThingSpeak.setStatus("OK");
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
   else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  
  delay(500);
  esp_deep_sleep_start();
}
