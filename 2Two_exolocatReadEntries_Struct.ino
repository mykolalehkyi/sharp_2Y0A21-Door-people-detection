/*
  Two dual HC-SR04 with Temp and Humidity product
  Equipment: 2 HC-SR04,DHT11 or DHT22;
  Calculates the number of passes through the first and second sensors with detailed information.
  Calculates the number of entrances and exits from the room.
  With DHT11 Temperature and Humidity Sensor
  Displays results on Serial Monitor

=======================================================================================
|                                       Pins Connections 
=======================================================================================
|                      HC-SR04(first)   |    HC-SR04(second)    |    DHT-11(DHT22)  
|                   Trig  Echo  Vcc Gnd |   Trig  Echo  Vcc Gnd |   Vcc+  Gnd-  Data
|   Arduino Mega     3     4    5V  GND |    5     6     5V GND |    5V   GND    7
|   Arduino Due      3     4    5V  GND |    5     6     5V GND |    5V   GND    7   
========================================================================================
*/

// Include DHT Libraries from Adafruit
// Dependant upon Adafruit_Sensors Library
#include "DHT.h"//version 1.2.3

// Include NewPing Library
#include "NewPing.h"

// Define Constants

#define DHTPIN 7       // DHT-11 Output Pin connection
#define DHTTYPE DHT11   // DHT Type is DHT 11 (AM2302)
#define TRIGGER_PIN_1  3
#define ECHO_PIN_1     4
#define TRIGGER_PIN_2  5
#define ECHO_PIN_2     6
#define MAX_DISTANCE 400

NewPing sonar1(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE);
NewPing sonar2(TRIGGER_PIN_2, ECHO_PIN_2, MAX_DISTANCE);

// Define Variables
int counter12=0; //Number of entries in room
int counter21=0;  //Number of leaves the room
float delta=10; //Ignore value
float hum;    // Stores humidity value in percent
float temp;   // Stores temperature value in Celcius
float soundsp;  // Stores calculated speed of sound in M/S
float soundcm;  // Stores calculated speed of sound in cm/ms
int iterations = 3;

struct exolocatVar{
  float etalon; //Etalon of doors for exolocator
  float duration; // Stores First HC-SR04 pulse duration value
  float distance; // Stores calculated distance in cm for Sensor
}exotr1,exotr2;

// Initialize DHT sensor for normal 16mhz Arduino

DHT dht(DHTPIN, DHTTYPE); 

void setup() {
  Serial.begin (9600);
  dht.begin();
  etalon();
}

void loop()
{   
    hum = dht.readHumidity();  // Get Humidity value
    temp= dht.readTemperature();  // Get Temperature value
    
    // Calculate the Speed of Sound in M/S
    soundsp = 331.4 + (0.606 * temp) + (0.0124 * hum);
    
    // Convert to cm/ms
    
  soundcm = soundsp / 10000;
  
  // Measure duration for first sensor
    delay(25);  
  exotr1.duration = sonar1.ping_median(iterations);
  
  // Add a delay between sensor readings
  
  delay(25);
  
  // Measure duration for first sensor
  
  exotr2.duration = sonar2.ping_median(iterations);
  
  // Calculate the distances for both sensors
  
  exotr1.distance = (exotr1.duration / 2) * soundcm;
  exotr2.distance = (exotr2.duration / 2) * soundcm;
  
  // Send results to Serial Monitor
    distOut();

  if(exotr1.distance<=exotr1.etalon-delta || exotr1.distance>=exotr1.etalon+delta)
  check2();
  else if (exotr2.distance<=exotr2.etalon-delta || exotr2.distance>=exotr2.etalon+delta)
  check1();
}

void check2(){
  for(int i=0;i<10;i++){
  delay(50);
   hum = dht.readHumidity();  // Get Humidity value
    temp= dht.readTemperature();  // Get Temperature value
    
    // Calculate the Speed of Sound in M/S
    soundsp = 331.4 + (0.606 * temp) + (0.0124 * hum);
    
    // Convert to cm/ms
    
  soundcm = soundsp / 10000;

  // Measure duration for second sensor
  
  exotr2.duration = sonar2.ping_median(iterations);
  
  // Calculate the distance of sensor
  exotr2.distance = (exotr2.duration / 2) * soundcm;
  if(exotr2.distance<=exotr2.etalon-delta || exotr2.distance>=exotr2.etalon+delta){
  counter12++;
  distOut();
  out();
  return;
    }
  }
}


