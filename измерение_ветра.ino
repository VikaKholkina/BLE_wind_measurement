#include "TimerOne.h" // используем библиотеку Timer1
#include <math.h>

#define WindSensorPin (2) // ввод анемометра
#define WindVanePin (A4) // ввод флюгера
#define VaneOffset 0; // смещение флюгера от севера

int VanePos; // положение флюгера 0-1023
int WindDirection; // положение флюгера в градусах 0-359 
int RelDirection; // значение положения флюгера с учетом смещения оси относительно севера



volatile bool MainTimer; // возвращает true при проходе таймера n раз
volatile unsigned int TimerSup; // счетчик наполнений таймера
volatile unsigned long Rotations; // счетчик оборотов анемометра
volatile unsigned long ContactBounceTime; // таймер для обработки дребезга контактов

float WindSpeed; // скорость ветра


void setup() {

	LastValue = 0;

	MainTimer = false;

	TimerSup = 0;
	Rotations = 0; 

	Serial.begin(9600);

	pinMode(WindSensorPin, INPUT);
	// настройка аппаратного прерывания для отслеживания ипмпульсов на вводе от анемометра
	attachInterrupt(digitalPinToInterrupt(WindSensorPin), isr_rotation, FALLING);// вызов функции при смене значения на вводе анемометра c High на LOW, 
																				

	
	Timer1.initialize();// Устанавливаем значение срабатвания таймера в 1 сек (по умолчанию)
	Timer1.attachInterrupt(isr_timer);// Настратваем прерывание таймера
}

void loop() {
	
	getWindDirection();// определяем направление ветра
	
		if(MainTimer) {
			// Согласно документации сила ветра в миль/час V=P(2.25/T)
			// Переводим в метры в сек V = P(2.25/3)*(1609/3600) = P * 0.3352
			WindSpeed = Rotations * 0.3352;
			Rotations = 0; // Сброс счетчика импульсов от анемометра

			MainTimer = false;// Cброс таймера
			
		}
	while (Serial.available() > 0) {// прослушиваем порт на наличие запроса
	
	
	
		char c = Serial.read();
		if (c == '\n') { // отслеживаем конец строки 
			Serial.print("Скорость ветра: ");Serial.print(WindSpeed); Serial.print("м/с\n");// пересылаем данные по скорости ветра
			Serial.print("Направление ветра: ");defDirection(RelDirection);Serial.print("\n");// пересылаем данные по направлению ветра
			Serial.print("Сила ветра: "); BoufortStrength(WindSpeed); // оценка силы ветра
			input_string = "";// сбрасываем строку ввода
		}
		else
		{
			input_string += c; // собираем посимвольно сообщение с последовательного порта
		}
	}		
}





// функция прерывания по таймеру
void isr_timer() {

	TimerSup++;

	if(TimerSup == 3)
		{
		MainTimer = true;
		TimerSup = 0;
	}
}

// обработчик аппаратного прерывания
void isr_rotation() {

	if((millis() - ContactBounceTime) > 15 ) { // отстройка от дребезга контактов
		Rotations++; // счетчик импульсов от анемометра
		ContactBounceTime = millis();
	}
}



// определение направления ветра
void getWindDirection() {

	VanePos = analogRead(WindVanePin);
	WindDirection = map(VanePos, 0, 1023, 0, 359);
	RelDirection = WindDirection + VaneOffset;

	if(RelDirection > 360)
	RelDirection = RelDirection - 360;

	if(RelDirection < 0)
	RelDirection = RelDirection + 360;

}

// перевод градусов в румбы
void defDirection(int direction) {

	if(direction < 11)
		Serial.print("север");
	else if (direction < 34)
		Serial.print("северо-северо-восток");
	else if (direction < 56)
		Serial.print("северо-восток");
	else if (direction < 79)
		Serial.print("восток-северо-восток");
	else if (direction < 101)
		Serial.print("восток");
	else if (direction < 124)
		Serial.print("восток-юго-восток");
	else if (direction < 146)
		Serial.print("юго-восток");
	else if (direction < 169)
		Serial.print("юго-юго-восток");
	else if (direction < 191)
		Serial.print("юг");
	else if (direction < 214)
		Serial.print("юго-юго-запад");
	else if (direction < 236)
		Serial.print("юго-запад");
	else if (direction < 259)
		Serial.print("запад-юго-запад");
	else if (direction < 281)
		Serial.print("запад");
	else if (direction < 304)
		Serial.print("запад-северо-запад");
	else if (direction < 326)
		Serial.print("северо-запад");
	else if (direction < 349)
		Serial.print("северо-северо-запад");
	else
		Serial.print("север");
}

// определение силы ветра
void BoufortStrength(float speed) {

	if(speed < 0.3)
		Serial.println("Штиль");
	else if(speed >= 0.3 && speed < 1.6)
		Serial.println("Тихий ветер");
	else if(speed >= 1.6 && speed < 3.3)
		Serial.println("Лёгкий ветер");
	else if(speed >= 3.3 && speed < 5.4)
		Serial.println("Слабый ветер");
	else if(speed >= 5.4 && speed < 7.9)
		Serial.println("Умеренный ветер");
	else if(speed >= 7.9 && speed < 10.7)
		Serial.println("Свежий ветер");
	else if(speed >= 10.7 && speed < 13.8)
		Serial.println("Сильный ветер");
	else if(speed >= 13.8 && speed < 17.1)
		Serial.println("Крепкий ветер");
	else if(speed >= 17.1 && speed < 20.7)
		Serial.println("Очень крепкий ветер");
	else if(speed >= 20.7 && speed < 24.4)
		Serial.println("Шторм");
	else if(speed >= 24.4 && speed < 28.4)
		Serial.println("Сильный шторм");
	else if(speed >= 24.4 && speed < 32,6)
		Serial.println("Жестокий шторм");
	else
		Serial.println("!!!Ураган!!!");
}