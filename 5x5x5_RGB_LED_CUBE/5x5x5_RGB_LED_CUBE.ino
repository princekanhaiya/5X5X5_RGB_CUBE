#include <TimerOne.h>
#include<SoftwareSerial.h>
#include "Arduino.h"
#define TIMER_US 50                              // 50 uS timer duration 
#define UBLB(a,b,c,d,e) (((a&0x1f)<<20)|((b&0x1f)<<15)|((c&0x1f)<<10)|((d&0x1f)<<5)|((e&0x1f)<<0));
byte numberOfShiftRegisters = 10, serialDataPin = 11, clockPin = 13, latchPin = 12, OE = 10;
byte blueLed[25] = { 37, 31, 19, 15, 25,
                     36, 33, 24, 16, 01,
                     66, 56, 54, 22, 10, 
                     78, 71, 60, 50, 44,  
                     76, 69, 58, 48, 41}; 
             
byte redLed[25] =  { 39, 34, 27, 18, 04,
                     38, 29, 26, 17, 03,
                     68, 64, 55, 45, 11,
                     79, 72, 61, 51, 42,
                     74, 73, 62, 52, 46 };
              
byte greenLed[25]= { 35, 28, 23, 13, 00,
                     32, 30, 21, 14,  9,
                     65, 63, 53, 20, 12,  
                     77, 70, 59, 49, 43,
                     75, 67, 57, 47, 40 };
                     
byte rows[5] = {8, 7, 6, 5, 9};

#define buzzer 4
#define buzzerFreq 1000 //1kHz freq because crystal used in h/w is of 16MHz
#define pcbTestLed A0
#define TOTAL_COL 25
#define TOTAL_ROW 5
#define COLOR 6


uint8_t cube[5][5];
uint8_t current_rows = 0;
uint32_t temp=0;
uint8_t * digitalValues;

volatile int patternNo = 0;
volatile int intFlag = 0;

void setup() {
  Serial.begin(9600); // Starts the serial communication

  // // Reset any PWM configuration that the arduino may have set up automagically!
  TIMSK2 = (TIMSK2 & B11111110) | 0x01;
  TCCR2B = (TCCR2B & B11111000) | 0x04;
  randomSeed(analogRead(10));  //seeding random for random pattern
  Timer1.initialize(TIMER_US); // Initialise timer 1
  for (int i = 0; i < 5; i++) {
    pinMode(rows[i], OUTPUT);
  }
  pinMode(OE, OUTPUT);
  analogWrite(OE, 100);

  // define pins as outputs
  pinMode(clockPin, OUTPUT);
  pinMode(serialDataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  // set pins low
  digitalWrite(clockPin, LOW);
  digitalWrite(serialDataPin, LOW);
  digitalWrite(latchPin, LOW);

  digitalValues = (uint8_t *)malloc(numberOfShiftRegisters * sizeof(uint8_t));
  memset(digitalValues, 0, numberOfShiftRegisters * sizeof(uint8_t));
  for(int i=numberOfShiftRegisters;i>=0;i--)
      shiftOut(serialDataPin, clockPin, MSBFIRST, digitalValues[i]);
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
}


ISR (TIMER2_OVF_vect){       
    uint8_t i;  
    //temp=UBLB(cube[current_rows][4],cube[current_rows][3],cube[current_rows][2],cube[current_rows][1],cube[current_rows][0]);
    temp=(cube[current_rows][0]&0x1f);  
    temp=(temp<<5)|(cube[current_rows][1]&0x1f);
    temp=(temp<<5)|(cube[current_rows][2]&0x1f);
    temp=(temp<<5)|(cube[current_rows][3]&0x1f);
    temp=(temp<<5)|(cube[current_rows][4]&0x1f);

    for (i=0; i<25; i++){
      if (temp %2)
        digitalValues[redLed[i] / 8] |= 1 << (redLed[i] % 8);
      else
        digitalValues[redLed[i]/ 8] &= ~(1 << (redLed[i] % 8));
      temp/=2;
    }
    for(int i=numberOfShiftRegisters;i>=0;i--)
      shiftOut(serialDataPin, clockPin, MSBFIRST, digitalValues[i]);
    digitalWrite(latchPin, HIGH);
    digitalWrite(latchPin, LOW);

    for (i = 0; i < TOTAL_ROW; i++)
        digitalWrite(rows[i], LOW);
        
    // digitalWrite(rows[current_rows++], HIGH);
    // if (current_rows == TOTAL_ROW)
    //     current_rows = 0;

if(current_rows <TOTAL_ROW)
    digitalWrite(rows[current_rows], HIGH);
  else
    digitalWrite(rows[current_rows+1], LOW);
  current_rows++;
  
  if (current_rows == 5)
    current_rows = 0;

}

void loop() {
  for(int i=0;i<5;i++){
      cube[i][0]=0b11111;
      cube[i][1]=0b10001;
      cube[i][2]=0b10001;
      cube[i][3]=0b10001;
      cube[i][4]=0b11111;
  } 
  delay(200);
  off();
  for(int i=0;i<5;i++){
      cube[i][0]=0b00000;
      cube[i][1]=0b01110;
      cube[i][2]=0b01010;
      cube[i][3]=0b01110;
      cube[i][4]=0b00000;
  } 
  delay(200);
  off();

  for(int i=0;i<5;i++){
      cube[i][0]=0b00000;
      cube[i][1]=0b00000;
      cube[i][2]=0b00100;
      cube[i][3]=0b00000;
      cube[i][4]=0b00000;
  } 
  delay(200);
  off();
}

void off(){
  for(int i=0;i<5;i++){
    for (int j= 0; j < 5; j++)
      cube[i][j]=0b00000;
  }
}
