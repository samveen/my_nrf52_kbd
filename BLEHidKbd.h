/**************************************************************************/
/*!
    @file     BLEHidKbd.h
    @author   samveen (samveen.in)

    @section LICENSE

    See License file
*/
/**************************************************************************/
#ifndef BLEHIDKBD_H_
#define BLEHIDKBD_H_

#include <bluefruit.h>

class BLEHidKbd : public BLEHidGeneric
{
  public:
    // Callback Signatures
    typedef void (*kbd_led_cb_t)  (uint16_t conn_hdl, uint8_t leds_bitmap);

    BLEHidKbd(void);

    virtual err_t begin(void);

    //------------- Keyboard -------------//
    // Single connection
    void setKeyboardLedCallback(kbd_led_cb_t fp);

    bool keyboardReport(hid_keyboard_report_t* report);
    bool keyboardReport(uint8_t modifier, uint8_t keycode[6]);

    bool keyPress(char ch);
    bool keyRelease(void);
    bool keySequence(const char* str, int interval=5);

    // Multiple connections
    bool keyboardReport(uint16_t conn_hdl, hid_keyboard_report_t* report);
    bool keyboardReport(uint16_t conn_hdl, uint8_t modifier, uint8_t keycode[6]);

    bool keyPress(uint16_t conn_hdl, char ch);
    bool keyRelease(uint16_t conn_hdl);
    bool keySequence(uint16_t conn_hdl, const char* str, int interval=5);

    //------------- Consumer Media Keys -------------//
    // Single connection
    bool consumerReport(uint16_t usage_code);
    bool consumerKeyPress(uint16_t usage_code);
    bool consumerKeyRelease(void);

    // Multiple connections
    bool consumerReport(uint16_t conn_hdl, uint16_t usage_code);
    bool consumerKeyPress(uint16_t conn_hdl, uint16_t usage_code);
    bool consumerKeyRelease(uint16_t conn_hdl);

    //------------- Mouse -------------//
    // Single connection
    bool mouseReport(hid_mouse_report_t* report);
    bool mouseReport(uint8_t buttons, int8_t x, int8_t y, int8_t wheel=0, int8_t pan=0);

    bool mouseButtonPress(uint8_t buttons);
    bool mouseButtonRelease(void);

    bool mouseMove(int8_t x, int8_t y);
    bool mouseScroll(int8_t scroll);
    bool mousePan(int8_t pan);

    // Multiple connections
    bool mouseReport(uint16_t conn_hdl, hid_mouse_report_t* report);
    bool mouseReport(uint16_t conn_hdl, uint8_t buttons, int8_t x, int8_t y, int8_t wheel=0, int8_t pan=0);

    bool mouseButtonPress(uint16_t conn_hdl, uint8_t buttons);
    bool mouseButtonRelease(uint16_t conn_hdl);

    bool mouseMove(uint16_t conn_hdl, int8_t x, int8_t y);
    bool mouseScroll(uint16_t conn_hdl, int8_t scroll);
    bool mousePan(uint16_t conn_hdl, int8_t pan);

  protected:
    uint8_t _mse_buttons;
    kbd_led_cb_t _kbd_led_cb;

    static void blehid_kbd_keyboard_output_cb(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len);
};

#endif /* BLEHIDKBD_H_ */
