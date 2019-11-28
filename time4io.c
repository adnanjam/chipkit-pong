#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw(){
    return (*(&PORTD) & 0x0F00) >> 8;
}

int getbtns(){
    return (PORTD & 0x00E0) >> 5;
}