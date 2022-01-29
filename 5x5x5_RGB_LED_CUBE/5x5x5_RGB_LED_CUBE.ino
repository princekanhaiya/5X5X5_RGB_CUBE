#include <TimerOne.h>
#include<SoftwareSerial.h>
#include "Arduino.h"
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
#define AXIS_X 1
#define AXIS_Y 2
#define AXIS_Z 3


uint8_t cube[5][5];
uint8_t current_rows = 0;
uint32_t temp=0;
uint8_t * digitalValues;

int patternNo = 0;
volatile int intFlag = 0;

void setup() {
  Serial.begin(9600); // Starts the serial communication

  // // Reset any PWM configuration that the arduino may have set up automagically!
  TIMSK2 = (TIMSK2 & B11111110) | 0x01;
  TCCR2B = (TCCR2B & B11111000) | 0x04;
  randomSeed(analogRead(10));  //seeding random for random pattern
  for (int i = 0; i < 5; i++) {
    pinMode(rows[i], OUTPUT);
  }
  pinMode(OE, OUTPUT);
  digitalWrite(OE,LOW);
  //analogWrite(OE, 250);

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
    
    temp=(cube[current_rows][0]&0x1f);  
    temp=(temp<<5)|(cube[current_rows][1]&0x1f);
    temp=(temp<<5)|(cube[current_rows][2]&0x1f);
    temp=(temp<<5)|(cube[current_rows][3]&0x1f);
    temp=(temp<<5)|(cube[current_rows][4]&0x1f);

    for (i=0; i<25; i++){
      byte data;
      if(intFlag==0)
        data=redLed[i];
      if(intFlag==1)
        data=greenLed[i];
      if(intFlag==2)
        data=blueLed[i];

      if (temp %2)
        digitalValues[data/ 8] |= 1 << (data % 8);
      else
        digitalValues[data/ 8] &= ~(1 << (data % 8));
      temp/=2;
    }

    digitalWrite(latchPin, LOW);
    for(int i=numberOfShiftRegisters*8;i>=0;i--){
			digitalWrite(serialDataPin, (digitalValues[i/8] & 0b10000000) != 0);
			digitalValues[i/8] <<= 1;
			digitalWrite(clockPin, HIGH);
		  digitalWrite(clockPin, LOW);		
    }
    digitalWrite(latchPin, HIGH);
    
    //all rows off(
    PORTD &=0b00011111;
    PORTB &=0b11111100;

    digitalWrite(rows[current_rows++], HIGH);
    current_rows%=TOTAL_ROW;

}

void loop() {
  // effect_intro();
  // if(intFlag==0)
  //   intFlag=1;
  // else if(intFlag==1)
  //   intFlag=2;
  // else
  //   intFlag=0;
  // off();

  //firework(-1,0,500);
  //randomRainUp();
  //randomRainDown();
  //randomRainMultiDrop(50);
  //spark();

  // effect_planboing(AXIS_Z, 400);
  // effect_planboing(AXIS_Y, 400);
  // effect_planboing(AXIS_X, 400);

  //effect_blinky();
    
  // effect_random_filler(500,1);
  // delay(1000);
  // effect_random_filler(500,0);
  // delay(1000);
    
  //effect_rain(100);
   
  // effect_boxside_randsend_parallel (AXIS_X, 0, 450, 1);delay(200);
  // effect_boxside_randsend_parallel (AXIS_X, 1, 450, 1);delay(200);
  // effect_boxside_randsend_parallel (AXIS_Y, 0, 450, 1);delay(200);
  // effect_boxside_randsend_parallel (AXIS_Y, 1, 450, 1);delay(200);
  // effect_boxside_randsend_parallel (AXIS_Z, 0, 450, 1);delay(200);
  // effect_boxside_randsend_parallel (AXIS_Z, 1, 450, 1);delay(200);



  //colorchange();
}






