#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <NewPing.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int BUZZER_PIN = 12;
const int SONAR_PIN = 13;
const int RX_PIN = 6;
const int TX_PIN = 5;

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
NewPing sonar(SONAR_PIN, SONAR_PIN, 200);
SoftwareSerial mySoftwareSerial(RX_PIN, TX_PIN); 
DFRobotDFPlayerMini myDFPlayer;

const long TIME = 60000;
const int WIN_SCORE = 10, MAX_DIST = 16;
int dist, sound, score = 0;
boolean isStart = true, isFinish = false;
long startGameTime, timeLeft;

void setup() 
{
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  if(!myDFPlayer.begin(mySoftwareSerial)) 
  { 
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(30); 
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  lcd.begin(16, 2);
  lcd.backlight();
  pinMode(BUZZER_PIN, OUTPUT);
  randomSeed(analogRead(0));
}

void loop() 
{ 
  // start the game
  if(isStart)
  {
    timeLeft = TIME;
    isStart = false;
    isFinish = false;
    score = 0;
    lcd.clear();
    lcd.print(0);
    myDFPlayer.play(13); 
    startGameTime = millis();
  }

  // check if times up
  if(millis() - startGameTime > TIME)
  {
    beep(2093, 3000);
    isFinish = true;
    isStart = true;
    sound = random(9, 13);
    myDFPlayer.play(sound);
    delay(5000);
  }

  if(!isFinish)
  {
    // if one second passed - update the time on lcd
    if(timeLeft - TIME + millis() - startGameTime > 1000)
    {
      timeLeft = TIME - millis() + startGameTime;
      lcd.clear();
      lcd.print(score);
      lcd.setCursor(12, 0);
      lcd.print(long(timeLeft / 1000));
    }

    // check if score a basket 
    delay(30);                     
    dist = sonar.ping_cm(); 
    if(dist < MAX_DIST)
    {
      score++;
      lcd.clear();
      lcd.print(score);
      lcd.setCursor(12, 0);
      lcd.print(long(timeLeft / 1000));
      sound = random(1, 9);
      myDFPlayer.play(sound);
      delay(1000);
    }
  }

  // check if won
  if(score >= WIN_SCORE)
  {
    isFinish = true;
    isStart = true;
    myDFPlayer.play(14);
    delay(150000);
  }
}

void beep(int frequencyInHertz, long timeInMilliseconds)
{ 
  int x;
  long delayAmount = (long)(1000000 / frequencyInHertz);
  long loopTime = (long)((timeInMilliseconds * 1000) / (delayAmount * 2));
  for (x = 0; x < loopTime; x++)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(delayAmount);
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(delayAmount);
  }
}

