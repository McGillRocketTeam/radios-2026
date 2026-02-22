#include "ConsoleRouter.h"

#include "BandSelect.h"
#include "Config.h"
#include "MqttTopics.h"
#include "PinLayout.h"

#if TEENSY == 41
#include <ArduinoJson.h>
#include <EthernetConfig.h>
#include <PubSubClient.h>

#include <QNEthernet.h>
using namespace qindesign::network;

static EthernetClient ethClient;
static PubSubClient mqttClient(ethClient);
static ConsoleRouter *s_router = nullptr;

// === Static global function forward decleration ===

static bool ethernetUp();
static bool mqttUp();

volatile bool ethernetReconnectNeeded = false;

static constexpr size_t MAX_MQTT_CMD = 256;
volatile bool mqttCmdReady = false;
char mqttCmdBuf[MAX_MQTT_CMD];

namespace
{
    static void onMqttMessage(char *topic, uint8_t *payload, unsigned int length)
    {
        // Trampoline must exist
        ConsoleRouter *r = s_router;
        if (!r)
            return;

        if (mqttCmdReady)
            return;

        size_t n = (length < (MAX_MQTT_CMD - 1)) ? length : (MAX_MQTT_CMD - 1);
        memcpy(mqttCmdBuf, payload, n);
        mqttCmdBuf[n] = '\0';

        mqttCmdReady = true;
    }
}

// === Setup ===

ConsoleRouter::ConsoleRouter() {}

void ConsoleRouter::begin(MqttTopic::Role role, CommandParser &parser)
{
    Serial.begin(GS_SERIAL_BAUD_RATE);
    // This is a local anonymous variable that helps the ethernet CB access this object
    s_router = this;
    commandParser = &parser;

    if (!ENABLE_ETHERNET_CONNECTION)
        return;

    setTopics(role);
    delay(100);

    Ethernet.setHostname(MqttTopic::topic(role, _band, MqttTopic::TopicKind::NAME));

    // Initialise the ethernet stack only needs to be once as fine we handle reconnecting
    _ethernetStackInitialised = Ethernet.begin();
    ethernetInit();
    // This ISR will work with the reconnect function
    // to reconnect the ethernet connection if it fails
    ethernetTimer.begin(ethernetCheckISR, ETHERNET_RECONNECT_INTERVAL);

    mqttClient.setCallback(onMqttMessage);
}

bool ConsoleRouter::isReady()
{
    return s_router != nullptr;
}

void ConsoleRouter::setTopics(MqttTopic::Role role)
{
    if (BandSelect::is903())
    {
        _band = MqttTopic::Band::B;
        _role = role;
    }
    else
    {
        _band = MqttTopic::Band::A;
        _role = role;
    }

    telemetryTopic = MqttTopic::topic(role, _band, MqttTopic::TopicKind::TELEMETRY);
    metadataTopic = MqttTopic::topic(role, _band, MqttTopic::TopicKind::METADATA);
    ackTopic = MqttTopic::topic(role, _band, MqttTopic::TopicKind::ACKS);
    commandTopic = MqttTopic::topic(role, _band, MqttTopic::TopicKind::COMMANDS);
    debugTopic = MqttTopic::topic(role, _band, MqttTopic::TopicKind::DEBUG);
}
void ConsoleRouter::ethernetInit()
{
    Serial.println("Ethernet init (Teensy 4.1) via QNEthernet...");
    Serial.println("Starting Ethernet with DHCP...");

    if (!_ethernetStackInitialised)
    {
        Serial.println("MCU ethernet harware error retrying stack init");
        _ethernetStackInitialised = Ethernet.begin();
        return;
    }

    uint32_t t0 = millis();
    while (Ethernet.linkStatus() != LinkON)
    {
        if (millis() - t0 > (uint32_t)INITIAL_PHY_LINK_TIMEOUT)
        {
            Serial.println("Ethernet link DOWN (timeout).");
            return;
        }
        Ethernet.loop();
        delay(10);
    }
    Serial.println("Ethernet link UP.");

    if (!Ethernet.waitForLocalIP(DHCP_IP_ASSIGN_TIMEOUT))
    {
        Serial.println("DHCP timed out — no IP assigned.");
        return;
    }

    Serial.print("IP: ");
    Serial.println(Ethernet.localIP());

    mqttReconnect();
}

void ConsoleRouter::handleConsoleReconnect()
{
    if (!ENABLE_ETHERNET_CONNECTION)
        return;
    Ethernet.loop();

    if (!ethernetReconnectNeeded)
        return;

    ethernetReconnectNeeded = false;

    // If the cable is not plugged in do not attempt the blocking dhcp timeout
    if (Ethernet.linkStatus() != LinkON)
    {
        Serial.println("Ethernet link still DOWN: waiting for cable");
        return;
    }

    if (!ethernetUp())
    {
        Serial.println("Ethernet down. Re-initializing...");
        ethernetInit();
    }
    else if (ethernetUp() && !mqttUp())
    {
        Serial.println("MQTT disconnected. Reconnecting...");
        mqttReconnect();
    }
    else
    {
        // We have auto recovered from prev config so just send status
        sendStatus();
    }
}

