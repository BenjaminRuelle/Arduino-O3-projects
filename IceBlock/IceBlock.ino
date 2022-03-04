/**
  This program is the one of the group 1 of Quito concerning the Ice block Challenge 2022
  
  The system is based on an Arduino Uno and an ESP8266. The program reads the temperature and humidity values of the box in order to calculate the time remaining before the ice block is completely melted.
  The read values are available on an online ThingsSpeaks dashboard at the address: https://thingspeak.com/channels/1637538/shared_view
  And the status of the box and the EEOM is sent to the icam website: https://op-dev.icam.fr/~icebox/readExperience.php?idexperience=31
 **/
 
#include <DallasTemperature.h>
#include <OneWire.h>
#include <SoftwareSerial.h>
#include "DHT.h"

#define  RX 10
#define  TX 11
#define ONE_WIRE_BUS 2 
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

SoftwareSerial my_ESP8266_WIFI(RX,TX);

DHT dht(4, DHT11);

int Waterlvl;

//Buzzer song variable
const int buzzerPin =7 ;
const int songLength = 18;
char notes[] = "cdfda ag cdfdg gf "; 
int beats[] = {1,1,1,1,1,1,4,4,2,1,1,1,1,1,1,4,4,2};
int tempo = 113;

//Define temperature variable : tempX are for DS18 sensors and temperature are for DHT11
int temp1;
int temp2;
int temp3;
int temp4;
long temperature;
long humidity;

//Define Network configuration
//String SSID = "Timmy";       // CHANGE ME
//String PASSCODE = "Antben00"; // CHANGE ME
String SSID = "Claro_CORAL";       // CHANGE ME
String PASSCODE = "1712722261"; 
//String SSID = "Selina CoWork";      
//String PASSCODE = "GetShitDone"; 

//Define API configuration
String API_WRITE_KEY = "0RSUY8MLOJ80AXBJ";   // Dashboard API Key
String API_ICAM_KEY = "fcbe5";               // Icam twitt API Key
String API_ICAM_KEY_EXPERIENCE = "daee";     // Icam experience API Key
String SERVERthingspeak = "api.thingspeak.com";
String SERVERicam = "op-dev.icam.fr";
String PORT = "80";
String ID_EXPERIENCE = "31";
String GROUP_NAME = "Q1_Quito";

//Variable for Icam API
String STATUS;    //Status of the cube[1:Creation of the experience, 2:Start of the experience, 3:End 
int eeom;         //Time before melting in second

int countTrueCommand;
int countTimeCommand;
boolean found = false;

//Millis loop       
long previousMillis = 0;
long interval = 10000; 

void setup() {
  pinMode(8,OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  dht.begin();
  sensors.begin();  
  Serial.begin(115200);
  my_ESP8266_WIFI.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=3",5,"OK");
  sendCommand("AT+CWJAP=\""+ SSID +"\",\""+ PASSCODE +"\"",20,"OK");

}

void loop() {
  //Reading sensors
  delay(1000);
  Waterlvl = analogRead(A0);
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();     
  temp1 = sensors.getTempCByIndex(0);
  temp2 = sensors.getTempCByIndex(1);

  //Checking Status
  STATUS = cubeStatus(); 

  //Calculation of EEOM
  eeom = EEOM_Calculation(temperature, temp1, temp2, 20);
  
unsigned long currentMillis = millis(); 
 
if(currentMillis - previousMillis > interval) {
     previousMillis = currentMillis;
    
     updateEEOM(String(eeom));
     updateStatus(STATUS);
     updateDashboard(temp1,temp2);
     Serial.println("Update to API Icam");
  }
  
  Serial.print("Temperature 1 ");  
  Serial.println(temp1);
  Serial.print("Temperature 2 ");
  Serial.println(temp2);
  Serial.print("EEOM :");
  Serial.println(eeom);
  Serial.print("Temperature DHT :");
  Serial.println(temperature);
  Serial.print("Humidity DHT :");
  Serial.println(humidity);
  Serial.print("Water Level :");
  Serial.println(Waterlvl);
  Serial.print("Experience Status :");
  Serial.println(STATUS);  

}

///============================== Update Dashboard ===========================
void updateDashboard(int temperature_1, int temperature_2){

 String getData = "GET /update?api_key="+ API_WRITE_KEY + "&field1=" + String(temperature_1) + "&field2=" + String(temperature_2); 
 sendCommand("AT+CIPMUX=1",5,"OK"); // connects to multiple network
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ SERVERthingspeak  +"\","+ PORT,15,"OK"); // start communication
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");  // send data to server
 my_ESP8266_WIFI.println(getData); 
 //delay(1500);
 countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");// end connection
 Serial.println("Update Dashboard"); 
 
}

