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
    Serial.println("KeyPress.ino");
    Serial.println("------------");
    Serial.println("setup()");

    pinMode(LED_RED, OUTPUT); // POWER indicator

    init_keymatrix();
    Serial.println("Finished setup()");
}

void loop() {
    keycount=scankeys();
    Serial.print("Keys pressed: "); Serial.println(keycount);
    if(!(++loopcount%20))
        ledToggle(LED_RED);
    delay(100);
}
