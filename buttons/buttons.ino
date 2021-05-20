// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable serial gateway
#define MY_GATEWAY_SERIAL

#include <SPI.h>
#include <MySensors.h>  
#include <Bounce2.h>

// Enable repeater functionality for this node
// #define MY_REPEATER_FEATURE/

// #define NUMBER_OF_BUTTONS 30
// #define FIRST_BUTTON_PIN 24

#define NUMBER_OF_BUTTONS 10
#define FIRST_BUTTON_PIN 4

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
  delay(3000); // Setup locally attached sensors
  
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
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Buttons", "1.0");

  for (int sensor = 0; sensor < NUMBER_OF_BUTTONS; sensor++) {
    // Register all sensors to gw (they will be created as child devices)
    present(sensor, S_BINARY);
  }
}

void loop() { 
  for (int i = 0 ; i < NUMBER_OF_BUTTONS; i++) {
    if (buttons[i]->update()) {
      int pin = i + FIRST_BUTTON_PIN;
      if (buttons[i]->pressed()) {
//        saveState(i, !loadState(i));
        state[i] = !state[i];
        send(messages[i]->set(state[i]));
      }
    }
  }

  if (millis() - saveStateTime > 60000) {
    saveStateToEeprom();
    saveStateTime = millis();
  }
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
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type==V_STATUS) {
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   } 
}
