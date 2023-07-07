#include <cstdlib>
#include <unordered_map>
#include <SDL_keycode.h>
#include <SDL_events.h>
#include "ArduinoCompatible.h"

class Serial Serial;

long map(long value, long fromLow, long fromHigh, long toLow, long toHigh) {
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}


static float a = 0;
int analogRead(int pin) {
    a = a + 1;

    return sin(0.1f * a)*1023;
}
