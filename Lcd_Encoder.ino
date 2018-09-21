#include <OneButton.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define PIN_A 2
#define PIN_B 4
#define SW 5
#define PULSE_PIN_LEN 5 //8 минимальна€ длинна импульса в миллесекундах на которую мы обращаем внимание


volatile unsigned long failingTime = 0;
volatile bool fl = false; // флаг что нужно вывести
volatile bool value_b = 0;
volatile byte prevA = 0;
volatile int Value0 = 0;
volatile int Value1 = 1;
volatile int Value2 = 2;
volatile int Value3 = 3;
volatile unsigned long pulseLen = 0;
boolean blinkEn = false;
boolean editEn = false;
uint8_t cursor_pos = 0;

//Setup LCD
LiquidCrystal_I2C lcd(0x3F, 16, 2);

//Setup a new OneButton
OneButton oneButton(SW, true);


void click() {
	if (editEn) {
		if (cursor_pos < 3) {
			cursor_pos++;
		}
		else {
			cursor_pos = 0;
		}
		if (cursor_pos == 0) lcd.setCursor(0, 0);
		if (cursor_pos == 1) lcd.setCursor(8, 0);
		if (cursor_pos == 2) lcd.setCursor(0, 1);
		if (cursor_pos == 3) lcd.setCursor(8, 1);
	}
}

void doubleClick() {
	lcd.clear();
	lcd.print("Double Click");
}

void duringLongPress() {
	lcd.clear();
	lcd.print("Multiple Long Press");
}

void longPressStart() {
	if (editEn) {
		editEn = false;
		blinkEn = false;
		lcd.noBlink();
	}
	else {
		editEn = true;
		blinkEn = true;
		lcd.home();
		lcd.blink();
	}
}

void longPressStop() {
	lcd.clear();
	lcd.print("Long Press Stop");
}

void setup() {
	Serial.begin(9600);
	pinMode(SW, INPUT_PULLUP);

	//OneButton	
	oneButton.attachClick(click);
	oneButton.attachDoubleClick(doubleClick);
	//oneButton.setDebounceTicks(100000);
	//oneButton.attachDuringLongPress(duringLongPress); // ћногократное нажатие при удержании	
	oneButton.attachLongPressStart(longPressStart); // Ќачало длительного нажати€
	//oneButton.attachLongPressStop(longPressStop); //  онец длительного нажати€

	//Encoder
	digitalWrite(PIN_A, HIGH);
	digitalWrite(PIN_B, HIGH);
	attachInterrupt(0, handler_a, CHANGE);

	//LCD
	lcd.init();
	lcd.backlight();
	lcd.noAutoscroll();

	lcd.print(Value0);
	lcd.setCursor(8, 0);
	lcd.print(Value1);
	lcd.setCursor(0, 1);
	lcd.print(Value2);
	lcd.setCursor(8, 1);
	lcd.print(Value3);
}

void handler_a() {
	byte A = digitalRead(PIN_A);
	if (!fl) { // пока не отчитались ничего больше не делаем
		if (prevA && !A) { // фронт упал
			value_b = digitalRead(PIN_B); // определили направление, но пока только "запомнили его"
			failingTime = millis(); // и запомнили когда мы "упали в ноль", начали отсчет длины импульса
		}


		if (!prevA && A && failingTime) { // восход€щий фронт и мы в режиме "отсчет времени импульса
			pulseLen = millis() - failingTime;
			if (pulseLen > PULSE_PIN_LEN) { // импульс бы достаточно длинный что-бы поверить тому что мы прочитали в его начале
				if (value_b) {
					if (cursor_pos == 0) Value0++;
					if (cursor_pos == 1) Value1++;
					if (cursor_pos == 2) Value2++;
					if (cursor_pos == 3) Value3++;
				}
				else {
					if (cursor_pos == 0) Value0--;
					if (cursor_pos == 1) Value1--;
					if (cursor_pos == 2) Value2--;
					if (cursor_pos == 3) Value3--;
				}
				fl = true; // включаем пометку что нужно отчитатс€ в Serial
			}
			failingTime = 0; // больше не ведем осчет времени импульса
		}
	}

	prevA = A;
}

void loop() {
	//OneButton
	oneButton.tick();

	if (fl && editEn) {
		lcd.clear();
		lcd.print(Value0);
		lcd.setCursor(8, 0);
		lcd.print(Value1);
		lcd.setCursor(0, 1);
		lcd.print(Value2);
		lcd.setCursor(8, 1);
		lcd.print(Value3);

		if (cursor_pos == 0) lcd.setCursor(0, 0);
		if (cursor_pos == 1) lcd.setCursor(8, 0);
		if (cursor_pos == 2) lcd.setCursor(0, 1);
		if (cursor_pos == 3) lcd.setCursor(8, 1);

		fl = false;
	}
}


