#include <TimeLib.h>

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

#include <SPI.h>
#include <SD.h>
#define CS_PIN 53
File myFile;

const int IRpin = A0;          // аналоговый пин для подключения выхода Vo сенсора
const int iTimes=5; //змінна для сер.знач.
//int value1;                    // для хранения аналогового значения
int PeopleCou=0; 

void setup(){
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  setSyncProvider(requestSync);  //set function to call when sync required
  Serial.println("Waiting for sync message");
  initializeSD();
  myFile = SD.open("test.txt", FILE_WRITE);
  //writeFile("Everything work");
  readFile();
}

void  loop(){  
         
  if (Serial.available()) {
    processSyncMessage();
  }
  if (timeStatus()!= timeNotSet) {
    //digitalClockDisplay();  
    Serial.println(digitalClockString());
  }

 float volts = irRead()*0.0048828125;
 // и в расстояние в см 
 float distance=32*pow(volts,-1.10);
 CheckDistance(distance);
  
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}
String digitalClockString(){
  // digital clock display of the time
  String str="[";
  str+=hour();
  int digits=minute();
  str+=":";
  if(digits < 10)
    str+='0';
  str+=digits;
  digits=second();
  str+=":";
  if(digits < 10)
    str+='0';
  str+=digits;
  str+=" ";
  str+=day();
  str+=".";
  str+=month();
  str+=".";
  str+=year();
  str+="]";  
  return str;
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
     }
  }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}


void printDigits(byte digits){
  // utility function for digital clock display: prints colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits,DEC);   
}

void initializeSD(){
  Serial.print("Initializing SD card...");

  if (!SD.begin(CS_PIN)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
}
void writeFile(String str){

  myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println(str);
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  } 
}

void readFile(){
// re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}
// Усреднение нескольких значений для сглаживания
int irRead() {
  float averaging = 0;             //  переменная для суммирования данных
  // Получение iTimes значений
  float value1;
  for (int i=0; i<iTimes; i++) {
    value1 = analogRead(IRpin);
    //Serial.println(value1);
    averaging = averaging + value1;
    delay(10);      // Ожидание 55 ms перед каждым чтением
  }
  averaging = averaging / iTimes;      // усреднить значения
  return(averaging);              
} 

void CheckDistance(float distance){
  Serial.println(distance);      // выдаем в порт значение
  if(distance<40 || distance>115){
    PeopleCou++;
     Serial.println(digitalClockString()+"Object detected. Counted: "+PeopleCou);
     writeFile(digitalClockString()+"Object detected. Counted: "+PeopleCou);
    delay(1500);
  }
}
