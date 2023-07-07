class USBHID {
public:
    void begin(const uint8_t bar[], int foo) {
    }
};

class HIDConsumer {
public:
    HIDConsumer(USBHID usbhid) {}
    void press(int foo) {
    }
    void release() {
    }

    const static int BRIGHTNESS_DOWN=0;
    const static int VOLUME_DOWN=0;
    const static int VOLUME_UP=0;
    const static int BRIGHTNESS_UP=0;
};



class HIDKeyboard {
public:
    HIDKeyboard(USBHID usbhid) {

    }
    void press(int foo) {
    }
    void release(int foo) {
    }
};

const bool USBComposite = true;

#define HID_KEYBOARD_REPORT_DESCRIPTOR(...) 1
#define HID_CONSUMER_REPORT_DESCRIPTOR(...) 1
