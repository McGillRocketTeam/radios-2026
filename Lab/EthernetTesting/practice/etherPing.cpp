#include <NativeEthernet.h>

byte mac[] = { 0x04, 0xE9, 0xE5, 0x12, 0x34, 0x56 };
IPAddress ip(192, 168, 8, 50);         // Teensy's static IP
IPAddress serverIp(192, 168, 8, 107);  // Your PC running the Python server
const int serverPort = 9000;

EthernetClient client;
unsigned long lastPingTime = 0;
const unsigned long pingInterval = 10;
String currentCommand;
int count = 0;

void setup() {
  Serial.begin(115200);  
  Ethernet.begin(mac, ip);
  delay(1000);

  Serial.print("Local IP: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  if (!client.connected()) {
    Serial.println("Connecting to server...");
    if (client.connect(serverIp, serverPort)) {
      Serial.println("Connected!");
    } else {
      Serial.println("Connection failed.");
      delay(2000);  
      return;
    }
  }

  unsigned long now = millis();
  if (now - lastPingTime >= pingInterval) {
    client.println(count);
    count++;
    lastPingTime = now;

    while (client.available()) {
      char receivedChar = client.read();

      if (receivedChar == '\n'){
        Serial.print("Received message:");
        Serial.println(currentCommand);
        currentCommand = "";
      } else {
        currentCommand += receivedChar;
      }

    }
  }
}
