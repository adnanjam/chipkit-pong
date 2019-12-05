/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h> /* Declarations of uint_32 and the like */
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

int healthA = 3;
int healthB = 3;

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

  if (getbtns() == 1)
    move_pad(-1, &padAOffset, 0, 1);
  if (getbtns() == 2)
    move_pad(1, &padAOffset, 0, 1);

  if (getbtns() == 4)
    move_pad(-1, &padBOffset, -1, -2);
  if (getbtns() == 8)
    move_pad(1, &padBOffset, -1, -2);
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

void move_ballX(void)
{
  if (right == 1)
  {
    ballx++;
    if (ballx >= 127)
      reset_ball();
  }

  if (right == 0)
  {
    ballx--;
    if (ballx <= 1)
      reset_ball();
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

/* This function is called repetitively from the main program */
void labwork(void)
{

  if (IFS(0) & 0x100)
  {
    IFSCLR(0) = 0x100;
    count++;
    timeoutcunt++;
  }

  if (timeoutcunt == 3)
  {
    move_players();
    show_players();
    timeoutcunt = 0;
  }

  if (count == trigger)
  {
    show_ball(0);
    move_ballX();
    check_collision();
    move_ballY();
    show_ball(1);
    count = 0;
  }

  if (trigger < 1)
    trigger = 1;

  // display_ball(ballx, bally);
  render();
  // display_debug(&bally);

  // display_clear();
}
