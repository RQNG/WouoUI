#pragma once

#include "U8g2lib.h"
#include <cstdint>
#include <SDL_timer.h>
#include "iostream"
#include <cstdlib>
#include "Pins.h"
#include "WouoFix.h"
#include "Print.h"

#define PROGMEM

// 使用SDL模拟 12864 oled


class U8G2_SDL_128X64 : public U8G2, public Print {
public:
    U8G2_SDL_128X64(const u8g2_cb_t *pStruct, int i, int i1, int i2) : U8G2() {
        u8g2_SetupBuffer_SDL_128x64(&u8g2, &u8g2_cb_r0);
    }

    size_t write(const uint8_t *buffer, size_t size) override {
        return 1;
    }

    size_t write(uint8_t) override {
        return 1;
    }
};


// 兼容串口输出,直接输出到stdout
class Serial {
public:

    void println(const char *message) {
        std::cout << message << std::endl;
    }

    void println(int value) {
        std::cout << value << std::endl;
    }

    void println(float value) {
        std::cout << value << std::endl;
    }


    void begin(int rate) {
        std::cout << "set fake Serial baud rate  " << rate << std::endl;

    }
};

extern Serial Serial;


#define delay(ms) SDL_Delay(ms)

long map(long value, long fromLow, long fromHigh, long toLow, long toHigh);


int analogRead(int pin);

#define LOW 0
#define HIGH 1