void effect_boxside_randsend_parallel (char axis, int origin, int delay, int mode){
	int i;
	int done;
	unsigned char cubepos[25];
	unsigned char pos[25];
	int notdone = 1;
	int notdone2 = 1;
	int sent = 0;
	
	for (i=0;i<25;i++)
	{
		pos[i] = 0;
	}
	
	while (notdone)
	{
		if (mode == 1)
		{
			notdone2 = 1;
			while (notdone2 && sent<25)
			{
				i = rand()%25;
				if (pos[i] == 0)
				{
					sent++;
					pos[i] += 1;
					notdone2 = 0;
				}
			}
		} else if (mode == 2)
		{
			if (sent<25)
			{
				pos[sent] += 1;
				sent++;
			}
		}
		
		done = 0;
		for (i=0;i<25;i++)
		{
			if (pos[i] > 0 && pos[i] <4)
			{
				pos[i] += 1;
			}
				
			if (pos[i] == 4)
				done++;
		}
		
		if (done == 25)
			notdone = 0;
		
		for (i=0;i<25;i++)
		{
			if (origin == 0)
			{
				cubepos[i] = pos[i];
			} else
			{
				cubepos[i] = (4-pos[i]);
			}
		}
		
		
		delay_ms(delay);
		draw_positions_axis(axis,cubepos,0);

	}
	
}

void effect_rain (int iterations){
	int i, ii;
	int rnd_x;
	int rnd_y;
	int rnd_num;
	
	for (ii=0;ii<iterations;ii++){
		rnd_num = rand()%4;
		for (i=0; i < rnd_num;i++){
			rnd_x = rand()%5;
			rnd_y = rand()%5;
			setpixel(rnd_x,rnd_y,4);
		}
		
		delay_ms(1000);
		shift(AXIS_Z,-1);
	}
}

// Set or clear exactly 512 voxels in a random order.
void effect_random_filler (int delay, int state){
	int x,y,z;
	int loop = 0;

	if (state)
		off();
	else
		on();
	
	while (loop<125){
		x = rand()%5;
		y = rand()%5;
		z = rand()%5;

		if ((state == 0 && getpixel(x,y,z) == 0x01) || (state == 1 && getpixel(x,y,z) == 0x00)){
			alterpixel(x,y,z,state);
			delay_ms(delay);
			loop++;
		}	
	}
}

void effect_blinky(){
	int i,r;
  off();
	for (r=0;r<2;r++){
		i = 1000;
		while (i>0){
			off();
			delay_ms(i);
			on();
			delay_ms(100);
			i = i - (15+(1000/(i/10)));
		}
		
		delay_ms(1000);
		i = 1000;
		while (i>0){
			off();
			delay_ms(751-i);
			on();
			delay_ms(100);
			i = i - (15+(1000/(i/10)));
		}
	}
}

void colorchange(){
  if(intFlag==0)
    intFlag=1;
  else if(intFlag==1)
    intFlag=2;
  else
    intFlag=0;
}

//random rain up
void randomRainUp(){
  fill(0x00);
  uint8_t time = 50;
  uint8_t X = random(0,5);
  uint8_t Y = random(0,5);
  uint8_t Z = random(0,5);
  for(int z=0;z<5;z++){
    setpixel(X,Y,z);
    z==2||z==3?delay(time):delay(time+100);
    clrpixel(X,Y,z);
  }
}

//random rain down
void randomRainDown(){
  fill(0x00);
  uint8_t time = 50;
  uint8_t X = random(0,5);
  uint8_t Y = random(0,5);
  uint8_t Z = random(0,5);
  for(int z=4;z>=0;z--){
    setpixel(X,Y,z);
    z==2||z==3?delay(time):delay(time+100);
    clrpixel(X,Y,z);
  }
}

void randomRainMultiDrop(uint8_t time){
  fill(0x00);
  uint8_t X1=random(0,5),X2= random(0,5),X3= random(0,5),X4= random(0,5);
  uint8_t Y1=random(0,5),Y2= random(0,5),Y3= random(0,5),Y4= random(0,5);
  for(int z=4;z>=0;z--){
    setpixel(X1,Y1,z);
    setpixel(X2,Y2,z);
    setpixel(X3,Y3,z);
    setpixel(X4,Y4,z);
    z==2||z==3?delay(time):delay(time+100);
    clrpixel(X1,Y1,z);
    clrpixel(X2,Y2,z);
    clrpixel(X3,Y3,z);
    clrpixel(X4,Y4,z);
  }
}

void spark() {
  fill(0x00);
  shift(AXIS_Z,-1);
  uint8_t numDrops = random(0, 5);
  for (uint8_t i = 0; i < numDrops; i++) {
      setpixel(random(0, 5), random(0, 5), random(0, 5));
  }
}

void effect_intro() {
  int cnt, cnt_2, time;
  //Bottom To Top
  for (cnt = 0; cnt <= 4; cnt++) {
    box_wireframe(0, 0, 0, 4, 4, cnt);
    delay_ms(1000);
  }
  for (cnt = 0; cnt <= 4; cnt++) {
    clrplane_z(cnt);
    delay_ms(1000);
  }

  for (cnt = 0; cnt <= 4; cnt++) {
    box_wireframe(0, 0, 0, 4, 4, cnt);
    delay_ms(1000);
  }
  for (cnt = 0; cnt < 4; cnt++) {
    clrplane_z(cnt);
    delay_ms(1000);
  }

  //   Shift Things Right
  // 1
  shift(AXIS_Y, -1);
  for (cnt = 0; cnt <TOTAL_ROW; cnt++) {
    setpixel(cnt, 0, 3);
  }
  delay_ms(1000);
  //2
  shift(AXIS_Y, -1);
  for (cnt = 0; cnt <TOTAL_ROW; cnt++) {
    setpixel(cnt, 0, 2);
  }
  setpixel(0, 0, 3);
  setpixel(4, 0, 3);
  delay_ms(1000);
  //3
  shift(AXIS_Y, -1);
  for (cnt = 0; cnt <TOTAL_ROW; cnt++) {
    setpixel(cnt, 0, 1);
  }
  setpixel(0, 0, 3);
  setpixel(4, 0, 3);
  setpixel(0, 0, 2);
  setpixel(4, 0, 2);
  delay_ms(1000);

  //4
  shift(AXIS_Y, -1);
  for (cnt = 0; cnt <TOTAL_ROW; cnt++) {
    setpixel(cnt, 0, 0);
  }
  setpixel(0, 0, 3);
  setpixel(4, 0, 3);
  setpixel(0, 0, 2);
  setpixel(4, 0, 2);
  setpixel(0, 0, 1);
  setpixel(4, 0, 1);
  delay_ms(1000);

  //Right To Left
  for (cnt = 0; cnt <TOTAL_ROW; cnt++) {
    box_wireframe(0, 0, 0, 4, cnt, 4);
    delay_ms(500);
  }
  for (cnt = 0; cnt < TOTAL_ROW-1; cnt++) {
    clrplane_y(cnt);
    delay_ms(500);
  }

  //Shift to the bottom
  for (cnt_2 = 3; cnt_2 >= 0; cnt_2--) {
    shift(AXIS_Z, -1);
    for (cnt = 0; cnt <TOTAL_ROW; cnt++) {
      setpixel(cnt, cnt_2, 0);
    }
    for (cnt = 3; cnt > cnt_2; cnt--) {
      setpixel(0, cnt, 0);
      setpixel(4, cnt, 0);
    }

    delay_ms(1000);
  }

  //Make All Wall Box
  for (cnt = 0; cnt <TOTAL_ROW-1; cnt++) {
    fill(0x00);
    box_walls(0, 0, 0, 4, 4, cnt);
    delay_ms(1500);
  }

  time = 3000;
  for (cnt_2 = 0; cnt_2 < 5; cnt_2++) {
    time = time - 500;
    //Make Box Smaller
    for (cnt = 1; cnt <= 1; cnt++) {
      fill(0x00);
      box_walls(cnt, cnt, cnt, 4 - cnt, 4 - cnt, 4 - cnt);
      delay_ms(time);
    }

    //Make Box Bigger
    for (cnt = 1; cnt >=0; cnt--) {
      fill(0x00);
      box_walls(cnt, cnt, cnt, 4 - cnt, 4 - cnt, 4 - cnt);
      delay_ms(time);
    }
  }

  for (cnt_2 = 0; cnt_2 < 5; cnt_2++) {
    time = time + 500;
    //Make Box Smaller
    for (cnt = 0; cnt <= 1; cnt++) {
      fill(0x00);
      box_walls(cnt, cnt, cnt, 4 - cnt, 4 - cnt, 4 - cnt);
      delay_ms(time);
    }

    //Make Box Bigger
    for (cnt = 1; cnt >=0; cnt--) {
      fill(0x00);
      box_walls(cnt, cnt, cnt, 4 - cnt, 4 - cnt, 4 - cnt);
      delay_ms(time);
    }
  }
}

void firework(int i, int j, int time) {
  fill(0x00);

  setpixel(1 - i, 2 - j, 0);
  delay_ms(900 - time);

  clrpixel(1 - i, 2 - j, 0);
  setpixel(2 - i, 2 - j, 1);
  delay_ms(1200 - time);

  clrpixel(2 - i, 2 - j, 1);
  setpixel(2 - i, 3 - j, 2);
  delay_ms(1400 - time);

  clrpixel(2 - i, 3 - j, 2);
  setpixel(1 - i, 3 - j, 3);
  delay_ms(1700 - time);   

  //Explode
  clrpixel(1 - i, 3 - j, 3);
  setpixel(2 - i, 1 - j, 4);
  setpixel(2 - i, 2 - j, 3);
  setpixel(2 - i, 1 - j, 3);
  setpixel(1 - i, 1 - j, 3);
  setpixel(3 - i, 1 - j, 3);
  delay_ms(2000 - time);

  shift(AXIS_Z, -1);
  setpixel(2 - i, 3 - j, 5);
  setpixel(2 - i, 1 - j, 5);
  setpixel(1 - i, 1 - j, 5);
  setpixel(3 - i, 1 - j, 5);
  delay_ms(900 - time);

  shift(AXIS_Z, -1);
  setpixel(2 - i, 3 - j, 3);
  setpixel(2 - i, 0 - j, 3);
  setpixel(1 - i, 2 - j, 3);
  setpixel(4 - i, 2 - j, 3);
  delay_ms(900 - time);

  shift(AXIS_Z, -1);
  setpixel(2 - i, 3 - j, 1);
  setpixel(1 - i, 0 - j, 1);
  setpixel(0 - i, 1 - j, 1);
  setpixel(3 - i, 1 - j, 1);
  delay_ms(1400 - time);

  shift(AXIS_Z, -1);
  delay_ms(1400 - time);

  shift(AXIS_Z, -1);
  delay_ms(1400 - time);

  shift(AXIS_Z, -1);
  delay_ms(1400 - time);

  shift(AXIS_Z, -1);
  delay_ms(700 - time);

  fill(0x00);

}

void draw_positions_axis (char axis, unsigned char positions[25], int invert){
  int x, y, p;
  fill(0x00);

  for (x = 0; x < TOTAL_ROW; x++){
    for (y = 0; y < TOTAL_ROW; y++){
      if (invert)
        p = (TOTAL_ROW-1 - positions[(x * TOTAL_ROW) + y]);
      else
        p = positions[(x * TOTAL_ROW) + y];

      if (axis == AXIS_Z)
        setpixel(x, y, p);

      if (axis == AXIS_Y)
        setpixel(x, p, y);

      if (axis == AXIS_X)
        setpixel(p, y, x);
    }
  }

}

// Draw a plane on one axis and send it back and forth once.
void effect_planboing (int plane, int speed){
  int i;
  for (i = 0; i < TOTAL_ROW; i++){
    fill(0x00);
    setplane(plane, i);
    delay_ms(speed);
  }

  for (i = TOTAL_ROW-1; i >= 0; i--){
    fill(0x00);
    setplane(plane, i);
    delay_ms(speed);
  }
}

void space(int iterations) {

  int i, ii;
  int rnd_y;
  int rnd_z;
  int rnd_num;
  int time;

  time = 700;

  for (ii = 0; ii < iterations; ii++)
  {
    time = time - (iterations / 15);
    rnd_num = rand() % 4;

    for (i = 0; i < rnd_num; i++)
    {
      rnd_y = rand() % 8;
      rnd_z = rand() % 8;
      setpixel(TOTAL_ROW-1, rnd_y, rnd_z);
    }

    delay_ms(time);
    shift(AXIS_X, -1);
  }

  for (ii = 0; ii < iterations; ii++)
  {
    time = time + (iterations / 15);
    rnd_num = rand() % 4;

    for (i = 0; i < rnd_num; i++)
    {
      rnd_y = rand() % 8;
      rnd_z = rand() % 8;
      setpixel(TOTAL_ROW-1, rnd_y, rnd_z);
    }

    delay_ms(time);
    shift(AXIS_X, -1);
  }

}

