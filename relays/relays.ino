#define NUM_ELEMENTS(x) (sizeof(x) / sizeof(x[0]))

// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable serial gateway
#define MY_GATEWAY_SERIAL

// BOARD 1
#define RELAY_TV               24
#define RELAY_KITCHEN_SCONCE_1 25
#define RELAY_KITCHEN_SCONCE_2 26
#define RELAY_KITCHEN_SCONCE_3 27
#define RELAY_KITCHEN_SCONCE_4 28
#define RELAY_KITCHEN_MAIN_1   29
#define RELAY_KITCHEN_MAIN_2   30
#define RELAY_KITCHEN_MAIN_3   31
// BOARD 2
#define RELAY_LIVING_SCONCE_1   8
#define RELAY_LIVING_SCONCE_2   9
#define RELAY_LIVING_1         10
#define RELAY_HALL_2           11
#define RELAY_LIVING_2         12
#define RELAY_BED_3_1          13
#define RELAY_BED_3_2          14
#define RELAY_BED_2_1          15
// BOARD 3
#define RELAY_BED_2_2          16
#define RELAY_BED_1_SCONCE_1   17
#define RELAY_BED_1_SCONCE_2   18
#define RELAY_BED_1_MAIN_1     19
#define RELAY_BED_1_MAIN_2     20
#define RELAY_ENTRY_1          21
#define RELAY_ENTRY_2          22
#define RELAY_DINING           23
// BOARD 4
#define RELAY_HALL_1           0
#define RELAY_BATH_2_SCONCE_1  1
#define RELAY_BATH_2_SCONCE_2  2
#define RELAY_BATH_2_MAIN      3
#define RELAY_BATH_1_SCONCE_1  4
#define RELAY_BATH_1_SCONCE_2  5
#define RELAY_BATH_1_MAIN_1    6
#define RELAY_BATH_1_MAIN_2    7

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
#define INITIAL_DELAY 5000

#define I2C_RECEIVER 4

MyMessage *messages[NUMBER_OF_RELAYS];
bool state[NUMBER_OF_RELAYS];
long saveStateTime = millis();

struct Button {
    byte *relays;
    int numElements;
};

// in comments physical buttons starting from left
byte BUTTON_00_BATH_1[]  = { RELAY_BATH_1_MAIN_1, RELAY_BATH_1_MAIN_2 };
byte BUTTON_01_BATH_1[]  = { RELAY_BATH_1_MAIN_1, RELAY_BATH_1_MAIN_2 };
byte BUTTON_02_BED_1[]   = { RELAY_BED_1_MAIN_1, RELAY_BED_1_MAIN_2 };
byte BUTTON_03_BED_1[]   = { RELAY_BED_1_MAIN_1, RELAY_BED_1_MAIN_2 };
byte BUTTON_04_BED_1[]   = { RELAY_BED_1_SCONCE_1 };
byte BUTTON_05_BED_1[]   = { RELAY_BED_1_SCONCE_2 };
byte BUTTON_06_BED_3[]   = { RELAY_BED_3_1, RELAY_BED_3_2 };
byte BUTTON_07_BED_3[]   = { RELAY_BED_3_1, RELAY_BED_3_2 };
byte BUTTON_08_BATH_1[]  = { RELAY_BATH_1_SCONCE_1 };
byte BUTTON_09_BATH_1[]  = { RELAY_BATH_1_SCONCE_2 };
byte BUTTON_10_BED_1[]   = { RELAY_BED_1_MAIN_1, RELAY_BED_1_MAIN_2 };
byte BUTTON_11_BED_1[]   = { RELAY_BED_1_MAIN_1, RELAY_BED_1_MAIN_2 };
byte BUTTON_12_BATH_2[]  = { RELAY_BATH_2_MAIN };
byte BUTTON_13_BATH_2[]  = { RELAY_BATH_2_MAIN };
byte BUTTON_14_KITCHEN[] = { RELAY_KITCHEN_SCONCE_1, RELAY_KITCHEN_SCONCE_2 };
byte BUTTON_15_KITCHEN[] = { RELAY_KITCHEN_SCONCE_3, RELAY_KITCHEN_SCONCE_4 };
byte BUTTON_16_BATH_2[]  = { RELAY_BATH_2_SCONCE_1 };
byte BUTTON_17_BATH_2[]  = { RELAY_BATH_2_SCONCE_2 };
byte BUTTON_18_LIVING[]  = { RELAY_TV };
byte BUTTON_19_LIVING[]  = { RELAY_LIVING_SCONCE_1, RELAY_LIVING_SCONCE_2};
byte BUTTON_20_MAIN[]    = { RELAY_DINING }; // 6th
byte BUTTON_21_[]        = { }; // not connected
byte BUTTON_22_[]        = { }; // not connected
byte BUTTON_23_MAIN[]    = { RELAY_HALL_2 }; // 5th
byte BUTTON_24_BED_2[]   = { RELAY_BED_2_1 };
byte BUTTON_25_BED_2[]   = { RELAY_BED_2_2 };
byte BUTTON_26_MAIN[]    = { RELAY_LIVING_1 }; // 1st
byte BUTTON_27_MAIN[]    = { RELAY_KITCHEN_MAIN_2 }; // 4th
byte BUTTON_28_MAIN[]    = { RELAY_LIVING_2 }; // 2nd
byte BUTTON_29_MAIN[]    = { RELAY_KITCHEN_MAIN_1, RELAY_KITCHEN_MAIN_2, RELAY_KITCHEN_MAIN_3 }; // 3rd
byte BUTTON_30_ENTRY[]   = { RELAY_ENTRY_1, RELAY_ENTRY_2 };
byte BUTTON_31_ENTRY[]   = { RELAY_HALL_1 };

