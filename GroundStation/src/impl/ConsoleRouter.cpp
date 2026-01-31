#include "ConsoleRouter.h"
#include <NativeEthernet.h>
#include <EthernetConfig.h>
#include <Config.h>
#include <PubSubClient.h>
#include <PinLayout.h>
#include <ArduinoJson.h>

static EthernetClient ethClient;
static PubSubClient mqttClient(ethClient);

// MQTT Topics

// Topics radio-<location>-<a or b for freq>/...
const char *TOPIC_RADIO_PD_A_TELEMETRY = "radio-pad-a/telemetry";
const char *TOPIC_RADIO_PD_A_METADATA = "radio-pad-a/metadata";

const char *TOPIC_RADIO_PD_B_TELEMETRY = "radio-pad-b/telemetry";
const char *TOPIC_RADIO_PD_B_METADATA = "radio-pad-b/metadata";

const char *TOPIC_RADIO_CS_A_TELEMETRY = "radio-controlstation-a/telemetry";
const char *TOPIC_RADIO_CS_A_METADATA = "radio-controlstation-a/metadata";
const char *TOPIC_RADIO_CS_A_COMMANDS = "radio-controlstation-a/commands";

const char *TOPIC_RADIO_CS_A_DEBUG = "radio-controlstation-a/debug";
const char *TOPIC_RADIO_CS_B_TELEMETRY = "radio-controlstation-b/telemetry";

const char *TOPIC_RADIO_CS_B_METADATA = "radio-controlstation-b/metadata";
const char *TOPIC_RADIO_CS_B_DEBUG = "radio-controlstation-b/debug";

// === Static global function forward decleration ===

static bool ethernetUp();
static bool mqttUp();
static bool mqttReconnect(const char *topic);

volatile bool ethernetReconnectNeeded = false;

namespace
{
    static void onMqttMessage(char *topic, uint8_t *payload, unsigned int length)
    {
        Serial.print("[MQTT] ");
        Serial.print(topic);
        Serial.print(" : ");

        for (unsigned int i = 0; i < length; i++)
        {
            Serial.write(payload[i]);
        }
        Serial.println();
    }
}

// === Setup ===

ConsoleRouter::ConsoleRouter() {}

void ConsoleRouter::begin()
{
    Serial.begin(GS_SERIAL_BAUD_RATE);

    if (!ENABLE_ETHERNET_CONNECTION)
        return;

    setTopicsFromPins();
    delay(100);

    ethernetInit();
    // This ISR will work with the reconnect function
    // to reconnect the ethernet connection if it fails
    ethernetTimer.begin(ethernetCheckISR, ETHERNET_RECONNECT_INTERVAL);
    mqttClient.setCallback(onMqttMessage);
}

void ConsoleRouter::setTopicsFromPins()
{
    pinMode(FREQ_PIN, INPUT);
    if (digitalRead(FREQ_PIN) == HIGH)
    {
        // High on freq pin means 915 band so radio B
        metadataTopic = TOPIC_RADIO_CS_B_METADATA;
        telemetryTopic = TOPIC_RADIO_CS_B_TELEMETRY;
        debugTopic = TOPIC_RADIO_CS_B_DEBUG;
    }
    else
    {
        metadataTopic = TOPIC_RADIO_CS_A_METADATA;
        telemetryTopic = TOPIC_RADIO_CS_A_TELEMETRY;
        debugTopic = TOPIC_RADIO_CS_A_DEBUG;
    }
}

void ConsoleRouter::ethernetInit()
{
    if (MAC == nullptr)
        return;

    Serial.println("Ethernet init (Teensy 4.1)...");
    Serial.println("This is a BLOCKING begin");
    Serial.println("if there is no physical link it will halt the code");
    Ethernet.begin(MAC, STATIC_IP);

    ethClient.setConnectionTimeout(TCP_TIMEOUT_SETTING);

    delay(1000);

    if (Ethernet.linkStatus() != LinkON)
    {
        Serial.println("Ethernet link DOWN.");
        return;
    }

    Serial.println("Ethernet link UP.");
    Serial.print("Server: ");
    Serial.println(SERVER_IP);

    mqttClient.setServer(SERVER_IP, SERVER_PORT);
    mqttReconnect(metadataTopic);
}

void ConsoleRouter::handleConsoleReconnect()
{
    if (!ENABLE_ETHERNET_CONNECTION)
        return;

    if (ethernetReconnectNeeded)
    {
        ethernetReconnectNeeded = false;

        if (!ethernetUp())
        {
            Serial.println("Ethernet down. Re-initializing...");
            ethernetInit();
        }

        if (ethernetUp() && !mqttUp())
        {
            Serial.println("MQTT disconnected. Reconnecting...");
            mqttReconnect(metadataTopic);
        }
    }
}

void ConsoleRouter::mqttLoop()
{
    if (!ENABLE_ETHERNET_CONNECTION)
        return;

    if (ethernetUp())
    {
        if (!mqttUp())
        {
            if (mqttReconnect(metadataTopic))
            {
                sendStatus();
            }
        }
        mqttClient.loop();
    }
}

void ConsoleRouter::ethernetCheckISR()
{
    ethernetReconnectNeeded = true;
}

ConsoleRouter &ConsoleRouter::getInstance()
{
    static ConsoleRouter instance;
    return instance;
}

int ConsoleRouter::available()
{
    // Only Serial is readable; MQTT is publish-only
    return Serial.available();
}

int ConsoleRouter::read()
{
    if (Serial.available() > 0)
        return Serial.read();
    return -1;
}

