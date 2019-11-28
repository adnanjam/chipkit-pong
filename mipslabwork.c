/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <stdio.h>
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

int mytime = 0x5957;
volatile char* portE =  (volatile char *)0xbf886110;

int count = 0;
int timeoutcount = 0;
int ballx = 0;
int bally = 0x00000001;
int right = 1;
int down = 1;

int padAOffset = 13;
int padBOffset = 13;
int height = 8;

/* Interrupt Service Routine */
void user_isr( void ){
  return;
}

/* Lab-specific initialization goes here */
void labinit( void ){ 
  *portE = 0; 
  PORTD = 0x0fe0;
  // initialize timer 2:
  T2CON = T2CON | 0b01110000;              // Stop Timer and clear control register,// set prescaler at 1:1, internal clock source
  TMR2 = 0x0;               // Clear timer register 
  PR2 = 8e6 / 4096;             // Load period register 
  IFSCLR(0) = 0x00000100;  // Clear Timer interrupt status flag 
  T2CONSET = 0x8000;        // Start Timer

  return;
}
void move_pad(void){
  int i;
  for(i = 0; i < height; i++){
    if(getbtns() == 1){
      
      screen[(padAOffset+height)*128 + 128*i] = 0;
      screen[(padAOffset+height)*128 + 128*i + 1] = 0;
      padAOffset++;
      screen[(padAOffset)*128 + 128*i] = 1;
      screen[(padAOffset)*128 + 128*i + 1] = 1;


    }
    else if(getbtns() == 2){
      screen[(padAOffset)*128 + 128*i] = 0;
      screen[(padAOffset)*128 + 128*i + 1] = 0;
      padAOffset++;
      screen[(padAOffset+height)*128 + 128*i] = 1;
      screen[(padAOffset+height)*128 + 128*i + 1] = 1;
    }
    else{ 
      screen[padAOffset*128 + 128*i] = 1;
      screen[padAOffset*128 + 128*i + 1] = 1;
    
      screen[padBOffset*128 + 128*i - 1] = 1;
      screen[padBOffset*128 + 128*i - 2] = 1;}
    }
}


void moveX(void){
  if(right == 1){
    ballx++;
    if(ballx == 128) right = 0;
  }else{
    ballx--;
    if(ballx == 0){ right = 1;}
  }
}

void moveY(void){
  if(down == 1){
    bally = bally << 1;
    if(bally == 0x40000000) down = 0;
  }else{
    bally =  bally >> 1;
    if(bally == 0x00000001){ down = 1;}
  }
}


/* This function is called repetitively from the main program */
void labwork( void ){
  
  if(IFS(0) & 0x100){
    IFSCLR(0) = 0x100;
    count++; 
  }

  if(count == 5){   
    move_pad();
    count = 0;
  }

  // display_ball(ballx, bally);
  render();
  // display_debug(&bally);

  // display_clear();
    
} 