void ConsoleRouter::mqttLoop()
{
    if (!ENABLE_ETHERNET_CONNECTION)
        return;
    Ethernet.loop();
    if (!ethernetUp())
        return;

    if (!mqttUp())
    {
        return;
    }
    mqttClient.loop();

    if (mqttCmdReady)
    {
        mqttCmdReady = false;

        if (commandParser)
        {
            String s(mqttCmdBuf);
            commandParser->enqueueCommand(s);

            // Confirm the receipt of the command
            sendCmdAckRx(s);
        }
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
        if (mqttUp())
        {
            Serial.println("send status call");
            Serial.println("mqtt up");
            JsonDocument doc;

            doc["status"] = "OK";
            if (BandSelect::is435())
            {
                doc["frequency"] = FREQUENCY_435_STR;
            }
            else
            {
                doc["frequency"] = FREQUENCY_903_STR;
            }
            doc["long_status"] = "Online and ready";

            char macStr[18];
            uint8_t macRaw[6];
            getMac(macRaw);

            snprintf(macStr, sizeof(macStr),
                     "%02X:%02X:%02X:%02X:%02X:%02X",
                     macRaw[0], macRaw[1], macRaw[2],
                     macRaw[3], macRaw[4], macRaw[5]);

            doc["mac"] = macStr;

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

void ConsoleRouter::sendCmdAckRx(const String &s)
{
    uint8_t ackId = 0;
    const char *status = "RX_NOK";

    int commaIdx = s.indexOf(',');
    if (commaIdx <= 0 || commaIdx >= (int)s.length() - 1)
    {
        Serial.println("missing comma from cmd");
        publishAck(status, ackId);
        return;
    }

    String idStr = s.substring(0, commaIdx);
    idStr.trim();
    if (idStr.length() == 0)
    {
        Serial.println("missing id from cmd");
        publishAck(status, ackId);
        return;
    }

    bool looksNumeric = true;
    for (int i = 0; i < (int)idStr.length(); i++)
    {
        char c = idStr[i];
        if (i == 0 && (c == '+' || c == '-'))
            continue;
        if (c < '0' || c > '9')
        {
            looksNumeric = false;
            break;
        }
    }
    if (!looksNumeric)
    {
        Serial.println("cmd id not numeric");
        publishAck(status, ackId);
        return;
    }

    long idLong = idStr.toInt();
    if (idLong < 0 || idLong > 255)
    {
        Serial.println("cmd id out of range");
        publishAck(status, ackId);
        return;
    }
    ackId = (uint8_t)idLong;

    String cmd_string = s.substring(commaIdx + 1);
    cmd_string.trim();
    if (cmd_string.length() == 0)
    {
        Serial.println("cmd has no string part");
        publishAck(status, ackId);
        return;
    }

    status = "RX_OK";
    if (!publishAck(status, ackId))
    {
        Serial.println("publish ack failed");
        return;
    }
}

void ConsoleRouter::sendCmdAckTx(int ack_id)
{
    if (ack_id < 0 || ack_id > 255)
    {
        Serial.println("tx cmd id out of range");
        publishAck("TX_NOK", 0);
        return;
    }

    if (!publishAck("TX_OK", (uint8_t)ack_id))
    {
        Serial.println("publish tx ack failed");
        return;
    }
}

void ConsoleRouter::getMac(uint8_t mac[6])
{
    if (!mac)
        return;
    Ethernet.macAddress(mac);
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

// === Generally accesable helper functions ===

// Check if the Wire is connected AND Ip is assigned
static bool ethernetUp()
{
    if (Ethernet.linkStatus() != LinkON)
        return false;
    IPAddress ip = Ethernet.localIP();
    return !(ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0);
}
static bool mqttUp()
{
    return mqttClient.connected();
}

// === Internal helper functions ===

// Make sure status does not have spaces!
bool ConsoleRouter::publishAck(const char *status, uint8_t cmdId)
{
    if (!ethernetUp() || !mqttUp())
        return false;

    uint8_t jsonBuffer[128];

    // status is a string -> must be JSON-escaped if it can contain quotes/newlines/backslashes.
    int n = snprintf((char *)jsonBuffer, sizeof(jsonBuffer),
                     "{\"cmd_id\":%u,\"status\":\"%s\"}",
                     (unsigned)cmdId, status);

    if (n <= 0 || (size_t)n >= sizeof(jsonBuffer))
        return false;

    return mqttClient.publish(ackTopic, jsonBuffer, (size_t)n, false);
}

bool ConsoleRouter::mqttReconnect()
{
    if (!ethernetUp())
        return false;

    const char *clientId = MqttTopic::topic(_role, _band, MqttTopic::TopicKind::NAME);
    const char *willPayload = "";

    IPAddress brokerIp;
    Serial.print("Resolving MQTT broker host: ");
    Serial.println(BROKER_HOST);

    bool ok = Ethernet.hostByName(BROKER_HOST, brokerIp);
    if (!ok || brokerIp == INADDR_NONE)
    {
        Serial.println("DNS lookup failed for broker host");
        return false;
    }

    Serial.print("MQTT broker resolved to ");
    Serial.println(brokerIp);

    mqttClient.setServer(brokerIp, SERVER_PORT);

    if (mqttClient.connect(clientId, "", "", metadataTopic, 0, true, willPayload))
    {
        Serial.print("MQTT connected to ");
        Serial.println(brokerIp);
        sendStatus();
        mqttClient.subscribe(commandTopic, 1);
        return true;
    }

    Serial.print("MQTT connect failed to ");
    Serial.println(brokerIp);
    return false;
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
#endif