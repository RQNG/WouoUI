#include <unordered_map>
#include <SDL_events.h>
#include "WouoFix.h"
#include "Arduino.h"

//每次循环加上10毫秒延迟
extern "C" void postLoop(){
    delay(10);
}


bool takeOver = false;
long longPress = -1;


std::unordered_map<int32_t, bool> keyStatus;


void knob_inter();
void btn_scan();



int digitalRead(int pin) {
    if (takeOver) {
        if (longPress == 0) {
            keyStatus[pin] = !keyStatus[pin];
            longPress--;
        }
        if (longPress > 0) {
            longPress--;
        }
        return keyStatus[pin];
    }


    SDL_Event event;
    /* https://wiki.libsdl.org/SDL_PollEvent */
    if (SDL_PollEvent(&event) != 0) {
        switch (event.type) {
            case SDL_QUIT:
                exit(0);
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case 'q': {
                        // 模拟编码器
                        takeOver = true;
                        keyStatus[PB12] = 1;
                        keyStatus[PB13] = 0;
                        knob_inter();
                        keyStatus[PB12] = 0;
                        keyStatus[PB13] = 1;
                        knob_inter();
                        keyStatus[PB12] = 1;
                        keyStatus[PB13] = 0;
                        knob_inter();
                        keyStatus[PB12] = 0;
                        keyStatus[PB13] = 1;
                        knob_inter();
                        takeOver = false;

                    }
                        break;
                    case 'w': {
                        // 模拟编码器
                        takeOver = true;
                        keyStatus[PB12] = 1;
                        keyStatus[PB13] = 1;
                        knob_inter();
                        keyStatus[PB12] = 0;
                        keyStatus[PB13] = 0;
                        knob_inter();
                        keyStatus[PB12] = 1;
                        keyStatus[PB13] = 1;
                        knob_inter();
                        keyStatus[PB12] = 0;
                        keyStatus[PB13] = 0;
                        knob_inter();
                        takeOver = false;

                    }
                        break;
                    case 'e': {
                        // 模拟短按
                        takeOver = true;
                        keyStatus[PB14] = 1;
                        btn_scan();
                        keyStatus[PB14] = 1;
                        btn_scan();
                        keyStatus[PB14] = 0;
                        longPress = 3;
                        btn_scan();
                        takeOver = false;
                    }
                        break;
                    case 'r': {
                        // 模拟长按
                        takeOver = true;
                        keyStatus[PB14] = 1;
                        btn_scan();
                        keyStatus[PB14] = 1;
                        btn_scan();
                        keyStatus[PB14] = 0;
                        longPress = 400;
                        btn_scan();
                        takeOver = false;
                    }
                        break;

                    default: {
                    }
                }
                break;
            case SDLK_UP:
                switch (event.key.keysym.sym) {
                    case 'e':
//                        keyStatus[PB14] = 0;
                        break;
                    case 'r':
                        break;
                }
        }
    }


    return keyStatus[pin];
}

