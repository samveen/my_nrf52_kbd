/*********************************************************************
 This code targets the Bluefruit nRF52 Feather and started life as the
 Adafruit Blinky example. Much mutated since then.
 See NOTICE file for details.
*********************************************************************/
#include <bluefruit.h>
#include "BLEHidKbd.h"

// Macros
#define GPIO(x) (x)
#define k(x) HID_KEY_##x

// Constants
#define KEY_PRESSED_STATE (LOW)

// Battery status
#define VBAT_MV_PER_LSB   (0.73242188F)   // 3.0V ADC range and 12-bit ADC resolution = 3000mV/4096
#define VBAT_DIVIDER      (0.71275837F)   // 2M + 0.806M voltage divider on VBAT = (2M / (0.806M + 2M))
#define VBAT_DIVIDER_COMP (1.403F)        // Compensation factor for the VBAT divider
#define REAL_VBAT_MV_PER_LSB (VBAT_DIVIDER_COMP * VBAT_MV_PER_LSB)

/* This is used internally, not sent to the HID stack.
 * Maybe we can set this equal to HID_KEY_NONE, as the key matrix cannot
 * generate HID_KEY_NONE, so any conditions on it will work fine
 */
#define HID_KEY_FN    0xEA

/************* INFO SECTION BEGIN ******************/
/* Using an IDE connector set as a mount and bus allows me to seperate the pins
   of the AdaFruit nRF52 into the read and write set of pins, with the Battery
   level analog pin as the separator between the read and write sets. The set
   with lesser pins is designated pins_write as they are shared between multiple
   matrix segments: the keyboards has atleast 2 segments.
*/
uint32_t pins_read_on_board [] = {
                            GPIO(16),GPIO(15),GPIO(07),GPIO(11),/*GPIO(31)*/
 GPIO(02),GPIO(03),GPIO(04),GPIO(05),GPIO(28),GPIO(29),GPIO(12),/*GPIO(13) is next segment*/
};

uint32_t pins_write_on_board [] = { /* P31(VBAT ADC) is separator */
  /*GPIO(31)*/GPIO(30), GPIO(27), GPIO(26),GPIO(25),
    GPIO(13), GPIO(14),/*GPIO(8),GPIO(6),*/GPIO(20),
                  /* 06 and 08 seem to be serial-only, not GPIO */
};

uint32_t vbat_pin = PIN_VBAT;
/************* INFO SECTION END   ******************/

BLEDis bledis;
BLEBas blebas;
BLEHidKbd blehid;

char kbdName[]="3245WW+nRF52832";
char kbdManufacturer[]="Samveen via Adafruit";

uint_fast8_t keycount=0;

bool hasKeyPressed=false;

/****** Key Matrix Section START ****/
uint8_t state_l;
uint8_t state_r;

/* Function key pins */
uint32_t fn_pin_read = GPIO(15);
uint32_t fn_write_pin= GPIO(26);

/* Left READ pins */
uint32_t pins_read_left[] = {GPIO(28),GPIO(05),GPIO(04),GPIO(03),GPIO(02)};
uint32_t pincount_read_left = uint32_t(sizeof(pins_read_left)/sizeof(pins_read_left[0]));

/* Right READ pins */
uint32_t pins_read_right[] = {GPIO(16),GPIO(15),GPIO(07),GPIO(11),GPIO(12)};
uint32_t pincount_read_right = uint32_t(sizeof(pins_read_right)/sizeof(pins_read_right[0]));

/* Common WRITE pins */
uint32_t pins_write[] = {GPIO(13),GPIO(14),GPIO(30),GPIO(27),GPIO(26),GPIO(25),GPIO(20)};
uint32_t pincount_write = uint32_t(sizeof(pins_write)/sizeof(pins_write[0]));