void check1(){
  for(int i=0;i<10;i++){
  delay(50);
   hum = dht.readHumidity();  // Get Humidity value
    temp= dht.readTemperature();  // Get Temperature value
    
    // Calculate the Speed of Sound in M/S
    soundsp = 331.4 + (0.606 * temp) + (0.0124 * hum);
    
    // Convert to cm/ms
    
  soundcm = soundsp / 10000;

  // Measure duration for second sensor
  
  exotr1.duration = sonar1.ping_median(iterations);
  
  // Calculate the distance of sensor
  exotr1.distance = (exotr1.duration / 2) * soundcm;
  if(exotr1.distance<=exotr1.etalon-delta || exotr1.distance>=exotr1.etalon+delta){
  counter21++;
  distOut();
  out();
  return;
    }
  }
}


void  etalon(){
  Serial.println("Stabilasing tempreture and humidity...");
   Serial.println("Getting etalon values: ...");
    delay(1000);  // Delay so DHT-11 sensor can stabalize

    hum = dht.readHumidity();  // Get Humidity value
    temp= dht.readTemperature();  // Get Temperature value
    
    // Calculate the Speed of Sound in M/S
    soundsp = 331.4 + (0.606 * temp) + (0.0124 * hum);
    
    // Convert to cm/ms
    
  soundcm = soundsp / 10000;
  
  // Measure duration for first sensor
    
  exotr1.duration = sonar1.ping_median(10);
  
  // Add a delay between sensor readings
  
  delay(100);
  
  // Measure duration for first sensor
  
  exotr2.duration = sonar2.ping_median(10);
  
  // Calculate the distances for both sensors
  
  exotr1.distance = (exotr1.duration / 2) * soundcm;
  exotr2.distance = (exotr2.duration / 2) * soundcm;
  
  exotr1.etalon=exotr1.distance;
  exotr2.etalon=exotr2.distance;
  Serial.print("Etalon 1: ");

    if (exotr1.distance >= 400 || exotr1.distance <= 2) {
    Serial.print("Out of range ");
    }
    else {
    Serial.print(exotr1.distance);
    Serial.print(" cm ");
    }
    
    Serial.print("Etalon 2: ");

    if (exotr2.distance >= 400 || exotr2.distance <= 2) {
    Serial.print("Out of range");
    }
    else {
    Serial.print(exotr2.distance);
    Serial.print(" cm");
    }
  
  Serial.println(" ");
}
void distOut(){
  Serial.print("Distance 1: ");

    if (exotr1.distance >= 400 || exotr1.distance <= 2) {
    Serial.print("Out of range");
    }
    else {
    Serial.print(exotr1.distance);
    Serial.print(" cm ");
    }
    
    Serial.print("Distance 2: ");

    if (exotr2.distance >= 400 || exotr2.distance <= 2) {
    Serial.print("Out of range");
    }
    else {
    Serial.print(exotr2.distance);
    Serial.println(" cm");
    }
  Serial.print("Sound: ");
    Serial.print(soundsp);
    Serial.print(" m/s, ");
    Serial.print("Humid: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.print(" C, ");
  Serial.println(" ");
}

void out(){
  Serial.print("Number of entries: ");
  Serial.print(counter12);
  Serial.print(". ");
  Serial.print("Number of exits: ");
  Serial.print(counter21);
  Serial.println(". ");
  Serial.print("People in: ");
  Serial.print(max(counter12-counter21,0));
  Serial.print(" Visits: ");
  Serial.println(min(counter12,counter21));
  delay(1500);
  }
//Ультразвуковий датчик він залежить від погоди, вологості, температури. Акустичні чи електричні шуми збивають його. Потрібна weather resistant version. Звуковий сенксор мусить працювати паралельно щоб інший код не зупиняв його. .
//Ультразвуковий датчик він залежить від погоди, вологості, температури. Акустичні чи електричні шуми збивають його. І кут обєкта також його збиває. Потрібна weather resistant version.
//ІR sensor вимірює за допомогою інфрачервоного і тому страждає від сонця.
//Рішення VL53L0X laser sernor Вимірює до 200 см, але на практиці 125 см.
//Але найкраще купити лазер і модуль приймач і тоді вже точно все буде чекатись до 20 м 
/* // NODE MCU
#define D0 16
#define D1   5 // I2C Bus SCL (clock)
#define D2   4 // I2C Bus SDA (data)
#define D3   0
#define D4   2 // Same as "LED_BUILTIN", but inverted logic
#define D5 14 // SPI Bus SCK (clock)
#define D6 12 // SPI Bus MISO 
#define D7 13 // SPI Bus MOSI
#define D8 15 // SPI Bus SS (CS)
#define D9   3 // RX0 (Serial console)
#define D10 1 // TX0 (Serial console)
 */