///=============================== Update EEOM ===========================
void updateEEOM(String EEOM){
  
 String getData = "GET /~icebox/createPrediction.php?idexperience="+ ID_EXPERIENCE + "&prediction=" + EEOM + "&secretkey=" + API_ICAM_KEY_EXPERIENCE;
 sendCommand("AT+CIPMUX=1",5,"OK"); // connects to multiple network
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ SERVERicam +"\","+ PORT,15,"OK"); // start communication
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");  // send data to server
 my_ESP8266_WIFI.println(getData);
 //delay(1500); 
 countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");// end connection
 Serial.println("Update Eeom"); 
 
}
///=============================== UpdateSatus ===========================
void updateStatus(String Status){
  
 String getData = "GET /~icebox/changeExperienceStatus.php?idexperience="+ ID_EXPERIENCE + "&newStatus=" + Status + "&secretkey=" + API_ICAM_KEY_EXPERIENCE;
 sendCommand("AT+CIPMUX=1",5,"OK"); // connects to multiple network
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ SERVERicam +"\","+ PORT,15,"OK"); // start communication
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");  // send data to server
 my_ESP8266_WIFI.println(getData);
 //delay(1500); 
 countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");// end connection
 Serial.println("Update Status"); 
}

///=============================== Send data ===========================
void sendCommand(String command, int maxTime, char readReplay[]) {
      Serial.print(countTrueCommand);
      Serial.print(". at command => ");
      Serial.print(command);
      Serial.print(" ");
      while(countTimeCommand < (maxTime*1)){
           my_ESP8266_WIFI.println(command);//at+cipsend
        if(my_ESP8266_WIFI.find(readReplay)){   //ok
            found = true;
            break;
          }
        countTimeCommand++;
      }
      
      if(found == true){
        Serial.println("SUCCESS");
        countTrueCommand++;
        countTimeCommand = 0;
      }
      
      if(found == false){
          Serial.println("Fail");
          countTrueCommand = 0;
          countTimeCommand = 0;
        }
        found = false;
    }
    
///================================ Buzzer Sound ===========================
void buzzerplay(){
int i, duration;
  for (i = 0; i < songLength; i++){
    duration = beats[i] * tempo;
    if (notes[i] == ' '){
      delay(duration);
    }else{
      tone(buzzerPin, frequency(notes[i]), duration);
      delay(duration);
      }
    delay(tempo/10);}
  while(true){}
  }
  
int frequency(char note){
  int i;
  const int numNotes = 8; 
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int frequencies[] = {262, 294, 330, 349, 392, 440, 494, 523};
  for (i = 0; i < numNotes; i++){
    if (names[i] == note){
      return(frequencies[i]);
    }
  }
  return(0);}

///================================ EEOM Calculation ===========================
int EEOM_Calculation(int temp1,int temp2,int temp3,int temp4){
 int eeom = random(0,100);
 return eeom;
}


///================================ Status of Cube ===========================
String cubeStatus(){
  String cubeStatus; 
  Waterlvl = analogRead(A0);
  
  if(Waterlvl >= 600){
    cubeStatus = "2";
    buzzerplay(); 
    digitalWrite(8,HIGH);
  } 
  else if(Waterlvl >= 50){
    cubeStatus = "1";  
    digitalWrite(8,LOW); 
  }
  else{
    cubeStatus = "0";   
    digitalWrite(8,LOW);
  }
  return cubeStatus;
} 
