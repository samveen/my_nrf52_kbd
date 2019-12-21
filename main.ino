/*********************************************************************
 This code targets the Bluefruit nRF52 Feather and started life as the
 Adafruit Blinky example. Much mutated since then.
 See NOTICE file for details.
*********************************************************************/
#include <bluefruit.h>

// Macros
#define GPIO(x) (x)
#define k(x) HID_KEY_##x

// Constants
#define KEY_PRESSED_STATE (LOW)
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
/************* INFO SECTION END   ******************/

BLEDis bledis;
BLEHidAdafruit blehid;

char* kbdName="3245WW+ via nRF52832";
char* manufacturer("Samveen via Adafruit");

uint32_t GPIO_IN1=GPIO(25);
uint32_t GPIO_OUT1=GPIO(20);

uint_fast8_t keycount=0;

bool hasKeyPressed=false;

/****** Key Matrix Section START ****/
uint8_t state;

/* Left READ pins */
uint32_t pins_read_left[] = {GPIO(02),GPIO(03),GPIO(04),GPIO(05),GPIO(28)};
uint32_t pincount_read_left = uint32_t(sizeof(pins_read_left)/sizeof(pins_read_left[0]));

/* Right READ pins */
uint32_t pins_read_right[] = {GPIO(16),GPIO(15),GPIO(07),GPIO(11),GPIO(12)};
uint32_t pincount_read_right = uint32_t(sizeof(pins_read_right)/sizeof(pins_read_right[0]));

/* Common WRITE pins */
uint32_t pins_write[] = {GPIO(13),GPIO(30),GPIO(20),GPIO(27),GPIO(25),GPIO(26),GPIO(14)};
uint32_t pincount_write = uint32_t(sizeof(pins_write)/sizeof(pins_write[0]));

/* KeyCode Mapping to keymatrix formed by pins_read X pins_write */
/* Left half */
uint8_t keys_left[][7] = {
{ k(GRAVE) ,    k(1),   k(2),           k(3),       k(4),       k(5),   k(6)},
{ k(TAB),       k(Q),   k(W),           k(E),       k(R),       k(T),   k(Y)},
{ k(CAPS_LOCK), k(A),   k(S),           k(D),       k(F),       k(G),   k(H)},
{ k(SHIFT_LEFT),k(NONE),k(Z),           k(X),       k(C),       k(V),   k(B)},
{ k(NONE),      k(FN),  k(CONTROL_LEFT),k(ALT_LEFT),k(GUI_LEFT),k(NONE),k(SPACE)},
};

/* Right half */
uint8_t keys_right[][7] = {
{ k(7),    k(8),        k(9),     k(0),        k(MINUS),       k(EQUAL),        k(BACKSPACE)},
{ k(U),    k(I),        k(O),     k(P),        k(BRACKET_LEFT),k(BRACKET_RIGHT),k(BACKSLASH)},
{ k(J),    k(K),        k(L),     k(SEMICOLON),k(APOSTROPHE),  k(RETURN),       k(NONE)},
{ k(N),    k(M),        k(COMMA), k(PERIOD),   k(SLASH),       k(DELETE),       k(SHIFT_RIGHT)},
{ k(SPACE),k(ALT_RIGHT),k(ESCAPE),k(ARROW_UP), k(ARROW_LEFT),  k(ARROW_DOWN),   k(ARROW_RIGHT)}
};

/* Cant do more than 10 fingers in a go, now, can you? */
/* +2 for that lucky guy with 6 perfect digits */
uint8_t keys[12];

/* Modifier key flags or do I use a bitfield ? */
bool fn=false;
bool shift_l=false;
bool shift_r=false;
bool ctrl_l=false;
bool ctrl_r=false;
bool alt_l=false;
bool alt_r=false;
/****** Key Matrix Section END   ****/


void setup() {
    Serial.begin(115200);
    while ( !Serial ) delay(10);   // for nrf52840 with native usb

    Serial.println("KeyPress");
    Serial.println("------------");
    Serial.println("main::setup()::start");

    pinMode(LED_RED, OUTPUT); // POWER indicator

    Serial.println("main::setup()::BT");

    // Setup BT
    Bluefruit.begin();
    Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
    Bluefruit.setName(kbdName);

    // Configure and Start Device Information Service
    bledis.setManufacturer(kbdManufacturer);
    bledis.setModel(kbdName);
    bledis.begin();

    // Start BLE HID
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
    i=pincount_read;
    do {
      pinMode(pins_read[--i], INPUT_PULLUP);
    } while(i);

    Serial.println("main:setup()::end");
}

void loop() {

    if (hasKeyPressed) {
        hasKeyPressed = false;
        blehid.keyRelease();

        delay(5);
    }

    keycount= 0;

    /* Loop through write pins, setting them low, and scan read pins for LOW */
    uint_fast32_t i = pincount_write;
    do {
        digitalToggle(pins_write[--i]); // Toggle to LOW
        uint_fast32_t j = pincount_read_left;
        do {
            state=digitalRead(pins_read_left[--j]);
            if (state == KEY_PRESSED_STATE) {
                keys[keycount++];
                switch(keys_left_half[j][i]) {
                  case k(FN):
                      Serial.print("FN,");
                      break;
                  case k(SHIFT_LEFT):
                      Serial.print("LShift,");
                      break;
                  case k(CONTROL_LEFT):
                      Serial.print("LCtrl,");
                      break;
                  case k(ALT_LEFT):
                      Serial.print("LAlt,");
                      break;
                  case k(GUI_LEFT):
                      Serial.print("LGUI,");
                      break;
                }
               Serial.print("(");Serial.print(j);Serial.print(",");Serial.print(i);Serial.print(")-> ");
               Serial.print("(");Serial.print(pins_write[j]);Serial.print(",");Serial.print(pins_read[i]);Serial.print(")-> ");
               Serial.println(keys_left_half[j][i],HEX);
            }
        } while(j); /* read_pins_left */
        digitalToggle(pins_write[i]); // Toggle back to HIGH
    } while(i); /* write_pins */

  /* Source: https://github.com/FriesFlorian/keyawesome */

    if(keycount>0) { // Keypress LED
        hasKeyPressed = true;
        Serial.print("Keys pressed: "); Serial.println(keycount);
    }

    delay(50);
}

void startAdv(void)
{
    Serial.println("BT::startAdv()");
    // Advertising packet
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();
    Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);

    // Include BLE HID service
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
  if (led_bitmap && KEYBOARD_LED_CAPSLOCK)
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

// vim: nowrap ts=4 sw=4 et:
