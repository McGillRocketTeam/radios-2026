#include "ConsoleRouter.h"
#include <NativeEthernet.h>
#include <EthernetConfig.h>
#include <Config.h>
#include <PubSubClient.h>


static EthernetClient ethClient;
static PubSubClient   mqttClient(ethClient);
static const char*    kTopic = "radio";

static bool ethernetUp();
static bool mqttUp();
static bool mqttReconnect();

// === GLOBALS ===
volatile bool ethernetReconnectNeeded = false;

ConsoleRouter::ConsoleRouter() = default;

void ConsoleRouter::begin()
{
    Serial.begin(GS_SERIAL_BAUD_RATE);
    delay(100);

    ethernetInit();
    ethernetTimer.begin(ethernetCheckISR, 10 * 1000 * 1000);
}

void ConsoleRouter::ethernetInit()
{
    if (MAC == nullptr) return;

    Serial.println("Ethernet init (Teensy 4.1)...");
    Ethernet.begin(MAC, STATIC_IP);

    // Shorter TCP connect timeout for broker reachability.
    ethClient.setConnectionTimeout(2000);

    delay(1000);

    if (Ethernet.linkStatus() != LinkON) {
        Serial.println("Ethernet link DOWN.");
        return;
    }

    Serial.println("Ethernet link UP.");
    Serial.print("Server: ");
    Serial.println(SERVER_IP);

    mqttClient.setServer(SERVER_IP, SERVER_PORT);

    // Connect immediately to ensure first publish won’t fail
    mqttReconnect();
}

void ConsoleRouter::handleConsoleReconnect()
{
    if (ethernetReconnectNeeded) {
        ethernetReconnectNeeded = false;

        if (!ethernetUp()) {
            Serial.println("Ethernet down. Re-initializing...");
            ethernetInit();
        }

        if (ethernetUp() && !mqttUp()) {
            Serial.println("MQTT disconnected. Reconnecting...");
            mqttReconnect();
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
    if (Serial.available() > 0) return Serial.read();
    return -1;
}

int ConsoleRouter::peek()
{
    if (Serial.available() > 0) return Serial.peek();
    return -1;
}

size_t ConsoleRouter::write(uint8_t c)
{
    // Publish single byte to MQTT
    if (ethernetUp() && mqttUp()) {
        mqttClient.publish(kTopic, &c, 1, false);
    }

    return Serial.write(c);
}

size_t ConsoleRouter::write(const uint8_t *buffer, size_t size)
{
    // Always mirror to USB serial
    Serial.write(buffer, size);

    if (ethernetUp()) {
        if (!mqttUp()) mqttReconnect();

        if (mqttUp()) {
            bool ok = mqttClient.publish(kTopic, buffer, (unsigned int)size, false);
            if (!ok) {
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

static bool mqttReconnect()
{
    if (!ethernetUp()) return false;

    const char* clientId = "teensy41-console";
    if (mqttClient.connect(clientId)) {
        Serial.println("MQTT connected.");
        return true;
    } else {
        Serial.println("MQTT connect failed.");
        return false;
    }
}

void ConsoleRouter::mqttLoop()
{
    if (ethernetUp()) {
        if (!mqttUp()) mqttReconnect();
        mqttClient.loop();
    }
}

// ====== Printing helpers ======
void ConsoleRouter::_publishBytes(const uint8_t* data, size_t len)
{
    // Mirror to MQTT if connected; silently ignore if not.
    if (ethernetUp() && mqttUp() && data && len) {
        mqttClient.publish(kTopic, data, (unsigned int)len, false);
    }
}

void ConsoleRouter::_printString(const String& s, bool newline)
{
    if (!newline) {
        Serial.print(s);
        _publishBytes(reinterpret_cast<const uint8_t*>(s.c_str()), s.length());
    } else {
        Serial.println(s);
        _publishBytes(reinterpret_cast<const uint8_t*>(s.c_str()), s.length());
        const char nl = '\n';
        _publishBytes(reinterpret_cast<const uint8_t*>(&nl), 1);
    }
}

// --- char* ---
void ConsoleRouter::print(const char* s) {
    if (!s) return;
    Serial.print(s);
    _publishBytes(reinterpret_cast<const uint8_t*>(s), strlen(s));
}
void ConsoleRouter::println(const char* s) {
    if (!s) { println(); return; }
    Serial.println(s);
    _publishBytes(reinterpret_cast<const uint8_t*>(s), strlen(s));
    const char nl = '\n';
    _publishBytes(reinterpret_cast<const uint8_t*>(&nl), 1);
}

// --- String ---
void ConsoleRouter::print(const String& s)   { _printString(s, false); }
void ConsoleRouter::println(const String& s) { _printString(s, true ); }

// --- single char ---
void ConsoleRouter::print(char c) {
    Serial.print(c);
    _publishBytes(reinterpret_cast<const uint8_t*>(&c), 1);
}
void ConsoleRouter::println() {
    Serial.println();
    const char nl = '\n';
    _publishBytes(reinterpret_cast<const uint8_t*>(&nl), 1);
}

void ConsoleRouter::print(const __FlashStringHelper* fs) {
    if (!fs) return;
    // Measure and copy from flash to RAM
    const char* p = reinterpret_cast<const char*>(fs);
    size_t n = strlen_P(p);
    Serial.print(fs);
    if (n == 0) return;
    // Small stack buffer for typical short literals; fallback to heap if large
    if (n < 128) {
        char buf[128];
        strncpy_P(buf, p, n);
        _publishBytes(reinterpret_cast<const uint8_t*>(buf), n);
    } else {
        char* buf = (char*)malloc(n + 1);
        if (!buf) return;
        strncpy_P(buf, p, n);
        _publishBytes(reinterpret_cast<const uint8_t*>(buf), n);
        free(buf);
    }
}
void ConsoleRouter::println(const __FlashStringHelper* fs) {
    print(fs);
    println();
}
