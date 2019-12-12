/*********************************************************************
 This code is for the Bluefruit nRF52 Feather and started off as the
 Arduino Blinky example. Much mutated since then.
 See NOTICE file for details.
*********************************************************************/
#define GPIO(x) (x)

uint32_t GPIO_IN1=GPIO(25);
uint32_t GPIO_OUT1=GPIO(20);

uint_fast8_t keycount=0;
uint_fast8_t loopcount=0;

void setup() {
    Serial.begin(115200);
    while ( !Serial ) delay(10);   // for nrf52840 with native usb

    Serial.println("KeyPress");
    Serial.println("------------");
    Serial.println("main:setup()");

    pinMode(LED_RED, OUTPUT); // POWER indicator

    // Setup BT
    //setup_bt();

    // Keymatrix initialization
    init_keymatrix();

    Serial.println("main:setup()::end");
}

void loop() {
    if(!(++loopcount%20)) // Loop indicator LED
        digitalToggle(LED_RED);

    keycount=scankeys();

    if(keycount>0) { // Keypress LED
        digitalToggle(LED_BLUE);
        Serial.print("Keys pressed: "); Serial.println(keycount);
    }

    delay(100);
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
