// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable serial gateway
#define MY_GATEWAY_SERIAL

#include <SPI.h>
#include <MySensors.h>
#include <Wire.h>

// Enable repeater functionality for this node
#define MY_REPEATER_FEATURE

#define FIRST_RELAY_PIN  22
#define NUMBER_OF_RELAYS 32
#define RELAY_ON LOW
#define RELAY_OFF HIGH

#define I2C_RECEIVER 4

MyMessage *messages[NUMBER_OF_RELAYS];
bool state[NUMBER_OF_RELAYS];
long saveStateTime = millis();
byte buttons[] = {
   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 
  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 
  20, 21, 22, 23, 24, 25, 26, 27, 3, 0
};

void before() { 
  for (int sensor=0, pin=FIRST_RELAY_PIN; sensor < NUMBER_OF_RELAYS; sensor++, pin++) {
    pinMode(pin, OUTPUT);   
    messages[sensor] = new MyMessage(sensor, V_STATUS);
    state[sensor] = loadState(sensor);
    digitalWrite(pin, state[sensor] ? RELAY_ON : RELAY_OFF);
  }
}

void setup() {
  Wire.begin(4);
  Wire.onReceive(i2cReceive);
  delay(3000);
}

void presentation()  
{   
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Relays", "1.0");

  for (int sensor=0; sensor < NUMBER_OF_RELAYS; sensor++) {
    // Register all sensors to gw (they will be created as child devices)
    present(sensor, S_BINARY);
  }
}

void loop() { 
  if (millis() - saveStateTime > 60000) {
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
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type==V_STATUS) {
     state[message.sensor] = message.getBool();
     int pin = message.sensor + FIRST_RELAY_PIN;
     bool newState = state[message.sensor] ? RELAY_ON : RELAY_OFF;
     digitalWrite(pin, newState);
     
     // Write some debug info
     Serial.print(pin);
     Serial.print("/");
     Serial.print(newState);
     Serial.print(" Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   } 
}

void i2cReceive(int howMany) {
  Serial.print("Received ");
  Serial.print(howMany);
  Serial.print(" bytes,");
  int button = Wire.read();
  bool newState = Wire.read();
  int sensor = buttons[button];

  Serial.print(" button ");
  Serial.print(button);
  Serial.print(" sensor ");
  Serial.print(sensor);
  Serial.print(" newState ");
  Serial.println(newState);
  state[sensor] = newState;
  int pin = sensor + FIRST_RELAY_PIN;
  digitalWrite(pin, state[sensor] ? RELAY_ON : RELAY_OFF);
}
