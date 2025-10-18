#include <NativeEthernet.h>

byte mac[] = { 0x04, 0xE9, 0xE5, 0x12, 0x34, 0x56 };  
IPAddress ip(192,168,8,100);                            

EthernetServer server(23);  // TCP server on port 23

void setup() {
  Serial.begin(115200);
  
  while (!Serial) ;  // Wait for serial monitor
  Serial.println("daonfalkfnd");
  delay(1000);  
  Ethernet.begin(mac, ip);
  delay(1000);  
  server.begin();


  Serial.print("Link Status: ");
  Serial.println(Ethernet.linkStatus() == LinkON ? "Connected" : "Disconnected");

  Serial.print("Local IP: ");
  Serial.println(Ethernet.localIP());  // check if 0.0.0.0 or valid IP


  Serial.print("Server IP address: ");
  Serial.println(Ethernet.localIP());
  
}

void loop() {
  EthernetClient client = server.available();
  static unsigned long t = 0;
  if (millis() - t > 1000) {
    Serial.print("Link: ");
    Serial.println(Ethernet.linkStatus() == LinkON ? "Connected" : "Disconnected");
    t = millis();
  }

  if (client) {
    Serial.println("Client connected");
    client.println("Hello from Teensy 4.1!");
    
    while (client.connected()) {
      if (client.available()) {
        client.write("detected");
        char c = client.read();
        Serial.write(c);
        client.write(c);  // Echo back
      }
    }

    Serial.println("Client disconnected");
    client.stop();
  }

  delay(50);
}
