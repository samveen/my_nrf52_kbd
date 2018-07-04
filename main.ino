/*********************************************************************
 This code is for the Bluefruit nRF52 Feather and started off as the
 Arduino Blinky example. Much mutated since then.
 See NOTICE file for details.
*********************************************************************/
#define GPIO(x) (x)

uint32_t GPIO_IN1=GPIO(25);
uint32_t GPIO_OUT1=GPIO(20);

void setup() {
    Serial.begin(115200);
    Serial.println("KeyPress.ino");
    Serial.println("------------");
    Serial.println("setup()");

    pinMode(LED_RED, OUTPUT); // indicator
    pinMode(GPIO_IN1, INPUT_PULLUP);
    pinMode(GPIO_OUT1, OUTPUT);
}

#define KEY_PRESSED_STATE (LOW)
uint8_t state;

void loop() {
    Serial.println("loop()");
    digitalToggle(GPIO_OUT1);
    state=digitalRead(GPIO_IN1);
    digitalToggle(GPIO_OUT1);
    if (state == KEY_PRESSED_STATE) {
        ledToggle(LED_RED);
        Serial.println("\nkey pressed");
    }
    delay(1000);
}
