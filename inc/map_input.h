#ifndef MAP_INPUT_H
#define MAP_INPUT_H
#include <stddef.h>

typedef struct s_sensors
{
	uint32_t date;
	uint32_t time;
	int16_t photodiode_1;
	int16_t photodiode_2;
	int16_t photodiode_3;
	int16_t photodiode_4;
	int16_t photodiode_5;
	int16_t photodiode_6;
	int16_t temperature_1;
	int16_t temperature_2;
	int16_t temperature_3;
	int16_t temperature_4;
	int16_t temperature_5;
	int16_t temperature_6;
	int16_t temperature_7;
	int16_t temperature_8;
	int16_t temperature_9;
	int16_t temperature_10;
	int8_t first_sample;
	float spectro_current;
	float organ_current;
	float vin_current;
	float q7_current;
	float t5v_current;
	float t3_3v_current;
	float motor_current;
	int16_t carousel_state;
	int16_t lid_state; //
	int32_t spectrum;  //
	int16_t organ_1;
	int16_t organ_2; //
	int16_t organ_3; ///
	int16_t organ_4; //
	int16_t organ_5; ///
	int16_t organ_6; //
	struct s_sensors *next;
} t_sensors;

enum e_data_type
{
	INTEGER = 0,
	FLOATING,
	BINARY
};

typedef struct s_map_input
{
	long offset;
	uint8_t data_type;
	uint8_t input_nu;
	uint8_t active;
	uint32_t int_data;
	float float_data;
	uint32_t int_delta;
	float float_delta;
	uint8_t binary;
} t_map_input;

static const t_map_input g_map_input[] = {
	// 7 note scales
	(t_map_input){.active = 1, .input_nu = 0, .offset = offsetof(t_sensors, photodiode_1), .data_type = FLOATING, .float_data = 2.5, .float_delta = 2.5},
	(t_map_input){.active = 1, .input_nu = 1, .offset = offsetof(t_sensors, photodiode_2), .data_type = FLOATING, .float_data = 2.5, .float_delta = 2.5},
	(t_map_input){.active = 1, .input_nu = 2, .offset = offsetof(t_sensors, photodiode_3), .data_type = FLOATING, .float_data = 2.5, .float_delta = 2.5},
	(t_map_input){.active = 1, .input_nu = 3, .offset = offsetof(t_sensors, photodiode_4), .data_type = FLOATING, .float_data = 2.5, .float_delta = 2.5},
	(t_map_input){.active = 1, .input_nu = 4, .offset = offsetof(t_sensors, photodiode_5), .data_type = FLOATING, .float_data = 2.5, .float_delta = 2.5},
	(t_map_input){.active = 1, .input_nu = 5, .offset = offsetof(t_sensors, photodiode_6), .data_type = FLOATING, .float_data = 2.5, .float_delta = 2.5},
	(t_map_input){.active = 1, .input_nu = 6, .offset = offsetof(t_sensors, temperature_1), .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	(t_map_input){.active = 1, .input_nu = 6, .offset = offsetof(t_sensors, temperature_2), .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	(t_map_input){.active = 1, .input_nu = 6, .offset = offsetof(t_sensors, temperature_3), .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	(t_map_input){.active = 1, .input_nu = 6, .offset = offsetof(t_sensors, temperature_4), .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	(t_map_input){.active = 1, .input_nu = 6, .offset = offsetof(t_sensors, temperature_5), .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	(t_map_input){.active = 1, .input_nu = 6, .offset = offsetof(t_sensors, temperature_6), .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	(t_map_input){.active = 1, .input_nu = 6, .offset = offsetof(t_sensors, temperature_7), .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	(t_map_input){.active = 1, .input_nu = 6, .offset = offsetof(t_sensors, temperature_8), .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	(t_map_input){.active = 1, .input_nu = 6, .offset = offsetof(t_sensors, temperature_9), .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	(t_map_input){.active = 1, .input_nu = 6, .offset = offsetof(t_sensors, temperature_10), .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	(t_map_input){.active = 1, .input_nu = 7, .offset = offsetof(t_sensors, first_sample), .data_type = BINARY, .binary = 0},
	(t_map_input){.active = 1, .input_nu = 8, .offset = offsetof(t_sensors, spectro_current), .data_type = FLOATING, .float_data = 0.5, .float_delta = 0.5},
	(t_map_input){.active = 1, .input_nu = 9, .offset = offsetof(t_sensors, organ_current), .data_type = FLOATING, .float_data = 0.05, .float_delta = 0.05},
	(t_map_input){.active = 1, .input_nu = 10, .offset = offsetof(t_sensors, vin_current), .data_type = FLOATING, .float_data = 1.25, .float_delta = 0.75},
	(t_map_input){.active = 1, .input_nu = 11, .offset = offsetof(t_sensors, q7_current), .data_type = FLOATING, .float_data = 0.1, .float_delta = 0.1},
	(t_map_input){.active = 1, .input_nu = 12, .offset = offsetof(t_sensors, t5v_current), .data_type = FLOATING, .float_data = 0.5, .float_delta = 0.5},
	(t_map_input){.active = 1, .input_nu = 13, .offset = offsetof(t_sensors, t3_3v_current), .data_type = FLOATING, .float_data = 0.25, .float_delta = 0.25},
	(t_map_input){.active = 1, .input_nu = 14, .offset = offsetof(t_sensors, motor_current), .data_type = FLOATING, .float_data = 0.025, .float_delta = 0.025},
	(t_map_input){.active = 1, .input_nu = 15, .offset = offsetof(t_sensors, carousel_state), .data_type = INTEGER, .int_data = 180, .int_delta = 180},

	// (t_data){.name = "Lid_state", .data_type = BINARY, .binary = 0},

	// (t_data){.name = "Position_360", .data_type = INTEGER, .int_data = 180, .int_delta = 180},
	// (t_data){.name = "Spectrum", .data_type = INTEGER, .int_data = 10000, .int_delta = 10000},
	// (t_data){.name = "Organ", .data_type = FLOATING, .float_data = 2.5, .float_delta = 2.5}

};

#endif