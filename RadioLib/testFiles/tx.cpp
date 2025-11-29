#include <Arduino.h>
#include <RadioLib.h>
#include <config/Config.h>
#include <config/PinLayout.h>

/*              Connections to Teensy 4.0
 * Pins:  13    14     15    18   22       24
 *        SCK   DIO1   BUSY  RST  FreqRes  3V3
 * ----------------------------------------------------------
 * ----------------------------------------------------------
 * ----------------------------------------------------------------
 * ----------- SX1262 Breakout ------------------------------------
 * ----------------------------------------------------------------
 * ----------------------------------------------------------
 * ----------------------------------------------------------
 *        RX1/TXD   TX1/RXD    NSS
 *        0         1          10
 */
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
  radio.setOutputPower(POWER_OUTPUT);
}

void loop() {
  char *msg = "Hello LoRa!";
  int state = radio.transmit(msg);

  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("Transmit failed, code ");
    Serial.println(state);
  }

  delay(2000);
}