Button buttonRelays[] = {
    { BUTTON_00_BATH_1,  NUM_ELEMENTS(BUTTON_00_BATH_1) },
    { BUTTON_01_BATH_1,  NUM_ELEMENTS(BUTTON_01_BATH_1) },
    { BUTTON_02_BED_1,   NUM_ELEMENTS(BUTTON_02_BED_1) },
    { BUTTON_03_BED_1,   NUM_ELEMENTS(BUTTON_03_BED_1) },
    { BUTTON_04_BED_1,   NUM_ELEMENTS(BUTTON_04_BED_1) },
    { BUTTON_05_BED_1,   NUM_ELEMENTS(BUTTON_05_BED_1) },
    { BUTTON_06_BED_3,   NUM_ELEMENTS(BUTTON_06_BED_3) },
    { BUTTON_07_BED_3,   NUM_ELEMENTS(BUTTON_07_BED_3) },
    { BUTTON_08_BATH_1,  NUM_ELEMENTS(BUTTON_08_BATH_1) },
    { BUTTON_09_BATH_1,  NUM_ELEMENTS(BUTTON_09_BATH_1) },
    { BUTTON_10_BED_1,   NUM_ELEMENTS(BUTTON_10_BED_1) },
    { BUTTON_11_BED_1,   NUM_ELEMENTS(BUTTON_11_BED_1) },
    { BUTTON_12_BATH_2,  NUM_ELEMENTS(BUTTON_12_BATH_2) },
    { BUTTON_13_BATH_2,  NUM_ELEMENTS(BUTTON_13_BATH_2) },
    { BUTTON_14_KITCHEN, NUM_ELEMENTS(BUTTON_14_KITCHEN) },
    { BUTTON_15_KITCHEN, NUM_ELEMENTS(BUTTON_15_KITCHEN) },
    { BUTTON_16_BATH_2,  NUM_ELEMENTS(BUTTON_16_BATH_2) },
    { BUTTON_17_BATH_2,  NUM_ELEMENTS(BUTTON_17_BATH_2) },
    { BUTTON_18_LIVING,  NUM_ELEMENTS(BUTTON_18_LIVING) },
    { BUTTON_19_LIVING,  NUM_ELEMENTS(BUTTON_19_LIVING) },
    { BUTTON_20_MAIN,    NUM_ELEMENTS(BUTTON_20_MAIN) },
    { BUTTON_21_,        NUM_ELEMENTS(BUTTON_21_) },
    { BUTTON_22_,        NUM_ELEMENTS(BUTTON_22_) },
    { BUTTON_23_MAIN,    NUM_ELEMENTS(BUTTON_23_MAIN) },
    { BUTTON_24_BED_2,   NUM_ELEMENTS(BUTTON_24_BED_2) },
    { BUTTON_25_BED_2,   NUM_ELEMENTS(BUTTON_25_BED_2) },
    { BUTTON_26_MAIN,    NUM_ELEMENTS(BUTTON_26_MAIN) },
    { BUTTON_27_MAIN,    NUM_ELEMENTS(BUTTON_27_MAIN) },
    { BUTTON_28_MAIN,    NUM_ELEMENTS(BUTTON_28_MAIN) },
    { BUTTON_29_MAIN,    NUM_ELEMENTS(BUTTON_29_MAIN) },
    { BUTTON_30_ENTRY,   NUM_ELEMENTS(BUTTON_30_ENTRY) },
    { BUTTON_31_ENTRY,   NUM_ELEMENTS(BUTTON_31_ENTRY) }
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
  delay(INITIAL_DELAY);
  Wire.begin(I2C_RECEIVER);
  Wire.onReceive(i2cReceive);
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
  Button button = buttonRelays[buttonId];
  for (int i = 0; i < button.numElements; i++) {
    byte relay = button.relays[i];
    state[relay] = !state[relay];
    int pin = relay + FIRST_RELAY_PIN;
    send(messages[relay]->set(state[relay]));
    digitalWrite(pin, state[relay] ? RELAY_ON : RELAY_OFF);
    //printDebugToSerial(pin, buttonId, relay, state[relay]);
  }
}

void printDebugToSerial(int pin, int button, int relay, bool newState) {
  Serial.print("pin ");
  Serial.print(pin);
  Serial.print(" bytes,");
  Serial.print(" button ");
  Serial.print(button);
  Serial.print(" sensor ");
  Serial.print(relay);
  Serial.print(" newState ");
  Serial.println(newState);
}
