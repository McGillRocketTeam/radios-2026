#ifndef BOOLEANDATA_H
#define BOOLEANDATA_H

#include <cstdint>

typedef struct __attribute__((__packed__)) booleans_data
    {
	    bool V_EXT_SNS : 1;
	    bool drogue_armed_SW : 1;
	    bool drogue_armed_HW : 1;
	    bool drogue_energized_SW : 1;
	    bool drogue_energizedGate_HW : 1;
	    bool drogue_energizedCurrent_HW : 1;
	    bool drogue_continuity_HW : 1;
	    bool main_armed_SW : 1;
	    bool main_armed_HW : 1;
	    bool main_energized_SW : 1;
	    bool main_energizedGate_HW : 1;
	    bool main_energizedCurrent_HW : 1;
	    bool main_continuity_HW : 1;
        bool prop_energized_elec : 1;
	    bool ufd_armed_SW : 1;
	    bool ufd_armed_HW : 1;
	    bool ufd_energized_SW : 1;
	    bool ufd_energizedGate_HW : 1;
	    bool ufd_energizedCurrent_HW : 1;
	    bool ufd_continuity_HW : 1;
	    bool run_armed_SW : 1;
	    bool run_armed_HW : 1;
	    bool run_energized_SW : 1;
	    bool run_energizedGate_HW : 1;
	    bool run_energizedCurrent_HW : 1;
	    bool run_continuity_HW : 1;
	    bool vent_armed_SW : 1;
	    bool vent_armed_HW : 1;
	    bool vent_energized_SW : 1;
	    bool vent_energizedGate_HW : 1;
	    bool vent_energizedCurrent_HW : 1;
	    bool vent_continuity_HW : 1;
    } booleans_data;

typedef union booleans_packet
    {
    	booleans_data data;
      	uint8_t bytes[sizeof(booleans_data)];
    } booleans_packet;

#endif //BOOLEANDATA_H
