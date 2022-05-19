#ifndef MAP_INPUT_H
#define MAP_INPUT_H
#include <stddef.h>

typedef struct s_sensors
{
	uint32_t date;
	uint32_t time;
	uint16_t photodiode_1;	  // 0 - 4095
	uint16_t photodiode_2;	  // 0 - 4095
	uint16_t photodiode_3;	  // 0 - 4095
	uint16_t photodiode_4;	  // 0 - 4095
	uint16_t photodiode_5;	  // 0 - 4095
	uint16_t photodiode_6;	  // 0 - 4095
	uint16_t temperature_1;	  // 0 - 4095
	uint16_t temperature_2;	  // 0 - 4095
	uint16_t temperature_3;	  // 0 - 4095
	uint16_t temperature_4;	  // 0 - 4095
	uint16_t temperature_5;	  // 0 - 4095
	uint16_t temperature_6;	  // 0 - 4095
	uint16_t temperature_7;	  // 0 - 4095
	uint16_t temperature_8;	  // 0 - 4095
	uint16_t temperature_9;	  // 0 - 4095
	uint16_t temperature_10;  // 0 - 4095
	int8_t microphone;		  // 0 - 1
	uint16_t spectro_current; // 0 - 65535
	uint8_t organ_current;	  // 0 - 255
	uint16_t vin_current;	  // 0 - 65535//
	uint8_t q7_current;		  // 0 - 255
	uint8_t t5v_current;	  // 0 - 255
	uint8_t t3_3v_current;	  // 0 - 255
	uint16_t motor_current;	  // 0 - 65535
	uint8_t carousel_state;	  // 0 - 119
	uint8_t lid_state;		  // 0 - 53
	uint16_t organ_1;		  // 0 - 1023
	uint16_t organ_2;		  // 0 - 1023
	uint16_t organ_3;		  // 0 - 1023
	uint16_t organ_4;		  // 0 - 1023
	uint16_t organ_5;		  // 0 - 1023
	uint16_t organ_6;		  // 0 - 1023
	uint32_t timestamp;		  // 0 - oo
	struct s_sensors *next;
} t_sensors;

enum e_data_type
{
	INTEGER = 0,
	FLOATING,
	BINARY,
	CHAR
};

typedef struct s_map_input
{
	long offset;
	uint8_t data_type;
	uint8_t input_nu;
	uint8_t active;

	uint16_t int_max;
	uint8_t char_max;
	char *name;
	// uint32_t int_data;
	// float float_data;
	// uint32_t int_delta;
	// float float_delta;
	// uint8_t binary;
	// uint8_t char_data;
	// uint8_t char_delta;
} t_map_input;

static const t_map_input g_map_input[] = {
	// 7 note scales
	(t_map_input){.active = 1, .input_nu = 0, .offset = offsetof(t_sensors, photodiode_1), .name = "photo1", .data_type = INTEGER, .int_max = 4096}, // x2
	(t_map_input){.active = 1, .input_nu = 1, .offset = offsetof(t_sensors, photodiode_2), .name = "photo2", .data_type = INTEGER, .int_max = 4096}, // x2
	(t_map_input){.active = 1, .input_nu = 1, .offset = offsetof(t_sensors, photodiode_3), .name = "photo3", .data_type = INTEGER, .int_max = 4095},
	(t_map_input){.active = 1, .input_nu = 1, .offset = offsetof(t_sensors, photodiode_4), .name = "photo4", .data_type = INTEGER, .int_max = 4095},
	(t_map_input){.active = 1, .input_nu = 1, .offset = offsetof(t_sensors, photodiode_5), .name = "photo5", .data_type = INTEGER, .int_max = 4095},
	(t_map_input){.active = 1, .input_nu = 1, .offset = offsetof(t_sensors, photodiode_6), .name = "photo6", .data_type = INTEGER, .int_max = 4095},
	(t_map_input){.active = 1, .input_nu = 2, .offset = offsetof(t_sensors, temperature_1), .name = "temp1", .data_type = INTEGER, .int_max = 4095}, // x2
	(t_map_input){.active = 1, .input_nu = 3, .offset = offsetof(t_sensors, temperature_2), .name = "temp2", .data_type = INTEGER, .int_max = 4096}, // x2
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, temperature_3), .name = "temp3", .data_type = INTEGER, .int_max = 4095},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, temperature_4), .name = "temp4", .data_type = INTEGER, .int_max = 4095},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, temperature_5), .name = "temp5", .data_type = INTEGER, .int_max = 4095},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, temperature_6), .name = "temp6", .data_type = INTEGER, .int_max = 4095},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, temperature_7), .name = "temp7", .data_type = INTEGER, .int_max = 4095},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, temperature_8), .name = "temp8", .data_type = INTEGER, .int_max = 4095},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, temperature_9), .name = "temp9", .data_type = INTEGER, .int_max = 4095},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, temperature_10), .name = "temp10", .data_type = INTEGER, .int_max = 4095},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, microphone), .name = "micro", .data_type = BINARY},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, spectro_current), .name = "spectro", .data_type = INTEGER, .int_max = 65535},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, organ_current), .name = "organ_cur", .data_type = CHAR, .char_max = 255},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, vin_current), .name = "vin_cur", .data_type = INTEGER, .int_max = 65535},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, q7_current), .name = "q7_cur", .data_type = CHAR, .char_max = 255},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, t5v_current), .name = "t5v_cur", .data_type = CHAR, .char_max = 255},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, t3_3v_current), .name = "t3_cur", .data_type = CHAR, .char_max = 255},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, motor_current), .name = "motor_cur", .data_type = INTEGER, .int_max = 65535},
	(t_map_input){.active = 1, .input_nu = 4, .offset = offsetof(t_sensors, carousel_state), .name = "car_state", .data_type = CHAR, .char_max = 119},
	(t_map_input){.active = 1, .input_nu = 5, .offset = offsetof(t_sensors, lid_state), .name = "lid", .data_type = CHAR, .char_max = 53},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, organ_1), .name = "organ1", .data_type = INTEGER, .int_max = 1023},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, organ_2), .name = "organ2", .data_type = INTEGER, .int_max = 1023},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, organ_3), .name = "organ3", .data_type = INTEGER, .int_max = 1023},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, organ_4), .name = "organ4", .data_type = INTEGER, .int_max = 1023},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, organ_5), .name = "organ5", .data_type = INTEGER, .int_max = 1023},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, organ_6), .name = "organ6", .data_type = INTEGER, .int_max = 1023},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, timestamp), .name = "time", .data_type = INTEGER, .int_max = 65535}

};

#endif