/* KeyCode Mapping to keymatrix formed by pins_read X pins_write */
/* Left half */
uint8_t keys_left[2][5][7] = { {
{ k(GRAVE),       k(1),       k(2),       k(3),   k(4),    k(5),   k(6)},
{ k(TAB),         k(Q),       k(W),       k(E),   k(R),    k(T),   k(Y)},
{ k(CAPS_LOCK),   k(A),       k(S),       k(D),   k(F),    k(G),   k(H)},
{ k(SHIFT_LEFT),  k(Z),       k(X),       k(C),   k(V),    k(B),   k(NONE)},
{ k(CONTROL_LEFT),k(ALT_LEFT),k(GUI_LEFT),k(NONE),k(SPACE),k(NONE),k(FN)},
    }, {
{ k(ESCAPE), k(NONE), k(NONE), k(NONE), k(NONE), k(NONE), k(NONE)},
{ k(NONE),   k(NONE), k(NONE), k(NONE), k(NONE), k(NONE), k(NONE)},
{ k(NONE),   k(NONE), k(NONE), k(NONE), k(NONE), k(NONE), k(NONE)},
{ k(NONE),   k(NONE), k(NONE), k(NONE), k(NONE), k(NONE), k(NONE)},
{ k(NONE),   k(NONE), k(NONE), k(NONE), k(NONE), k(NONE), k(FN)},
    }
};

/* Right half */
uint8_t keys_right[2][5][7] = { {
{ k(7),    k(8),        k(9),     k(0),         k(MINUS),       k(EQUAL),        k(BACKSPACE)},
{ k(U),    k(I),        k(O),     k(P),         k(BRACKET_LEFT),k(BRACKET_RIGHT),k(BACKSLASH)},
{ k(J),    k(K),        k(L),     k(SEMICOLON), k(APOSTROPHE),  k(RETURN),       k(NONE)},
{ k(N),    k(M),        k(COMMA), k(PERIOD),    k(SLASH),       k(DELETE),       k(SHIFT_RIGHT)},
{ k(SPACE),k(ALT_RIGHT),k(ESCAPE),k(ARROW_LEFT),k(ARROW_DOWN),  k(ARROW_UP),     k(ARROW_RIGHT)}
    }, {
{ k(NONE), k(NONE), k(NONE), k(NONE),   k(NONE), k(NONE),     k(NONE)},
{ k(NONE), k(NONE), k(NONE), k(NONE),   k(NONE), k(NONE),     k(NONE)},
{ k(NONE), k(NONE), k(NONE), k(NONE),   k(NONE), k(NONE),     k(NONE)},
{ k(NONE), k(NONE), k(NONE), k(NONE),   k(NONE), k(NONE),     k(NONE)},
{ k(NONE), k(NONE), k(NONE), k(PAGE_UP),k(HOME), k(PAGE_DOWN),k(END)}
    }
};

/* Cant do more than 10 fingers in a go, now, can you? */
/* +2 for that lucky guy with 6 perfect digits */
uint8_t keys[12];

/* modifier flag mask for keyboard report */
uint8_t modifiers;

/* Function key flag */
bool function_state=false;

/*TODO: Function key action implementation */

/****** Key Matrix Section END   ****/

/* Battery: BEGIN */
uint8_t battery_check=100;

uint8_t get_battery_voltage_percent(void) {
    float raw;

    // Set the analog reference to 3.0V (default = 3.6V)
    analogReference(AR_INTERNAL_3_0);

    // Set the resolution to 12-bit (0..4095)
    analogReadResolution(12); // Can be 8, 10, 12 or 14

    // Let the ADC settle
    delay(1);

    // Get the raw 12-bit, 0..3000mV ADC value
    raw = analogRead(vbat_pin);

    // Set the ADC back to the default settings
    analogReference(AR_DEFAULT);
    analogReadResolution(10);

    // Convert the raw value to compensated mv, taking the resistor-
    // divider into account (providing the actual LIPO voltage)
    // ADC range is 0..3000mV and resolution is 12-bit (0..4095)
    float vbat_mv = raw * REAL_VBAT_MV_PER_LSB;

    // Convert from raw mv to percentage (based on LIPO chemistry)
    if(vbat_mv<3300)
        return 0;
    else if(vbat_mv <3600) {
        vbat_mv -= 3300;
        return (uint8_t)(vbat_mv/30);
    } else {
        vbat_mv -= 3600;
        return (uint8_t)(10 + (vbat_mv * 0.15F));  // thats vbat_mv /6.66666666
    }
}

/* Battery: END */

