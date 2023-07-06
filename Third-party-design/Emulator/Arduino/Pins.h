#define INPUT 0
#define INPUT_PULLUP 0
#define CHANGE 0


void pinMode(int pin,int mode);
int digitalPinToInterrupt(int pin);
void attachInterrupt(int ii, void (*param)(void), int i);
int digitalRead(int pin);