#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw()
{
    return (*(&PORTD) & 0x0F00) >> 8;
}

int getbtns()
{
    // Take bits 5 - 7
    int bytes = (PORTD & 0xE0) >> 5;
    // Take the eleventh bit
    bytes = bytes | (PORTD & 0x800) >> 8;

    return bytes;
}