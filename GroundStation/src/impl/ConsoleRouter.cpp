// #include "ConsoleRouter.h"
// #include <EthernetConfig.h>

// volatile bool ethernetReconnectNeeded = false;
// ConsoleRouter::ConsoleRouter() = default;

// void ConsoleRouter::begin()
// {
//     Serial.begin(GS_SERIAL_BAUD_RATE);
//     // ONLY FOR TESTING NEED TO REMOVE LATER
//     delay(100);
//     #if TEENSY_BOARD_VERSION == 41
//     ethernetInit();
//     ethernetTimer.begin(ethernetCheckISR,10 * 1000 * 1000);
//     #endif
// }

// #if TEENSY_BOARD_VERSION == 41
// void ConsoleRouter::ethernetInit()
// {
//     if (MAC == nullptr){
//         return;
//     }
//     debugPrint("Attempting Ethernet init with .begin (Teensy 4.1)...");
//     Ethernet.begin(MAC, STATIC_IP);
//     // Default timeout before giving up on the link is 1 second 
//     client.setConnectionTimeout(100); 

//     delay(1000);
//     if (Ethernet.linkStatus() == LinkON)
//     {
//         debugPrint("Ethernet connected (Teensy 4.1)");
//         debugPrint(SERVER_IP);
//         if (client.connect(SERVER_IP, SERVER_PORT))
//         {
//             debugPrint("Successful connect to server");
//             client.println("Connected to server");
//         } else{
//             debugPrint("Failed to connect server");
//         }
//     }
//     else
//     {
//         debugPrint("Ethernet connection failed (Teensy 4.1)");
//         return;
//     }
// }
// #endif

// void ConsoleRouter::handleConsoleReconnect(){
//     #if TEENSY_BOARD_VERSION == 41
//     if (ethernetReconnectNeeded) {
//         ethernetReconnectNeeded = false;

//         if (Ethernet.linkStatus() != LinkON || !client.connected()) {
//             debugPrint("Ethernet disconnected. Re-initializing...");
            
//             ethernetInit(); 
//         }
//     }
//     #endif
//     return;
// }

// void ConsoleRouter::ethernetCheckISR() {
//     ethernetReconnectNeeded = true;
// }

// ConsoleRouter &ConsoleRouter::getInstance()
// {
//     static ConsoleRouter instance;
//     return instance;
// }

// int ConsoleRouter::available()
// {
//     int ethAvailable = (checkEthernetConnected()) ? ethernetAvailble() : 0;
//     int serialAvailable = Serial.available();
//     return ethAvailable + serialAvailable;
// }


// int ConsoleRouter::read()
// {
//     if (checkEthernetConnected() && ethernetAvailble() > 0) {
//         #if TEENSY_BOARD_VERSION == 41
//         return client.read();
//         #endif 
//     }

//     if (Serial.available() > 0) {
//         return Serial.read();
//     }

//     return -1; 
// }

// int ConsoleRouter::peek()
// {
//     if (checkEthernetConnected() && ethernetAvailble() > 0) {
//         #if TEENSY_BOARD_VERSION == 41
//         return client.peek();
//         #endif 
//     }

//     if (Serial.available() > 0) {
//         return Serial.peek();
//     }

//     return -1;
// }

// size_t ConsoleRouter::write(uint8_t c)
// {
    
//     if (checkEthernetConnected()) {
//         #if TEENSY_BOARD_VERSION == 41
//         client.write(c);
//         #endif 
//     }
//     return Serial.write(c); 
// }

// size_t ConsoleRouter::write(const uint8_t *buffer, size_t size)
// {
//     Serial.write(buffer, size);
//     if (checkEthernetConnected()) {
//         #if TEENSY_BOARD_VERSION == 41
//         client.write(buffer, size);
//         #endif 
//     }
//     return size;
// }
