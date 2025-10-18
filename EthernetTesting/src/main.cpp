#include <NativeEthernet.h>
#include <PubSubClient.h>


// Teensy static IP 
IPAddress ip(192, 168, 8, 50);
IPAddress gateway(192, 168, 8, 1);     
IPAddress subnet(255, 255, 255, 0);

// Laptop running Mosquitto
IPAddress mqttServer(192, 168, 8, 107);

// Ethernet MAC
byte mac[] = { 0x04, 0xE9, 0xE5, 0x12, 0x34, 0x56 };

// Ethernet & MQTT clients
EthernetClient ethClient;
PubSubClient client(ethClient);

//Function called when data is received from the teensy due to subscription
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("TeensyClient")) {
      Serial.println("connected");
      // Subscribe to a test topic
      client.subscribe("test/topic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Start Ethernet with static IP
  Ethernet.begin(mac,ip,gateway,gateway,subnet);
  delay(1000);

  Serial.print("Teensy IP: ");
  Serial.println(Ethernet.localIP());

  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publish every 5 seconds
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 5000) {
    lastMsg = millis();
    String msg = "Hello from Teensy at " + String(millis() / 1000) + "s";
    client.publish("test/topic", msg.c_str());
    Serial.println("Published: " + msg);
  }
}
