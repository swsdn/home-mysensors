// Enable debug prints to serial monitor
#define MY_DEBUG 

#include <SPI.h>
#include <Bounce2.h>
#include <Wire.h>

// Enable repeater functionality for this node
// #define MY_REPEATER_FEATURE

#define NUMBER_OF_BUTTONS 32
#define FIRST_BUTTON_PIN 22
#define INITIAL_DELAY 10000

#define I2C_RECEIVER 4

Bounce2::Button *buttons[NUMBER_OF_BUTTONS];

void setup() {
  for (int sensor = 0 ; sensor < NUMBER_OF_BUTTONS; sensor++) {
    buttons[sensor] = new Bounce2::Button();
  }
  Wire.begin(); 
  delay(INITIAL_DELAY);
  for (int i = 0 ; i < NUMBER_OF_BUTTONS; i++) {
    int pin = i + FIRST_BUTTON_PIN;
    configure(*buttons[i], pin);
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
    if (btn->update()) {
      int pin = i + FIRST_BUTTON_PIN;
      if (btn->pressed()) {
        i2cSend(i);
      }

      if (btn->read() == LOW && btn->previousDuration() > 1000) {
        Serial.println(btn->read());
        Serial.println(btn->currentDuration());
        printDebugToSerial(btn, pin);
      }
    }
  }
}

void i2cSend(int button) {
  Wire.beginTransmission(I2C_RECEIVER);
  Wire.write(button);
  Wire.endTransmission();
}

void printDebugToSerial(Bounce2::Button *button, int pin) {
  int value = button->read();
  long duration = button->previousDuration();
  Serial.print("pin="); Serial.print(pin);
  Serial.print(" value="); Serial.print(value);
  Serial.print(" duration="); Serial.println(duration);
}