void setup() {
    Serial.begin(115200);
    while ( !Serial ) delay(10);   // for nrf52840 with native usb

    Serial.println("My3245WW rev $Id$");
    Serial.println("------------");
    Serial.println("main::setup()::start");

    pinMode(LED_RED, OUTPUT); // POWER indicator

    Serial.println("main::setup()::BT");

    // Setup BT
    Bluefruit.begin();
    Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
    Bluefruit.setName(kbdName);

    Serial.println("main::setup()::BT::DIS");
    // Configure and Start Device Information Service
    bledis.setManufacturer(kbdManufacturer);
    bledis.setModel(kbdName);
    bledis.begin();

    Serial.println("main::setup()::BT::BAS");
    // Start BLE Battery Service
    blebas.begin();

    // publish battery status
    uint8_t bat_percentage = get_battery_voltage_percent();
    blebas.write(bat_percentage);

    // Start BLE HID
    blehid.enableMouse(false);
    blehid.begin();

    // Set callback for set LED from central
    blehid.setKeyboardLedCallback(set_keyboard_led);

    // Set up and start advertising
    startAdv();

    /* Key matrix initialization */
    Serial.println("main::setup()::keymatrix");

    /* Writing pins in output mode, default is HIGH */
    uint_fast32_t i = pincount_write;
    do {
      pinMode(pins_write[--i], OUTPUT);
    }while(i);

    /* Reading pins in INPUT mode with PULLUP */
    i=pincount_read_left;
    do {
      pinMode(pins_read_left[--i], INPUT_PULLUP);
      pinMode(pins_read_right[i], INPUT_PULLUP);
    } while(i);

    Serial.println("main:setup()::end");
}

void loop() {

    if (hasKeyPressed) {
        hasKeyPressed = false;
        blehid.keyRelease();

        delay(5);
    }

    /* Battery check */
    if(battery_check) {
        --battery_check;
    } else {
        battery_check=100;

        uint8_t bat_percentage = get_battery_voltage_percent();

        if(bat_percentage<=100) { // Only on valid percentage
            // Display the results
            Serial.print("LIPO at "); Serial.print(bat_percentage); Serial.println("%");
            // BT broadcast of battery status
            blebas.notify(bat_percentage);
        }
    }

    keycount=0;
    modifiers=0;

    /* Function key press check: Separate the check to make key checks easier */
    digitalToggle(fn_write_pin); // Toggle to LOW
    state_l=digitalRead(fn_pin_read);
    digitalToggle(fn_write_pin); // Toggle back to HIGH
    if (state_l == KEY_PRESSED_STATE) {
        Serial.print("FN,");
        Serial.print("(");Serial.print(fn_write_pin);Serial.print(",");Serial.print(fn_pin_read);Serial.print(")-> ");
                Serial.println(k(FN),HEX);
        function_state=1;
    } else
        function_state=0;

    /* Loop through write pins, setting them low, and scan read pins for LOW */
    uint_fast32_t i = pincount_write;
    do {
        digitalToggle(pins_write[--i]); // Toggle to LOW
        uint_fast32_t j = pincount_read_left;
        do {
            state_l=digitalRead(pins_read_left[--j]);
            state_r=digitalRead(pins_read_right[j]);
            if (state_l == KEY_PRESSED_STATE) {
                if (keys_left[function_state][j][i])
                    keys[keycount++]=keys_left[function_state][j][i];
                Serial.print("(");Serial.print(j);Serial.print(",");Serial.print(i);Serial.print(")-> ");
                Serial.print("(");Serial.print(pins_write[j]);Serial.print(",");Serial.print(pins_read_left[i]);Serial.print(")-> ");
                Serial.println(keys_left[function_state][j][i],HEX);
                /* modifier flags check */
                switch(keys_left[function_state][j][i]) {
                    case k(SHIFT_LEFT):
                        modifiers|=KEYBOARD_MODIFIER_LEFTSHIFT;
                        Serial.print("LShift,");
                        break;
                    case k(CONTROL_LEFT):
                        modifiers|=KEYBOARD_MODIFIER_LEFTCTRL;
                        Serial.print("LCtrl,");
                        break;
                    case k(ALT_LEFT):
                        modifiers|=KEYBOARD_MODIFIER_LEFTALT;
                        Serial.print("LAlt,");
                        break;
                    case k(GUI_LEFT):
                        modifiers|=KEYBOARD_MODIFIER_LEFTGUI;
                        Serial.print("LGUI,");
                        break;
                }
            }
            if (state_r == KEY_PRESSED_STATE) {
                keys[keycount++]=keys_right[function_state][j][i];
                Serial.print("(");Serial.print(j);Serial.print(",");Serial.print(i);Serial.print(")-> ");
                Serial.print("(");Serial.print(pins_write[j]);Serial.print(",");Serial.print(pins_read_right[i]);Serial.print(")-> ");
                Serial.println(keys_right[function_state][j][i],HEX);
                switch(keys_right[function_state][j][i]) {
                    case k(CONTROL_RIGHT):
                        modifiers|=KEYBOARD_MODIFIER_RIGHTCTRL;
                        Serial.print("LCtrl,");
                        break;
                    case k(SHIFT_RIGHT):
                        modifiers|=KEYBOARD_MODIFIER_RIGHTSHIFT;
                        Serial.print("LShift,");
                        break;
                    case k(ALT_RIGHT):
                        modifiers|=KEYBOARD_MODIFIER_RIGHTALT;
                        Serial.print("LAlt,");
                        break;
                    case k(GUI_RIGHT):
                        modifiers|=KEYBOARD_MODIFIER_RIGHTGUI;
                        Serial.print("LGUI,");
                        break;
                }
            }
        } while(j); /* read_pins_left */
        digitalToggle(pins_write[i]); // Toggle back to HIGH
    } while(i); /* write_pins */

  /* Source: https://github.com/FriesFlorian/keyawesome */

    if(keycount>0) { // Keypress LED
        hasKeyPressed = true;
        Serial.print("Keys pressed: "); Serial.println(keycount);

        uint8_t keycode[6];
        arrclr(keycode);

        if (keycount<=6) {
            memcpy(keycode,keys,keycount);
            blehid.keyboardReport(modifiers, keycode);
        } else {
            memcpy(keycode,keys,6);
            blehid.keyboardReport(modifiers, keycode);
            arrclr(keycode);

            memcpy(keycode,keys,keycount-6);
            blehid.keyboardReport(modifiers, keycode);
        }
    }

    delay(100);
}

