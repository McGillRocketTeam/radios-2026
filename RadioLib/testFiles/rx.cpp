#include <Arduino.h>
#include <RadioLib.h>
#include <config/Config.h>
#include <config/PinLayout.h>

SX1262 radio = new Module(PIN_NSS, PIN_DIO1, PIN_RST, PIN_BUSY);

void setup() {
  // Begin serial monitoring
  Serial.begin(9600);
  delay(1500);
  Serial.println("Initialising SX1262 transmitter...");

  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("SX1262 initialisation successful");
  }
  else {
    Serial.print("Init failed, code ");
    Serial.println(state);
    while (true);
  }

  // Configure radio
  radio.setFrequency(FREQUENCY);
  radio.setBandwidth(BANDWIDTH);
  radio.setSpreadingFactor(SPREADING_FACTOR);
}

void loop() {
  String msg;
  int state = radio.receive(msg);

  if (state == RADIOLIB_ERR_NONE) {
    // Successfully received a packet
    Serial.print("Success! ");
    Serial.print("Data: ");
    Serial.println(msg);
  }
  else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
    // Timeout occurred while waiting for a packet
    Serial.println("Timeout");
  }
  else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
    // Packet was received, but it was malformed
    Serial.println("Received corrupted packet");
  }
  else {
    Serial.print("Receive failed, code ");
    Serial.println(state);
  }
}