#include <Arduino.h>
#include <RadioLib.h>
#include <TelemetryPacket.h>
#include <Config.h>
#include <NativeEthernet.h>
#include <PubSubClient.h>
/*
 * Connections to Teensy 4.0
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
 *
 */

SX1262 radio = new Module(cs, irq, rst, gpio);
volatile bool receivedPacket = false;

// Teensy static IP
IPAddress ip(192, 168, 8, 50);
IPAddress gateway(192, 168, 8, 1);
IPAddress subnet(255, 255, 255, 0);

// Laptop running Mosquitto
IPAddress mqttServer(192, 168, 8, 134);

// Ethernet MAC
byte mac[] = {0x04, 0xE9, 0xE5, 0x12, 0x34, 0x56};

// Ethernet & MQTT clients
EthernetClient ethClient;
PubSubClient client(ethClient);

// Function called when data is received from the teensy due to subscription
void callback(char *topic, byte *payload, unsigned int length)
{
    String message;
    for (int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }
    if (message == "ping")
    {
        Serial.print("Current baud rate: ");
        Serial.println(Serial.baud());
    }
}

void reconnect()
{
    // Loop until reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("TeensyClient"))
        {
            Serial.println("connected");
            // Subscribe to a test topic
            client.subscribe("test/topic");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void isrRX()
{
    Serial.println("packet received done");
    receivedPacket = true;
}

void setup()
{
    // write your initialization code here
    Serial.begin(9600);
    Serial.println("Right before radio initialize");
    int state = radio.begin(FREQUENCY_USED, BANDWIDTH_USED, SPREADING_FACTOR_USED, CODING_RATE_USED, SYNC_WORD, POWER_OUTPUT, PREAMBLE_LENGTH, TCXO_VOLTAGE, USE_ONLY_LDO);
    Serial.print("state is: ");
    Serial.println(state);
    Serial.println("Right after radio initialize");
    radio.setDio1Action(isrRX);
    radio.startReceive();
    Serial.println("start receiving now...");
}
void loop()
{
    delay(1000);
    Serial.println("Looping");
    if (receivedPacket)
    {
        size_t length = radio.getPacketLength();
        Serial.println("Received a packet...");

        String msg;

        int status = radio.readData(msg);
        if (status != RADIOLIB_ERR_NONE)
        {
            Serial.print("readData failed, status: ");
            Serial.println(status);
            return;
        }
        else
        {
            Serial.print("Received message: ");
            Serial.println(msg);
            if (msg == "ping")
            {
            }
        }

        receivedPacket = false; // Reset after processing
    }
}