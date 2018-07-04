/*********************************************************************
 Key Matrix code
 See NOTICE file for details.
*********************************************************************/
#include <bluefruit.h>

#define GPIO(x) (x)
#define k(x) HID_KEY_##x

#define KEY_PRESSED_STATE (LOW)

/* Using an IDE connector set as a mount and bus allows me to seperate the pins
   of the AdaFruit nRF52 into the read and write set of pins, with the Battery
   level analog pin as the separator between the read and write sets. The set
   with lesser pins is designated pins_write as they are shared between multiple
   matrix segments: the keyboards has atleast 2 segments.
*/
uint32_t pins_read [] = {
                               GPIO(16), GPIO(15), GPIO( 7), GPIO(11),/*GPIO(31),*/
    GPIO(2), GPIO(3), GPIO(4), GPIO( 5), GPIO(28), GPIO(29), GPIO(12),
};

uint32_t pins_write [] = { /* P31(VBAT ADC) is separator */
/*GPIO(31),*/ GPIO(30), GPIO(27), GPIO(26), GPIO(25),
    GPIO(13), GPIO(14), GPIO( 8), GPIO( 6), GPIO(20),
};

uint32_t pincount_read = uint32_t(sizeof(pins_read)/sizeof(pins_read[0]));
uint32_t pincount_write = uint32_t(sizeof(pins_write)/sizeof(pins_write[0]));

uint8_t HID_KEY_FN = k(NONE);
uint8_t keys_left_half[][7] = { 
  { k(GRAVE), k(1), k(2), k(3), k(4), k(5), k(6) },
  { k(TAB), k(Q), k(W), k(E), k(R), k(T), k(Y)},
  { k(CAPS_LOCK), k(A), k(S), k(D), k(F), k(G), k(H)},
  { k(SHIFT_LEFT), k(Z), k(X), k(C), k(V), k(B)},
  { k(FN), k(CONTROL_LEFT), k(ALT_LEFT), k(GUI_LEFT), k(SPACE)},
};
uint8_t keys_right_half[][7] = {
  { k(7), k(8), k(9), k(0), k(MINUS), k(EQUAL), k(BACKSPACE)},
  { k(U), k(I), k(O), k(P), k(BRACKET_LEFT), k(BRACKET_RIGHT), k(BACKSLASH)},
  { k(J), k(K), k(L), k(SEMICOLON), k(APOSTROPHE), k(RETURN)},
  { k(N), k(M), k(COMMA), k(PERIOD), k(SLASH), k(DELETE), k(SHIFT_RIGHT)},
  { k(SPACE), k(ALT_RIGHT), k(ESCAPE), k(ARROW_UP), k(ARROW_LEFT), k(ARROW_DOWN), k(ARROW_RIGHT)}
};

uint8_t state;

void init_keymatrix()
{
  // Writing pins in output mode, default is HIGH
  uint32_t i = pincount_write; 
  do {
    pinMode(pins_write[--i], OUTPUT);
  } while(i);

  // Reading pins in INPUT mode with PULLUP
  i = pincount_read; 
  do {
    pinMode(pins_read[--i], INPUT_PULLUP);
  } while(i);
}

/* Function to scan key matrix and return the keys pressed */
uint8_t scankeys()
{
  uint_fast32_t cnt = 0;

  // Loop through write pins, setting them low, and scan read pins for LOW
  uint_fast32_t i = pincount_write; 
  do {
    digitalToggle(pins_write[--i]); // Toggle to LOW
    uint_fast32_t j = pincount_read; 
    do {
      state=digitalRead(pins_read[--j]);
      if (state == KEY_PRESSED_STATE) {
        Serial.print("("); Serial.print(i); Serial.print(","); Serial.print(j); Serial.println(")");
        ++cnt;
      }
    }while(j);
    digitalToggle(pins_write[i]); // Toggle back to HIGH
  } while(i);
  return (cnt);
  /* Source: https://github.com/FriesFlorian/keyawesome */
}