//This function turn an LED on
void setpixel(uint8_t x, uint8_t y, uint8_t z){
  if(inrange(x, y, z))
    cube[z][y] |= (1 << x);  
}

// Set a single pixel to OFF
void clrpixel(int x, int y, int z){
  if (inrange(x, y, z))
    cube[z][y] &= ~(1 << x);
}
// This function validates that we are drawing inside the cube.
unsigned char inrange(int x, int y, int z){
  if (x >= 0 && x < 5 && y >= 0 && y < 5 && z >= 0 && z < 5)
  {
    return 0x01;
  } else
  {
    // One of the coordinates was outside the cube.
    return 0x00;
  }
}

// Get the current status of a pixel
unsigned char getpixel(int x, int y, int z){
  if (inrange(x, y, z)){
    if (cube[z][y] & (1 << x))
      return 0x01;
    else
      return 0x00;
  } 
  else
    return 0x00;
}

// In some effect we want to just take bool and write it to a pixel
// this function calls the apropriate pixel manipulation function.
void alterpixel(int x, int y, int z, int state){
  if (state == 1)
    setpixel(x, y, z);
  else
    clrpixel(x, y, z);
}

// Flip the state of a pixel.
// If the pixel is 1, its turned into a 0, and vice versa.
void flppixel(int x, int y, int z)
{
  if (inrange(x, y, z))
    cube[z][y] ^= (1 << x);
}

// Makes sure x1 is alwas smaller than x2
// This is usefull for functions that uses for loops,
// to avoid infinite loops
void argorder(int ix1, int ix2, int *ox1, int *ox2){
  if (ix1 > ix2)
  {
    int tmp;
    tmp = ix1;
    ix1 = ix2;
    ix2 = tmp;
  }
  *ox1 = ix1;
  *ox2 = ix2;
}

// Sets all pixels along a X/Y plane at a given point
// on axis Z
void setplane_z (int z){
  int i;
  if (z >= 0 && z < TOTAL_ROW)
  {
    for (i = 0; i < TOTAL_ROW; i++)
      cube[z][i] = 0xff;
  }
}

// Clears pixels in the same manner as above
void clrplane_z (int z){
  int i;
  if (z >= 0 && z < TOTAL_ROW)
  {
    for (i = 0; i < TOTAL_ROW; i++)
      cube[z][i] = 0x00;
  }
}

void setplane_x (int x){
  int z;
  int y;
  if (x >= 0 && x < TOTAL_ROW)
  {
    for (z = 0; z < TOTAL_ROW; z++)
    {
      for (y = 0; y < TOTAL_ROW; y++)
      {
        cube[z][y] |= (1 << x);
      }
    }
  }
}

void clrplane_x (int x){
  int z;
  int y;
  if (x >= 0 && x < TOTAL_ROW)
  {
    for (z = 0; z < TOTAL_ROW; z++)
    {
      for (y = 0; y < TOTAL_ROW; y++)
      {
        cube[z][y] &= ~(1 << x);
      }
    }
  }
}

void setplane_y (int y){
  int z;
  if (y >= 0 && y < TOTAL_ROW)
  {
    for (z = 0; z < TOTAL_ROW; z++)
      cube[z][y] = 0xff;
  }
}

void clrplane_y (int y){
  int z;
  if (y >= 0 && y < TOTAL_ROW)
  {
    for (z = 0; z < TOTAL_ROW; z++)
      cube[z][y] = 0x00;
  }
}

void setplane (char axis, unsigned char i){
  switch (axis)
  {
    case AXIS_X:
      setplane_x(i);
      break;

    case AXIS_Y:
      setplane_y(i);
      break;

    case AXIS_Z:
      setplane_z(i);
      break;
  }
}

void clrplane (char axis, unsigned char i){
  switch (axis)
  {
    case AXIS_X:
      clrplane_x(i);
      break;

    case AXIS_Y:
      clrplane_y(i);
      break;

    case AXIS_Z:
      clrplane_z(i);
      break;
  }
}

// Fill a value into all 64 byts of the cube buffer
// Mostly used for clearing. fill(0x00)
// or setting all on. fill(0xff)
void fill (unsigned char pattern)
{
  int z;
  int y;
  for (z = 0; z < 8; z++)
  {
    for (y = 0; y < 8; y++)
    {
      cube[z][y] = pattern;
    }
  }
}

