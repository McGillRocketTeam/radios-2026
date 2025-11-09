#ifndef BOOLEANDATA_H
#define BOOLEANDATA_H

#include <cstdint>

typedef struct __attribute__((__packed__)) booleans_data
    {
	    bool FC_RPO : 1;
	    bool drogueEmatch_armed_SW : 1;
	    bool drogueEmatch_armed_HW : 1;
	    bool drogueEmatch_energized_SW : 1;
	    bool drogueEmatch_energizedGate_HW : 1;
	    bool drogueEmatch_energizedCurrent_HW : 1;
	    bool drogueEmatch_continuity_HW : 1;
	    bool drogueValve_armed_SW : 1;
	    bool drogueValve_armed_HW : 1;
	    bool drogueValve_energizedGate_HW : 1;
	    bool drogueValve_energizedCurrent_HW : 1;
	    bool drogueValve_energized_SW : 1;
	    bool drogueValve_continuity_HW : 1;
	    bool main_armed_SW : 1;
	    bool main_armed_HW : 1;
	    bool main_energized_SW : 1;
	    bool main_energizedGate_HW : 1;
	    bool main_energizedCurrent_HW : 1;
	    bool main_continuity_HW : 1;
	    bool nitroFill_armed_SW : 1;
	    bool nitroFill_armed_HW : 1;
	    bool nitroFill_energized_SW : 1;
	    bool nitroFill_energizedGate_HW : 1;
	    bool nitroFill_energizedCurrent_HW : 1;
	    bool nitroFill_continuity_HW : 1;
	    bool prop_energized_elec : 1;
	    bool mov_armed_SW : 1;
	    bool mov_armed_HW : 1;
	    bool mov_energized_SW : 1;
	    bool mov_energizedGate_HW : 1;
	    bool mov_energizedCurrent_HW : 1;
	    bool mov_continuity_HW : 1;
	    bool fdovNC_armed_SW : 1;
	    bool fdovNC_armed_HW : 1;
	    bool fdovNC_energized_SW : 1;
	    bool fdovNC_energizedGate_HW : 1;
	    bool fdovNC_energizedCurrent_HW : 1;
	    bool fdovNC_continuity_HW : 1;
	    bool fdovNO_armed_SW : 1;
	    bool fdovNO_armed_HW : 1;
	    bool fdovNO_energized_SW : 1;
	    bool fdovNO_energizedGate_HW : 1;
	    bool fdovNO_energizedCurrent_HW : 1;
	    bool fdovNO_continuity_HW : 1;
	    bool vent_armed_SW : 1;
	    bool vent_armed_HW : 1;
	    bool vent_energized_SW : 1;
	    bool vent_energizedGate_HW : 1;
	    bool vent_energizedCurrent_HW : 1;
	    bool vent_continuity_HW : 1;
	    bool pressurant_armed_SW : 1;
	    bool pressurant_armed_HW : 1;
	    bool pressurant_energized_SW : 1;
	    bool pressurant_energizedGate_HW : 1;
	    bool pressurant_energizedCurrent_HW : 1;
	    bool pressurant_continuity_HW : 1;
    } booleans_data;

typedef union booleans_packet
    {
    	booleans_data data;
      	uint8_t bytes[sizeof(booleans_data)];
    } booleans_packet;

#endif //BOOLEANDATA_H