int ConsoleRouter::peek()
{
    if (Serial.available() > 0)
        return Serial.peek();
    return -1;
}

void ConsoleRouter::sendTelemetry(const uint8_t *buffer, size_t size)
{
    if (ethernetUp())
    {
        if (!mqttUp())
            mqttReconnect(metadataTopic);

        if (mqttUp())
        {
            bool ok = mqttClient.publish(telemetryTopic, buffer, (unsigned int)size, false);
            if (!ok)
            {
                Serial.println("MQTT publish failed (packet too large or socket issue).");
            }
        }
    }
}

void ConsoleRouter::sendStatus()
{
    if (ethernetUp())
    {
        Serial.println("send status call");
        if (!mqttUp())
            mqttReconnect(metadataTopic);
        Serial.println("mqtt up");
        if (mqttUp())
        {
            JsonDocument doc;

            doc["status"] = "OK";
            doc["frequency"] = "435.00";
            doc["long_status"] = "this is the long status";

            uint8_t jsonBuffer[512];
            size_t jsonSize = serializeJson(doc, jsonBuffer);

            bool ok = mqttClient.publish(metadataTopic, jsonBuffer, jsonSize, true);
            if (!ok)
            {
                Serial.println("MQTT publish failed (packet too large or socket issue).");
            }
        }
    }
}

size_t ConsoleRouter::write(uint8_t c)
{
    // Publish single byte to MQTT
    if (ethernetUp() && mqttUp())
    {
        mqttClient.publish(debugTopic, &c, 1, false);
    }

    return Serial.write(c);
}

size_t ConsoleRouter::write(const uint8_t *buffer, size_t size)
{
    // Always mirror to USB serial
    Serial.write(buffer, size);

    if (ethernetUp())
    {
        if (!mqttUp())
            mqttReconnect(metadataTopic);

        if (mqttUp())
        {
            bool ok = mqttClient.publish(debugTopic, buffer, (unsigned int)size, false);
            if (!ok)
            {
                Serial.println("MQTT publish failed (packet too large or socket issue).");
            }
        }
    }
    return size;
}

// === Internal helper functions ===
static bool ethernetUp()
{
    return Ethernet.linkStatus() == LinkON;
}

static bool mqttUp()
{
    return mqttClient.connected();
}

static bool mqttReconnect(const char *topic)
{
    if (!ethernetUp())
        return false;

    const char *clientId = "teensy41-console";
    // Last will s
    const char *willPayload = "{\"status\":\"FAILED\"}";
    if (mqttClient.connect(clientId, "", "", topic, 0, true, willPayload))
    {
        Serial.println("MQTT connected.");
        ConsoleRouter::getInstance().sendStatus();
        mqttClient.subscribe(TOPIC_RADIO_CS_A_COMMANDS);
        return true;
    }
    else
    {
        Serial.println("MQTT connect failed.");
        return false;
    }
}

void ConsoleRouter::_publishBytes(const uint8_t *data, size_t len)
{
    if (ethernetUp() && mqttUp() && data && len)
    {
        mqttClient.publish(debugTopic, data, (unsigned int)len, false);
    }
}

void ConsoleRouter::_printString(const String &s, bool newline)
{
    if (!newline)
    {
        Serial.print(s);
        _publishBytes(reinterpret_cast<const uint8_t *>(s.c_str()), s.length());
    }
    else
    {
        Serial.println(s);
        _publishBytes(reinterpret_cast<const uint8_t *>(s.c_str()), s.length());
        // const char nl = '\n';
        // _publishBytes(reinterpret_cast<const uint8_t *>(&nl), 1);
    }
}

void ConsoleRouter::print(const char *s)
{
    if (!s)
        return;
    Serial.print(s);
    _publishBytes(reinterpret_cast<const uint8_t *>(s), strlen(s));
}
void ConsoleRouter::println(const char *s)
{
    if (!s)
    {
        println();
        return;
    }
    Serial.println(s);
    _publishBytes(reinterpret_cast<const uint8_t *>(s), strlen(s));
    // const char nl = '\n';
    // _publishBytes(reinterpret_cast<const uint8_t *>(&nl), 1);
}

// --- String ---
void ConsoleRouter::print(const String &s) { _printString(s, false); }
void ConsoleRouter::println(const String &s) { _printString(s, true); }

// --- single char ---
void ConsoleRouter::print(char c)
{
    Serial.print(c);
    _publishBytes(reinterpret_cast<const uint8_t *>(&c), 1);
}
void ConsoleRouter::println()
{
    Serial.println();
    // const char nl = '\n';
    // _publishBytes(reinterpret_cast<const uint8_t *>(&nl), 1);
}

void ConsoleRouter::print(const __FlashStringHelper *fs)
{
    if (!fs)
        return;
    // Measure and copy from flash to RAM
    const char *p = reinterpret_cast<const char *>(fs);
    size_t n = strlen_P(p);
    Serial.print(fs);
    if (n == 0)
        return;
    // Small stack buffer for typical short literals; fallback to heap if large
    if (n < 128)
    {
        char buf[128];
        strncpy_P(buf, p, n);
        _publishBytes(reinterpret_cast<const uint8_t *>(buf), n);
    }
    else
    {
        char *buf = (char *)malloc(n + 1);
        if (!buf)
            return;
        strncpy_P(buf, p, n);
        _publishBytes(reinterpret_cast<const uint8_t *>(buf), n);
        free(buf);
    }
}
void ConsoleRouter::println(const __FlashStringHelper *fs)
{
    print(fs);
    println();
}
