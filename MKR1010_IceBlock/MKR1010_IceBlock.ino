/*
  Sketch generated by the Arduino IoT Cloud Thing "Untitled"
  https://create.arduino.cc/cloud/things/51e429e2-f67d-4736-8f0c-549c907a2834

  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  float humidity;
  float temperature;
  float waterLevel;
  CloudTime eeom;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include "thingProperties.h"
#include "DHT.h"
#include <ArduinoHttpClient.h>
#include <DallasTemperature.h>

DHT dht(7, DHT11);

/************************/
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 2 
/************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

long temp1;
long temp2;
long temp3;
long temp4;

int code;
int period = 1000;
unsigned long time_now = 0;

const char kHostname[] = "op-dev.icam.fr";
WiFiClient client;
HttpClient http(client, kHostname);
char url;

void setup() {
  Serial.begin(9600);
  dht.begin();
  sensors.begin();
  
  delay(1500);

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
  if (millis() >= time_now + period) {
    time_now += period;

    sensors.requestTemperatures();
    temp1 = sensors.getTempCByIndex(0);
    temp2 = sensors.getTempCByIndex(1);
    temp3 = sensors.getTempCByIndex(2);
    temp4 = sensors.getTempCByIndex(3);

    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    waterLevel = analogRead(A0);

    calculEEOM(temp1, temp2, temp3, temp4);
    serial_print();
    
    char url[100];
    sprintf(url, "https://op-dev.icam.fr/~icebox/createTweeticam.php?author=Q3&message=++%d++&secretkey=fcbe5", eeom);
    http.get(url);
    Serial.print(url);
    }
}

void calculEEOM(long temp1, long temp2, long temp3, long temp4) {
  eeom = random(0, 100);
  //Do some magic here
}

void serial_print() {
  Serial.print("Temperature is ");
  Serial.print(temperature);
  Serial.print("°");
  Serial.print(" / Humidity is ");
  Serial.print(humidity);
  Serial.print("%" );
  Serial.print(" / EEOM value is ");
  Serial.print(eeom);
  Serial.print(" / Water level is ");
  Serial.print(waterLevel);
  Serial.println("%" );

}
