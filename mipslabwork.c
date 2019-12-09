/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h> /* Declarations of uint_32 and the like */
#include <string.h>
#include <stdio.h>
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declatations for these labs */

int mytime = 0x5957;
volatile char *portE = (volatile char *)0xbf886110;

int count = 0;
int timeoutcunt = 0;
int trigger = 10;

int ballx = 64;
int bally = 16;
int right = 1;
int down = 1;

int padAOffset = 13;
int padBOffset = 13;
int height = 8;

int pointsA = 0;
int pointsB = 0;
const MAX_SCORE = 3;

int bot = 0;

char highscores[8];

/* Interrupt Service Routine */
void user_isr(void)
{
  return;
}

/* Lab-specific initialization goes here */
void labinit(void)
{
  *portE = 0;
  PORTD = 0x0fe0;
  // initialize timer 2:
  T2CON = T2CON | 0b01110000; // Stop Timer and clear control register,// set prescaler at 1:1, internal clock source
  TMR2 = 0x0;                 // Clear timer register
  PR2 = 8e6 / (4096 * 4);     // Load period register
  IFSCLR(0) = 0x00000100;     // Clear Timer interrupt status flag
  T2CONSET = 0x8000;          // Start Timer

  return;
}

void move_pad(int direction, int *offset, int colOffset1, int colOffset2)
{
  int heightOffset = direction == 1 ? 0 : height - 1;
  screen[((*offset) + heightOffset) * 128 + colOffset1] = 0;
  screen[((*offset) + heightOffset) * 128 + colOffset2] = 0;

  *offset += direction;

  if (*offset < 1)
    *offset = 1;
  if (*offset > 31 - height)
    *offset = 31 - height;
}

void move_players(void)
{

  if (getbtns() & 0b0001)
    {
      move_pad(-1, &padAOffset, 0, 1);
    }
  if (getbtns() & 0b0010){
    move_pad(1, &padAOffset, 0, 1);
  }

  if(bot == 0){
    if (getbtns() & 0b0100)
      move_pad(-1, &padBOffset, -1, -2);
    if (getbtns() & 0b1000)
      move_pad(1, &padBOffset, -1, -2);
  }else{
    if(bally > padBOffset)
      move_pad(1, &padBOffset, -1, -2);
    if(bally < padBOffset)
      move_pad(-1, &padBOffset, -1, -2);
  }
  
}

void show_players(void)
{
  int i;
  for (i = 0; i < height; i++)
  {
    screen[padAOffset * 128 + 128 * i] = 1;
    screen[padAOffset * 128 + 128 * i + 1] = 1;
    screen[padBOffset * 128 + 128 * i - 1] = 1;
    screen[padBOffset * 128 + 128 * i - 2] = 1;
  }
}

void reset_ball(void)
{
  ballx = 64;
  bally = 16;
  trigger = 10;
}

void reset_points(){
  int i;
  for (i = 3; i > 0; i--){
    highscores[i*2] = highscores[(i-1)*2];
    highscores[i*2+1] = highscores[(i-1)*2+1]; 
  }
  highscores[0] = pointsA;
  highscores[1] = pointsB;
  pointsA = 0;
  pointsB = 0;
}

void update_score(int* points){
  (*points)++;
  if(*points == MAX_SCORE) {
    reset_points(points);
  }
}

void move_ballX(void)
{
  if (right == 1)
  {
    ballx++;
    if (ballx >= 127){
      update_score(&pointsA);
      reset_ball();
    }
  }

  if (right == 0)
  {
    ballx--;
    if (ballx <= 1){
      update_score(&pointsB);      
      reset_ball();
    }
  }
}

void move_ballY(void)
{
  if (down == 1)
  {
    bally++;
    if (bally > 26)
      down = 0;
  }

  if (down == 0)
  {
    bally--;
    if (bally < 3)
      down = 1;
  }
}

void show_ball(int value)
{
  screen[bally * 128 + (ballx)] = value;
  screen[bally * 128 + (ballx) + 1] = value;
  screen[(bally + 1) * 128 + (ballx)] = value;
  screen[(bally + 1) * 128 + (ballx) + 1] = value;
}

void check_collision()
{

  int y = bally + 1;
  if (right == 1)
  {
    // Pad B collision
    if (ballx == 125)
    {
      if (y >= padBOffset && y < padBOffset + 3)
      {
        down = 0;
        right = 0;
        trigger--;
      }
      if (y >= padBOffset + 3 && y < padBOffset + 5)
      {
        right = 0;
        down = -1;
        trigger--;
      }
      if (y >= padBOffset + 5 && y < padBOffset + 8)
      {
        down = 1;
        right = 0;
        trigger--;
      }
    }
  }

  if (right == 0)
  {
    // Pad A collision
    if (ballx == 2)
    {
      if (y >= padAOffset && y < padAOffset + 3)
      {
        down = 0;
        right = 1;
        trigger--;
      }
      if (y >= padAOffset + 3 && y < padAOffset + 5)
      {
        right = 1;
        down = -1;
        trigger--;
      }
      if (y >= padAOffset + 5 && y < padAOffset + 8)
      {
        down = 1;
        right = 1;
        trigger--;
      }
    }
  }
}

void show_score(int points, int offset){
  int i; 
  for(i = 0; i < MAX_SCORE*2; i++){
    // every second one is blank space
    int c = i % 2 == 0 ? 0 : 1;
    
    if (c == 1 && i > points*2){
      c = 0;
    }

    int r;
    for(r = 3; r < 9; r++)
      screen[r*128+offset+i] = c;
  }
}

void game_screen(void){
  
  if (IFS(0) & 0x100)
  {
    IFSCLR(0) = 0x100;
    count++;
    timeoutcunt++;
  }

  if (timeoutcunt == 6)
  {
    move_players();
    show_players();
    timeoutcunt = 0;
  }

  if (count == trigger)
  {
    show_score(pointsA, 32);
    show_score(pointsB, 96);

    show_ball(0);
    move_ballX();
    check_collision();
    move_ballY();
    show_ball(1);
      
    count = 0;
  }

  if (trigger < 1)
    trigger = 1;

  render();
}

void highscores_screen(void){
  int i;
  for(i = 0; i < 4; i++){
    char str[13];
    str[0] = 'G';
    str[1] = 'a';
    str[2] = 'm';
    str[3] = 'e';
    str[4] = ' ';
    str[5] = (i+1)+'0';
    str[6] = ':';
    str[7] = ' ';
    str[8] = highscores[2*i]+'0';
    str[9] = ' ';
    str[10] = '-';
    str[11] = ' ';
    str[12] = highscores[2*i+1]+'0';
    display_string(i, str);
  }
  display_update();
}


/* This function is called repetitively from the main program */
void labwork(void)
{

  if(getsw() == 1){
    highscores_screen();
  }
  else if(getsw() == 2){
    bot = 1;
    game_screen();
  }
  else{
    bot = 0;
    game_screen();
  }

}