// Draw a box with all walls drawn and all pixels inside set
void box_filled(int x1, int y1, int z1, int x2, int y2, int z2)
{
  int iy;
  int iz;

  argorder(x1, x2, &x1, &x2);
  argorder(y1, y2, &y1, &y2);
  argorder(z1, z2, &z1, &z2);

  for (iz = z1; iz <= z2; iz++)
  {
    for (iy = y1; iy <= y2; iy++)
    {
      cube[iz][iy] |= byteline(x1, x2);
    }
  }

}

// Darw a hollow box with side walls.
void box_walls(int x1, int y1, int z1, int x2, int y2, int z2)
{
  int iy;
  int iz;

  argorder(x1, x2, &x1, &x2);
  argorder(y1, y2, &y1, &y2);
  argorder(z1, z2, &z1, &z2);

  for (iz = z1; iz <= z2; iz++)
  {
    for (iy = y1; iy <= y2; iy++)
    {
      if (iy == y1 || iy == y2 || iz == z1 || iz == z2)
      {
        cube[iz][iy] = byteline(x1, x2);
      } else
      {
        cube[iz][iy] |= ((0x01 << x1) | (0x01 << x2));
      }
    }
  }

}

// Draw a wireframe box. This only draws the corners and edges,
// no walls.
void box_wireframe(int x1, int y1, int z1, int x2, int y2, int z2)
{
  int iy;
  int iz;

  argorder(x1, x2, &x1, &x2);
  argorder(y1, y2, &y1, &y2);
  argorder(z1, z2, &z1, &z2);

  // Lines along X axis
  cube[z1][y1] = byteline(x1, x2);
  cube[z1][y2] = byteline(x1, x2);
  cube[z2][y1] = byteline(x1, x2);
  cube[z2][y2] = byteline(x1, x2);

  // Lines along Y axis
  for (iy = y1; iy <= y2; iy++)
  {
    setpixel(x1, iy, z1);
    setpixel(x1, iy, z2);
    setpixel(x2, iy, z1);
    setpixel(x2, iy, z2);
  }

  // Lines along Z axis
  for (iz = z1; iz <= z2; iz++)
  {
    setpixel(x1, y1, iz);
    setpixel(x1, y2, iz);
    setpixel(x2, y1, iz);
    setpixel(x2, y2, iz);
  }

}

// Returns a byte with a row of 1's drawn in it.
// byteline(2,5) gives 0b00111100
char byteline (int start, int end)
{
  return ((0xff << start) & ~(0xff << (end + 1)));
}

// Flips a byte 180 degrees.
// MSB becomes LSB, LSB becomes MSB.
char flipbyte (char byte)
{
  char flop = 0x00;
  flop = (flop & 0b11111110) | (0b00000001 & (byte >> 7));
  flop = (flop & 0b11111101) | (0b00000010 & (byte >> 5));
  flop = (flop & 0b11111011) | (0b00000100 & (byte >> 3));
  flop = (flop & 0b11110111) | (0b00001000 & (byte >> 1));
  flop = (flop & 0b11101111) | (0b00010000 & (byte << 1));
  flop = (flop & 0b11011111) | (0b00100000 & (byte << 3));
  flop = (flop & 0b10111111) | (0b01000000 & (byte << 5));
  flop = (flop & 0b01111111) | (0b10000000 & (byte << 7));
  return flop;
}

// Draw a line between any coordinates in 3d space.
// Uses integer values for input, so dont expect smooth animations.
void line(int x1, int y1, int z1, int x2, int y2, int z2)
{
  float xy; // how many pixels do we move on the y axis for each step on the x axis
  float xz; // how many pixels do we move on the y axis for each step on the x axis
  unsigned char x, y, z;
  unsigned char lasty, lastz;

  // We always want to draw the line from x=0 to x=7.
  // If x1 is bigget than x2, we need to flip all the values.
  if (x1 > x2)
  {
    int tmp;
    tmp = x2; x2 = x1; x1 = tmp;
    tmp = y2; y2 = y1; y1 = tmp;
    tmp = z2; z2 = z1; z1 = tmp;
  }


  if (y1 > y2)
  {
    xy = (float)(y1 - y2) / (float)(x2 - x1);
    lasty = y2;
  } else
  {
    xy = (float)(y2 - y1) / (float)(x2 - x1);
    lasty = y1;
  }

  if (z1 > z2)
  {
    xz = (float)(z1 - z2) / (float)(x2 - x1);
    lastz = z2;
  } else
  {
    xz = (float)(z2 - z1) / (float)(x2 - x1);
    lastz = z1;
  }



  // For each step of x, y increments by:
  for (x = x1; x <= x2; x++)
  {
    y = (xy * (x - x1)) + y1;
    z = (xz * (x - x1)) + z1;
    setpixel(x, y, z);
  }

}

