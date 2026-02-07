#include "ConsoleRouter.h"
#include "Config.h"
#include "MqttTopics.h"

#if TEENSY != 41

ConsoleRouter::ConsoleRouter() {}

void ConsoleRouter::begin(MqttTopic::Role role, CommandParser& parser) {
  Serial.begin(GS_SERIAL_BAUD_RATE);
  commandParser = &parser;
  setTopics(role); // optional if you still want topics defined for prints
}

void ConsoleRouter::handleConsoleReconnect() { /* no-op */ }
void ConsoleRouter::mqttLoop() { /* no-op */ }

void ConsoleRouter::sendTelemetry(const uint8_t*, size_t) { /* no-op */ }
void ConsoleRouter::sendStatus() { /* no-op */ }
void ConsoleRouter::sendCmdAckRx(const String&) { /* no-op */ }
void ConsoleRouter::sendCmdAckTx(int) { /* no-op */ }

void ConsoleRouter::ethernetInit() { /* no-op */ }
bool ConsoleRouter::mqttReconnect() { return false; }
bool ConsoleRouter::publishAck(const char*, uint8_t) { return false; }

ConsoleRouter& ConsoleRouter::getInstance() {
  static ConsoleRouter instance;
  return instance;
}

int ConsoleRouter::available() { return Serial.available(); }
int ConsoleRouter::read() { return Serial.available() ? Serial.read() : -1; }
int ConsoleRouter::peek() { return Serial.available() ? Serial.peek() : -1; }

size_t ConsoleRouter::write(uint8_t c) { return Serial.write(c); }
size_t ConsoleRouter::write(const uint8_t* buffer, size_t size) {
  Serial.write(buffer, size);
  return size;
}

void ConsoleRouter::setTopics(MqttTopic::Role role) {
  // You can pick a default band or keep whatever logic you want here
  // band = MqttTopic::Band::A;
  telemetryTopic = nullptr;
  metadataTopic  = nullptr;
  ackTopic       = nullptr;
  commandTopic   = nullptr;
  debugTopic     = nullptr;
}

void ConsoleRouter::_publishBytes(const uint8_t*, size_t) {}
void ConsoleRouter::_printString(const String& s, bool newline) {
  if (newline) Serial.println(s); else Serial.print(s);
}

#endif
