#include <Wire.h>
#include <Time.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 
#define userID 3;
#define ID 1;
#define IP_SERVER "192.168.0.182";
#define SISS "inneti-wifi";
#define PASSWORD "2367111b";

#include <SPI.h>
#include <SD.h>
#define CS_PIN 53
File myFile;
String fileName = "test5.txt";
 tmElements_t tm;
const int iTimes = 5; //змінна для сер.знач.
//int value1;                    // для хранения аналогового значения
struct S_people
{
  int in;
  int out;
  int count;
};
S_people people = {0, 0, 0};

class Sharp2Y0A21 {

  private:
    int IRpin;
    String variableName;
    float etalon;

  public:
    int react = 0;
    Sharp2Y0A21(int IRpin, String variableName): IRpin(IRpin), variableName(variableName) {}

    // Усреднение нескольких значений для сглаживания
    float irRead() {
      float averaging = 0;             //  переменная для суммирования данных
      // Получение iTimes значений
      float value1;
      for (int i = 0; i < iTimes; i++) {
        value1 = analogRead(this->IRpin);
        //Serial.println(value1);
        averaging = averaging + value1;
        delay(10);      // Ожидание 55 ms перед каждым чтением
      }
      averaging = averaging / iTimes;      // усреднить значения
      return (averaging);
    }

    float irRead(int iTimes) {
      float averaging = 0;             //  переменная для суммирования данных
      // Получение iTimes значений
      float value1;
      for (int i = 0; i < iTimes; i++) {
        value1 = analogRead(this->IRpin);
        //Serial.println(value1);
        averaging = averaging + value1;
        delay(10);      // Ожидание 55 ms перед каждым чтением
      }
      averaging = averaging / iTimes;      // усреднить значения
      return (averaging);
    }

    float getDistance() {
      Serial.print(this->variableName + " show irRead: ");
      Serial.println(irRead());
      float volts = this->irRead() * 0.0048828125;
      // и в расстояние в см
      float distance = 32 * pow(volts, -1.10);
      Serial.print(this->variableName + " show: ");
      Serial.println(distance);
      return distance;
    }

    void stabilaze() {
      this->etalon = 32 * pow(irRead(20)* 0.0048828125, -1.10);
      Serial.println("Etalon is:");
      Serial.println(this->etalon);
    }

    int checkPass() {
      float distance = this->getDistance();
      if (this->etalon - 20 < distance && distance < this->etalon + 20)
        return 0;
      return 1;
    }

    int checkPassForXsec(int interval) {
      unsigned long currentMillis = millis();
      unsigned long previousMillis = currentMillis;
      while (currentMillis - previousMillis < interval) {
        if (this->checkPass()) {
          return 1;
        }
        currentMillis = millis();
      }
      return 0;
    }
};

Sharp2Y0A21 sharpLeft(A0, "sharpLeft");
Sharp2Y0A21 sharpRight(A1, "sharpRight");



void setup() {
  Serial.begin(9600);
  Serial1.begin(115200);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  setSyncProvider(requestSync);  //set function to call when sync required
  Serial.println("Waiting for sync message");
  initializeSD();
  //SD.remove("test.txt");
  myFile = SD.open(fileName, FILE_WRITE);
  //writeFile("Everything work");
  //readFile();
  sharpLeft.stabilaze();
  sharpRight.stabilaze();
   ClockModuleCheck();
}


void  loop() {

  if (Serial.available()) {
    processSyncMessage();
  }
  if (timeStatus() != timeNotSet) {
    //digitalClockDisplay();
    Serial.println(digitalClockString());
  }

  checkInOut();
  Serial.println(sharpLeft.checkPass());
  if (RTC.read(tm)) {
      if(tm.Second<5){
      
      sendData();
    }
    }
}

void digitalClockDisplay() {
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
String digitalClockString() {
  // digital clock display of the time
  String str = "[";
  str += tm.Hour;
  int digits = tm.Minute;
  str += ":";
  if (digits < 10)
    str += '0';
  str += digits;
  digits = tm.Second;
  str += ":";
  if (digits < 10)
    str += '0';
  str += digits;
  str += " ";
  str += print2digitsV2(tm.Day);
  str += ".";
  str += print2digitsV2(tm.Month);
  str += ".";
  str += tmYearToCalendar(tm.Year);
  str += "]";
  return str;
}
String print2digitsV2(int number) {
  String str="";
  if (number >= 0 && number < 10) {
    str+="0";
  }
  str+=number;
  return str;
}

void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if (Serial.find(TIME_HEADER)) {
    pctime = Serial.parseInt();
    if ( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
      setTime(pctime); // Sync Arduino clock to the time received on the serial port
    }
  }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);
  return 0; // the time will be sent later in response to serial mesg
}


void printDigits(byte digits) {
  // utility function for digital clock display: prints colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits, DEC);
}