// Delay loop.
// This is not calibrated to milliseconds,
// but we had allready made to many effects using this
// calibration when we figured it might be a good idea
// to calibrate it.
void delay_ms(uint16_t x)
{
  uint8_t y, z;
  for ( ; x > 0 ; x--) {
    for ( y = 0 ; y < 90 ; y++) {
      for ( z = 0 ; z < 6 ; z++) {
        asm volatile ("nop");
      }
    }
  }
}

// Shift the entire contents of the cube along an axis
// This is great for effects where you want to draw something
// on one side of the cube and have it flow towards the other
// side. Like rain flowing down the Z axiz.
void shift (char axis, int direction)
{
  int i, x , y;
  int ii, iii;
  int state;

  for (i = 0; i < TOTAL_ROW; i++)
  {
    if (direction == -1)
      ii = i;
    else
      ii = (TOTAL_ROW-1 - i);
  
    for (x = 0; x < TOTAL_ROW; x++)
    {
      for (y = 0; y < TOTAL_ROW; y++)
      {
        if (direction == -1)
        {
          iii = ii + 1;
        } else
        {
          iii = ii - 1;
        }

        if (axis == AXIS_Z)
        {
          state = getpixel(x, y, iii);
          alterpixel(x, y, ii, state);
        }

        if (axis == AXIS_Y)
        {
          state = getpixel(x, iii, y);
          alterpixel(x, ii, y, state);
        }

        if (axis == AXIS_X)
        {
          state = getpixel(iii, y, x);
          alterpixel(ii, y, x, state);
        }
      }
    }
  }

  if (direction == -1)
  {
    i = TOTAL_ROW-1;
  } else
  {
    i = 0;
  }

  for (x = 0; x < TOTAL_ROW; x++)
  {
    for (y = 0; y < TOTAL_ROW; y++)
    {
      if (axis == AXIS_Z)
        clrpixel(x, y, i);

      if (axis == AXIS_Y)
        clrpixel(x, i, y);

      if (axis == AXIS_X)
        clrpixel(i, y, x);
    }
  }
}

void test1(){
  for(int p=0;p<5;p++){
  for(uint8_t i=0;i<5;i++){
    for (uint8_t j= 0; j < 5; j++)
      cube[i][j]=0b00001<<p;
  }
  delay(50);
}
if(intFlag==0)
    intFlag=1;
  else if(intFlag==1)
    intFlag=2;
  else
    intFlag=0;
  off();
  for(int p=4;p>=0;p--){
  for(uint8_t i=0;i<5;i++){
    for (uint8_t j= 0; j < 5; j++)
      cube[i][j]=0b00001<<p;
  }
  delay(60);
}
}

void justPlay(){
    for(int i=0;i<5;i++){
      cube[i][0]=0b11111;
      cube[i][1]=0b10001;
      cube[i][2]=0b10001;
      cube[i][3]=0b10001;
      cube[i][4]=0b11111;
  } 
  delay(500);
  off();
  for(int i=1;i<4;i++){
      cube[i][0]=0b00000;
      cube[i][1]=0b01110;
      cube[i][2]=0b01010;
      cube[i][3]=0b01110;
      cube[i][4]=0b00000;
  } 
  delay(200);
  off();
  // if(intFlag==0)
  //   intFlag=1;
  // else if(intFlag==1)
  //   intFlag=2;
  // else
    intFlag=0;
}

void on(){
  for(uint8_t i=0;i<5;i++){
    for (uint8_t j= 0; j < 5; j++)
      cube[i][j]=0b11111;
  }
}

void off(){
  for(uint8_t i=0;i<5;i++){
    for (uint8_t j= 0; j < 5; j++)
      cube[i][j]=0b00000;
  }
}
