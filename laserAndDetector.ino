int laser1=22;
int detector1=A0;
int sensorValue1=0;

void setup() {
  Serial.begin(9600);
  pinMode(laser1,OUTPUT);
}

void loop() {
  digitalWrite(laser1,HIGH);
  sensorValue1=analogRead(detector1);//зчитуємо дані з фоторезистора
  Serial.println(sensorValue1,DEC);
  sensorValue1=0;
  //delay(500);
}