void startAdv(void)
{
    Serial.println("BT::startAdv()");
    // Advertising packet
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();
    Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);

    // Include BLE HID service
    Bluefruit.Advertising.addService(bledis);
    Bluefruit.Advertising.addService(blebas);
    Bluefruit.Advertising.addService(blehid);

    // There is enough room for the dev name in the advertising packet
    Bluefruit.Advertising.addName();

    /* Start Advertising
    * - Enable auto advertising if disconnected
    * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
    * - Timeout for fast mode is 30 seconds
    * - Start(timeout) with timeout = 0 will advertise forever (until connected)
    *
    * For recommended advertising interval
    * https://developer.apple.com/library/content/qa/qa1931/_index.html
    */
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
    Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
    Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds

    Serial.println("BT::startAdv()::end");
}

/**
 * Callback invoked when received Set LED from central.
 * Must be set previously with setKeyboardLedCallback()
 *
 * The LED bit map is as follows: (also defined by KEYBOARD_LED_* )
 *    Kana (4) | Compose (3) | ScrollLock (2) | CapsLock (1) | Numlock (0)
 */
void set_keyboard_led(uint16_t conn_handle, uint8_t led_bitmap)
{
  (void) conn_handle;

  // light up Red Led if any bits is set
  if (led_bitmap & KEYBOARD_LED_CAPSLOCK)
  {
    ledOn( LED_RED );
  }
  else
  {
    ledOff( LED_RED );
  }
}

/* // Scan from serial and write to BT code from NRF52 examples
void loop()
{
  // Only send KeyRelease if previously pressed to avoid sending
  // multiple keyRelease reports (that consume memory and bandwidth)
  if ( hasKeyPressed )
  {
    hasKeyPressed = false;
    blehid.keyRelease();

    // Delay a bit after a report
    delay(5);
  }

  if (Serial.available())
  {
    char ch = (char) Serial.read();

    // echo
    Serial.write(ch);

    blehid.keyPress(ch);
    hasKeyPressed = true;

    // Delay a bit after a report
    delay(5);
  }
}
*/

// vim: nowrap ts=4 sw=4 et cin:
