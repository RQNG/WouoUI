
#include "u8g2.h"
#include <stdio.h>
#include "SDL.h"
#include "Arduino.h"


void postLoop();

_Noreturn int main(int argc, char *argv[])
{
    setup();
    while (1){
        loop();
        postLoop();
    }
}

