// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable serial gateway
#define MY_GATEWAY_SERIAL

#include <SPI.h>
#include <MySensors.h>  

// Enable repeater functionality for this node
#define MY_REPEATER_FEATURE

#define FIRST_RELAY_PIN  4
#define NUMBER_OF_RELAYS 2
#define RELAY_ON  0
#define RELAY_OFF 1

MyMessage *messages[NUMBER_OF_RELAYS];
bool state[NUMBER_OF_RELAYS];
long saveStateTime = millis();

void before() { 
  for (int sensor=0, pin=FIRST_RELAY_PIN; sensor < NUMBER_OF_RELAYS; sensor++, pin++) {
    pinMode(pin, INPUT_PULLUP);   
    messages[sensor] = new MyMessage(sensor, V_STATUS);
    state[sensor] = loadState(sensor);
    digitalWrite(pin, state[sensor] ? RELAY_ON : RELAY_OFF);
  }
}

void setup() { 
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
     // Change relay state
     state[message.sensor] = message.getBool() ? RELAY_ON : RELAY_OFF;
     digitalWrite(message.sensor + FIRST_RELAY_PIN, state[message.sensor]);
     // Store state in eeprom
     saveState(message.sensor, message.getBool());
     // Write some debug info
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   } 
}
