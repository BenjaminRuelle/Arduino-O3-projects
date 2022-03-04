#include "DHT.h"

DHT dht(7, DHT11);

#define IRpin 2
#define lightPin 4
#define ventPin 5
#define buzzer 3
#define ventSwitch 10
#define lightSwitch 11

unsigned long currentTime = 0;
unsigned long previousTime = 0;
bool lightState = false;

long temperature;
long humidity;

void setup() {
  
 pinMode(IRpin,INPUT);
 pinMode(ventSwitch,INPUT);
 pinMode(lightSwitch,INPUT);
 
 pinMode(lightPin,OUTPUT);
 pinMode(ventPin,OUTPUT);
 pinMode(buzzer,OUTPUT);

 dht.begin();
 Serial.begin(9600);
}

void loop() {
  
  currentTime = millis();
  
  // Reading sensors
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  
  // Switch management
  if(digitalRead(ventSwitch) == HIGH){
    TurnOnVent();
  }
  if(digitalRead(ventSwitch) == LOW){
    TurnOffVent();
  }
  if(digitalRead(lightSwitch) == HIGH){
    TurnOnLight();
  }
  if(digitalRead(lightSwitch) == LOW){
    TurnOffLight();
  }
  
  if(motionDetection()==1){
    
    lightState = true;
    TurnOnLight();
    
    if(temperature > 28){
      TurnOnVent();
    }
 }
 
 delay(200);
}

bool motionDetection(){
  if(digitalRead(IRpin)==HIGH){
   Serial.println("Mouvement detected !");
   buzzerBip();
   return true;
  }
  if(digitalRead(IRpin)==LOW){
   return false;
  }
}

void lightTimer(){
 if(lightState = true && (currentTime - previousTime) > 300000){ //5 min
     previousTime = 0;
     currentTime = 0;
     lightState = false;
     TurnOffLight();
   }
}

void TurnOnLight(){
  digitalWrite(lightPin, HIGH);
  Serial.println("Turn on light");
}

void TurnOffLight(){
  digitalWrite(lightPin, LOW);
  Serial.println("Turn off light");
}

void TurnOnVent(){
  digitalWrite(ventPin, HIGH);
  Serial.println("Turn on vent");
}

void TurnOffVent(){
  digitalWrite(ventPin, LOW);
  Serial.println("Turn off vent");
}

void buzzerBip(){
  tone(buzzer, 1000); 
  delay(500);        
  noTone(buzzer);
}
