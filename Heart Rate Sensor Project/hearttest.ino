#include <LiquidCrystal.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
MAX30105 particleSensor;
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
Serial.println("Initializing...");
if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

particleSensor.setup();
particleSensor.setPulseAmplitudeRed(0x0A);
particleSensor.setPulseAmplitudeGreen(0);
lcd.begin(16, 2);
lcd.setCursor(3,0);
lcd.print("Heart Rate");
lcd.setCursor(5,1);
lcd.print("Monitor");
pinMode(13,OUTPUT);
delay(2000);
lcd.clear();
}
void loop() {
  // put your main code here, to run repeatedly:
long irValue = particleSensor.getIR();
if (irValue < 50000){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Please put your");
  lcd.setCursor(0,1);
  lcd.print("finger");
  }
if (irValue > 50000){
  //lcd.clear();
  if (checkForBeat(irValue) == true)
  {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  lcd.setCursor(0,0);
  lcd.print("Heart Rate :    ");
  lcd.setCursor(0,1);
  lcd.print(beatAvg);
  lcd.print(" ");
  lcd.print("BPM  ");
  digitalWrite(13,HIGH);
  delay(10);
  digitalWrite(13,LOW);
  }
  }
}
