// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable serial gateway
#define MY_GATEWAY_SERIAL

#include <SPI.h>
#include <MySensors.h>  
#include <Bounce2.h>
#include <Wire.h>

// Enable repeater functionality for this node
// #define MY_REPEATER_FEATURE

#define NUMBER_OF_BUTTONS 32
#define FIRST_BUTTON_PIN 22
#define SAVE_STATE_INTERVAL 10000
#define INITIAL_DELAY 10000

#define I2C_RECEIVER 4

Bounce2::Button *buttons[NUMBER_OF_BUTTONS];
MyMessage *messages[NUMBER_OF_BUTTONS];
bool state[NUMBER_OF_BUTTONS];
long saveStateTime = millis();

void before() { 
  for (int sensor = 0 ; sensor < NUMBER_OF_BUTTONS; sensor++) {
    buttons[sensor] = new Bounce2::Button();
    messages[sensor] = new MyMessage(sensor, V_STATUS);
    state[sensor] = loadState(sensor);
  }
}

void setup() {
  Wire.begin(); 
  delay(INITIAL_DELAY);
  for (int i = 0 ; i < NUMBER_OF_BUTTONS; i++) {
    int pin = i + FIRST_BUTTON_PIN;
    configure(*buttons[i], pin);
  }
}

void configure(Bounce2::Button &btn, int pin) {
  btn.attach(pin, INPUT_PULLUP);
  btn.interval(5);
  btn.setPressedState(LOW);
}

void presentation()  
{   
  sendSketchInfo("Buttons", "1.0");
  for (int sensor = 0; sensor < NUMBER_OF_BUTTONS; sensor++) {
    present(sensor, S_BINARY);
  }
}

void loop() { 
  for (int i = 0 ; i < NUMBER_OF_BUTTONS; i++) {
    if (buttons[i]->update()) {
      int pin = i + FIRST_BUTTON_PIN;
      if (buttons[i]->pressed()) {
        state[i] = !state[i];
        send(messages[i]->set(state[i]));
        i2cSend(i, state[i]);
      }
    }
  }

  if (millis() - saveStateTime > SAVE_STATE_INTERVAL) {
    saveStateToEeprom();
    saveStateTime = millis();
  }
}

void i2cSend(int sensor, bool state) {
  Wire.beginTransmission(I2C_RECEIVER);
  Wire.write(sensor);
  Wire.write(state);
  Wire.endTransmission();
}

void saveStateToEeprom() {
  Serial.println("Saving state to eeprom");
  int saved = 0;
  for (int i = 0 ; i < NUMBER_OF_BUTTONS; i++) {
    if (state[i] != loadState(i)) {
      saveState(i, state[i]);
      saved++;
    }
  }
  Serial.print("Saved to eeprom: ");Serial.println(saved);
}

void printDebugToSerial(Bounce2::Button *button, int pin) {
  int value = button->read();
  long duration = button->previousDuration();
  Serial.print("pin="); Serial.print(pin);
  Serial.print(" value="); Serial.print(value);
  Serial.print(" duration="); Serial.println(duration);
}

void receive(const MyMessage &message) {
  if (message.type == V_STATUS && message.sensor < NUMBER_OF_BUTTONS) {
    state[message.sensor] = message.getBool();
    i2cSend(message.sensor, state[message.sensor]);
       
    Serial.print("Incoming change for sensor:");
    Serial.print(message.sensor);
    Serial.print(", New status: ");
    Serial.println(message.getBool());
  } 
}