void initializeSD() {
  Serial.print("Initializing SD card...");

  if (!SD.begin(CS_PIN)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
}
void writeFile(String str) {

  myFile = SD.open(fileName, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to " + fileName + "...");
    myFile.println(str);
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening " + fileName);
  }
}

void readFile() {
  // re-open the file for reading:
  myFile = SD.open(fileName);
  if (myFile) {
    Serial.println(fileName + ":");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening " + fileName);
  }
}

void checkInOut() {
  unsigned long currentMillis = millis();
  unsigned long interval = 1000;
  if (sharpLeft.checkPass())
    if (sharpRight.checkPassForXsec(interval)) {
      people.in++;
      people.count++;
      Serial.println("Someone come in. People in room:");
      Serial.println(people.in);
      Serial.println(digitalClockString() + "Object detected. Counted: " + people.count);
      writeFile(digitalClockString() + "Object detected. Counted: " + people.count);
      delay(1500);
    }
  if (sharpRight.checkPass())
    if (sharpLeft.checkPassForXsec(interval)) {
      people.out++;
      people.count++;
      Serial.println("Someone out of. People out:");
      Serial.println(people.out);
      Serial.println(digitalClockString() + "Object detected. Counted: " + people.count);
      writeFile(digitalClockString() + "Object detected. Counted: " + people.count);
      delay(1500);
    }



}
void CheckDistance(float distance) {
  //Serial.println(distance);      // выдаем в порт значение
  if (distance < 40 || distance > 115) {
    people.count++;
    Serial.println(digitalClockString() + "Object detected. Counted: " + people.count);
    writeFile(digitalClockString() + "Object detected. Counted: " + people.count);
    delay(1500);
  }
}
void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
void ClockModuleCheck(){
 if (RTC.read(tm)) {
    Serial.print("Ok, Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
}
bool sendData()
{
  bool send=false;
  int reset=0;
  while(!send){

  if(reset>=3)
  {
  Serial.println("RESET");
  Serial1.println("AT+RST");
  
  delay(10000);
  reset=0;
  }

    
  connectWiFi();

Serial1.println("AT+CIPMUX=0\r\n");      // To Set MUX = 0
  delay(2000);                             // Wait for 2 sec

  // TCP connection 
  String cmd = "AT+CIPSTART=\"TCP\",\"";   // TCP connection with https://thingspeak.com server
  cmd += IP_SERVER;                // IP addr of api.thingspeak.com
  cmd += "\",80\r\n\r\n";                  // Port No. = 80

  Serial1.println(cmd);                    // Display above Command on PC
  Serial.println(cmd);                     // Send above command to Rx1, Tx1
                            // Wait for 20 Sec

  if(Serial1.find("ERROR"))                // If returns error in TCP connection
  { 
    Serial.println("AT+CIPSTART error");   // Display error msg to PC
    //return; 
  }

  // prepare GET string "GET /SensorCP/setdata.php?id=1&value=10&cdata=2019-01-06&userID=1\r\n\r\n"
  String getStr="GET /SensorCP/setdata.php?id=";
  getStr+=ID;
  getStr+="&value=";
  getStr+=people.in;
  getStr+="&cdata=";
  getStr+=tmYearToCalendar(tm.Year);
  getStr+="-";
  getStr+=print2digitsV2(tm.Month);
  getStr+="-";
  getStr+=print2digitsV2(tm.Day);
  getStr+="&userID=";
  getStr+=userID;
  getStr+="\r\nHHTP/1.1";
  Serial.println(getStr);                 // Display GET String on PC

  cmd = "AT+CIPSEND=";                    // send data length 
  cmd += String(getStr.length());
  cmd+="\r\n";


  delay(1000);
  Serial1.println(cmd);
  delay(2000);
  Serial.println("send"); 
  delay(2000);// Send Data length command to Tx1, Rx1
 
  if(Serial1.find(">"))// If prompt opens //verify connection with cloud
  {
    Serial.println("connected to Cloud");// Display confirmation msg to PC
    Serial1.println(getStr);
    send=true;
    people.in=0;
    // Send GET String to Rx1, Tx1
  }
  else
  { 
    Serial1.println("AT+CIPCLOSE\r\n");    // Send Close Connection command to Rx1, Tx1
    Serial.println("AT+CIPCLOSE");         // Display Connection closed command on PC
    reset++;
  } 

  // thingspeak free version needs 16 sec delay between updates 
  delay(2000);                            // wait for 16sec
  }
  
  
}

boolean connectWiFi() {               // Connect to Wifi Function
  Serial1.println("AT+CWMODE=1\r\n"); // Setting Mode = 1 
  delay(100);                         // wait for 100 mSec

  String cmd = "AT+CWJAP=\"";         // Connect to WiFi
  cmd += SISS;                   // ssid_name
  cmd += "\",\"";
  cmd += PASSWORD;                // password
  cmd += "\"\r\n";              
  
  Serial.println(cmd);                // Display Connect Wifi Command on PC
  Serial1.println(cmd);               // send Connect WiFi command to Rx1, Tx1 
  
  delay(10000);                       // wait for 10 sec

  Serial1.println("AT+CWJAP?");       // Verify Connected WiFi

  if(Serial1.find("+CWJAP"))        
  {
    Serial.println("OK, Connected to WiFi.");         // Display Confirmation msg on PC
    return true;
  }
  else
  {
    Serial.println("Can not connect to the WiFi.");   // Display Error msg on PC
    return false;
  }
 }
