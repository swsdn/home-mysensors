// Enable debug prints to serial monitor
#define MY_DEBUG 

#include <SPI.h>
#include <Bounce2.h>
#include <Wire.h>

// Enable repeater functionality for this node
// #define MY_REPEATER_FEATURE

#define NUMBER_OF_BUTTONS      32
#define FIRST_BUTTON_PIN       22
#define INITIAL_DELAY       10000
#define LONG_PRESS_OFFSET    1000

#define I2C_RECEIVER 4

Bounce2::Button *buttons[NUMBER_OF_BUTTONS];
bool longPressState[NUMBER_OF_BUTTONS];

void setup() {
  for (int sensor = 0 ; sensor < NUMBER_OF_BUTTONS; sensor++) {
    buttons[sensor] = new Bounce2::Button();
  }
  Wire.begin(); 
  delay(INITIAL_DELAY);
  for (int i = 0 ; i < NUMBER_OF_BUTTONS; i++) {
    int pin = i + FIRST_BUTTON_PIN;
    configure(*buttons[i], pin);
    longPressState[i] = false;
  }
  Serial.begin(115200);
  Serial.println("Started");
}

void configure(Bounce2::Button &btn, int pin) {
  btn.attach(pin, INPUT_PULLUP);
  btn.interval(5);
  btn.setPressedState(LOW);
}

void loop() {
  for (int i = 0 ; i < NUMBER_OF_BUTTONS; i++) {
    Bounce2::Button *btn = buttons[i];
    btn->update();
    if (btn->pressed()) {
      i2cSend(i);
      longPressState[i] = false;
    }
    if (btn->isPressed() && !longPressState[i] && btn->currentDuration() > 1000) {
      longPressState[i] = true;
      i2cSend(i + LONG_PRESS_OFFSET);
    }
  }
}

void i2cSend(int button) {
  Wire.beginTransmission(I2C_RECEIVER);
  Wire.write(button);
  Wire.endTransmission();
}

void printDebugToSerial(Bounce2::Button *button, int buttonNo) {
  int value = button->read();
  long duration = button->currentDuration();
  Serial.print("button="); Serial.print(buttonNo);
  Serial.print(" value="); Serial.print(value);
  Serial.print(" duration="); Serial.println(duration);
}
