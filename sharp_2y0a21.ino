//10-80 cm work range
const int IRpin = A0;          // аналоговый пин для подключения выхода Vo сенсора
const int iTimes=5; //змінна для сер.знач.
//int value1;                    // для хранения аналогового значения
unsigned long long ullPeopleCou=0; 
void setup() {
  Serial.begin(9600);            // Запуск последовательного порта
  }
 
void loop() {
 // irRead();
  //Serial.println(irRead(), DEC);
 // получаем сглаженное значение  и переводим в напряжение
 float volts = irRead()*0.0048828125;
 // и в расстояние в см 
 float distance=32*pow(volts,-1.10);
 CheckDistance(distance);
 delay(0);                    
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
    Serial.println("Object detected");
    ullPeopleCou++;
    delay(1500);
  }
}
