#define NUM_ELEMENTS(x) (sizeof(x) / sizeof(x[0]))

// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable serial gateway
#define MY_GATEWAY_SERIAL

#define RELAY_UNKNOWN         0
// BOARD 1
// BOARD 2
// BOARD 3
// BOARD 4
#define RELAY_HALL_1          0
#define RELAY_BATH_2_SCONCE_1 1
#define RELAY_BATH_2_SCONCE_2 2
#define RELAY_BATH_2_MAIN     3
#define RELAY_BATH_1_SCONCE_1 4
#define RELAY_BATH_1_SCONCE_2 5
#define RELAY_BATH_1_MAIN     6
#define RELAY_BATH_1_MAIN     7

#include <SPI.h>
#include <MySensors.h>
#include <Wire.h>

// Enable repeater functionality for this node
#define MY_REPEATER_FEATURE

#define FIRST_RELAY_PIN  22
#define NUMBER_OF_RELAYS 32
#define RELAY_ON LOW
#define RELAY_OFF HIGH
#define SAVE_STATE_INTERVAL 10000
#define INITIAL_DELAY 10000

#define I2C_RECEIVER 4

MyMessage *messages[NUMBER_OF_RELAYS];
bool state[NUMBER_OF_RELAYS];
long saveStateTime = millis();

// byte relaysOfButton[] = {
//    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
//   10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
//   20, 21, 22, 23, 24, 25, 26, 27, 3, 0
// };

struct Button {
    byte *relays;
    int numElements;
};

byte BUTTON_00_[] = {};
byte BUTTON_01_[] = {};
byte BUTTON_02_[] = {};
byte BUTTON_03_[] = {};
byte BUTTON_04_[] = {};
byte BUTTON_05_[] = {};
byte BUTTON_06_[] = {};
byte BUTTON_07_[] = {};
byte BUTTON_08_[] = {};
byte BUTTON_09_[] = {};
byte BUTTON_10_[] = {};
byte BUTTON_11_[] = {};
byte BUTTON_12_[] = {};
byte BUTTON_13_[] = {};
byte BUTTON_14_[] = {};
byte BUTTON_15_[] = {};
byte BUTTON_16_[] = {};
byte BUTTON_17_[] = {};
byte BUTTON_18_[] = {};
byte BUTTON_19_[] = {};
byte BUTTON_20_[] = {};
byte BUTTON_21_[] = {};
byte BUTTON_22_[] = {};
byte BUTTON_23_[] = {};
byte BUTTON_24_[] = {};
byte BUTTON_25_[] = {};
byte BUTTON_26_[] = {};
byte BUTTON_27_[] = {};
byte BUTTON_28_ENTRY_1[] = { RELAY_HALL_1, RELAY_BATH_2_MAIN, RELAY_BATH_1_MAIN };
byte BUTTON_29_ENTRY_2[] = { RELAY_HALL_1 };

Button buttonRelays[] = {
    { BUTTON_00_, 0 },
    { BUTTON_01_, 0 },
    { BUTTON_02_, 0 },
    { BUTTON_03_, 0 },
    { BUTTON_04_, 0 },
    { BUTTON_05_, 0 },
    { BUTTON_06_, 0 },
    { BUTTON_07_, 0 },
    { BUTTON_08_, 0 },
    { BUTTON_09_, 0 },
    { BUTTON_10_, 0 },
    { BUTTON_11_, 0 },
    { BUTTON_12_, 0 },
    { BUTTON_13_, 0 },
    { BUTTON_14_, 0 },
    { BUTTON_15_, 0 },
    { BUTTON_16_, 0 },
    { BUTTON_17_, 0 },
    { BUTTON_18_, 0 },
    { BUTTON_19_, 0 },
    { BUTTON_20_, 0 },
    { BUTTON_21_, 0 },
    { BUTTON_22_, 0 },
    { BUTTON_23_, 0 },
    { BUTTON_24_, 0 },
    { BUTTON_25_, 0 },
    { BUTTON_26_, 0 },
    { BUTTON_27_, 0 },
    { BUTTON_28_ENTRY_1, NUM_ELEMENTS(BUTTON_28_ENTRY_1) },
    { BUTTON_29_ENTRY_2, NUM_ELEMENTS(BUTTON_29_ENTRY_2) }
};

void before() { 
  for (int relay = 0, pin = FIRST_RELAY_PIN; relay < NUMBER_OF_RELAYS; relay++, pin++) {
    pinMode(pin, OUTPUT);   
    messages[relay] = new MyMessage(relay, V_STATUS);
    state[relay] = loadState(relay);
    digitalWrite(pin, state[relay] ? RELAY_ON : RELAY_OFF);
  }
}

void setup() {
  Wire.begin(I2C_RECEIVER);
  Wire.onReceive(i2cReceive);
  delay(INITIAL_DELAY);
}

void presentation()  
{   
  sendSketchInfo("Relays", "1.0");
  for (int sensor=0; sensor < NUMBER_OF_RELAYS; sensor++) {
    present(sensor, S_BINARY);
  }
}

void loop() { 
  if (millis() - saveStateTime > SAVE_STATE_INTERVAL) {
    saveStateToEeprom();
    saveStateTime = millis();
  }
}

void saveStateToEeprom() {
  Serial.println("Saving state to eeprom");
  int saved = 0;
  for (int i = 0 ; i < NUMBER_OF_RELAYS; i++) {
    if (state[i] != loadState(i)) {
      saveState(i, state[i]);
      saved++;
    }
  }
  Serial.print("Saved to eeprom: ");Serial.println(saved);
}

void receive(const MyMessage &message) {
  if (message.type == V_STATUS && message.sensor < NUMBER_OF_RELAYS) {
    state[message.sensor] = message.getBool();
    int pin = message.sensor + FIRST_RELAY_PIN;
    bool newState = state[message.sensor] ? RELAY_ON : RELAY_OFF;
    digitalWrite(pin, newState);
    printDebugToSerial(pin, newState, message);
  }
}

void printDebugToSerial(int pin, bool newState, MyMessage message) {
  Serial.print(pin);
  Serial.print("/");
  Serial.print(newState);
  Serial.print(" Incoming change for sensor:");
  Serial.print(message.sensor);
  Serial.print(", New status: ");
  Serial.println(message.getBool());
}

void i2cReceive(int howMany) {
  int buttonId = Wire.read();
  bool newState = Wire.read();
//   int relay = relaysOfButton[buttonId];
  Button button = buttonRelays[buttonId];
  for (int i = 0; i < button.numElements; i++) {
    state[button.relays[i]] = newState;
    int pin = button.relays[i] + FIRST_RELAY_PIN;
    digitalWrite(pin, state[button.relays[i]] ? RELAY_ON : RELAY_OFF);
    printDebugToSerial(howMany, buttonId, state[button.relays[i]], newState);
  }
//   state[relay] = newState;
//   int pin = relay + FIRST_RELAY_PIN;
//   digitalWrite(pin, state[relay] ? RELAY_ON : RELAY_OFF);
//   printDebugToSerial(howMany, button, relay, newState);
}

void printDebugToSerial(int howMany, int button, int relay, bool newState) {
  Serial.print("Received ");
  Serial.print(howMany);
  Serial.print(" bytes,");
  Serial.print(" button ");
  Serial.print(button);
  Serial.print(" sensor ");
  Serial.print(relay);
  Serial.print(" newState ");
  Serial.println(newState);
}