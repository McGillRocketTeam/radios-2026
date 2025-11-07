// Uncomment the following code to test for the correct sizes of packets (it should match the AV25 Interfacing Requirements doc)

#include <iostream>
#include <cassert>
#include <string>
#include "BooleanDataPacket.h"
#include "PayloadDataPacket.h"
#include "FlightComputerDataPacket.h"
#include "RadioDataPacket.h"
#include "TelemetryPacket.h"

#define correctBooleanSize 4
#define correctPayloadSize 22
#define correctFlightComputerSize 37
#define correctVariable1Size 5
#define correctVariable2Size 4
#define correctVariable3Size 6
#define correctVariable4Size 3
#define correctRadioSize 8

int generalSize = correctBooleanSize + correctPayloadSize + correctFlightComputerSize + correctRadioSize;
int correctTelemetry1Size = generalSize + correctVariable1Size;
int correctTelemetry2Size = generalSize + correctVariable2Size;
int correctTelemetry3Size = generalSize + correctVariable3Size;
int correctTelemetry4Size = generalSize + correctVariable4Size;

int main(){
  booleans_packet booleans;
  std::cout << "Boolean data takes up " + std::to_string(sizeof(booleans)) + " bytes." << std::endl;
  assert(sizeof(booleans) == correctBooleanSize);

  payload_packet payload;
  std::cout << "Payload data takes up " + std::to_string(sizeof(payload)) + " bytes." << std::endl;
  assert(sizeof(payload) == correctPayloadSize);

  flight_computer_packet flight_computer;
  std::cout << "Flight computer data takes up " + std::to_string(sizeof(flight_computer)) + " bytes." << std::endl;
  assert(sizeof(flight_computer) == correctFlightComputerSize);

  variable_data_1 variable_1;
  std::cout << "Variable data 1 takes up " + std::to_string(sizeof(variable_1)) + " bytes." << std::endl;
  assert(sizeof(variable_1) == correctVariable1Size);

  variable_data_2 variable_2;
  std::cout << "Variable data 2 takes up " + std::to_string(sizeof(variable_2)) + " bytes." << std::endl;
  assert(sizeof(variable_2) == correctVariable2Size);

  variable_data_3 variable_3;
  std::cout << "Variable data 3 takes up " + std::to_string(sizeof(variable_3)) + " bytes." << std::endl;
  assert(sizeof(variable_3) == correctVariable3Size);

  variable_data_4 variable_4;
  std::cout << "Variable data 4 takes up " + std::to_string(sizeof(variable_4)) + " bytes." << std::endl;
  assert(sizeof(variable_4) == correctVariable4Size);

  radio_packet radio;
  std::cout << "Radio data takes up " + std::to_string(sizeof(radio)) + " bytes." << std::endl;
  assert(sizeof(radio) == correctRadioSize);

  telemetry_packet_1 telemetry_1;
  std::cout << "Telemetry data 1 takes up " + std::to_string(sizeof(telemetry_1)) + " bytes." << std::endl;
  assert(sizeof(telemetry_1) == correctTelemetry1Size);

  telemetry_packet_2 telemetry_2;
  std::cout << "Telemetry data 2 takes up " + std::to_string(sizeof(telemetry_2)) + " bytes." << std::endl;
  assert(sizeof(telemetry_2) == correctTelemetry2Size);

  telemetry_packet_3 telemetry_3;
  std::cout << "Telemetry data 3 takes up " + std::to_string(sizeof(telemetry_3)) + " bytes." << std::endl;
  assert(sizeof(telemetry_3) == correctTelemetry3Size);

  telemetry_packet_4 telemetry_4;
  std::cout << "Telemetry data 4 takes up " + std::to_string(sizeof(telemetry_4)) + " bytes." << std::endl;
  assert(sizeof(telemetry_4) == correctTelemetry4Size);
}