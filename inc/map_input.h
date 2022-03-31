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
	int16_t lid_state;
	int32_t spectrum;
	int16_t organ_1;
	int16_t organ_2;
	int16_t organ_3;
	int16_t organ_4;
	int16_t organ_5;
	int16_t organ_6;
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
	// (t_data){.name = "Photodiode_1", .data_type = FLOATING, .float_data = 2.5, .float_delta = 2.5},
	// (t_data){.name = "Photodiode_2", .data_type = FLOATING, .float_data = 2.5, .float_delta = 2.5},
	// (t_data){.name = "Photodiode_3", .data_type = FLOATING, .float_data = 2.5, .float_delta = 2.5},
	// (t_data){.name = "Photodiode_4", .data_type = FLOATING, .float_data = 2.5, .float_delta = 2.5},
	// (t_data){.name = "Photodiode_5", .data_type = FLOATING, .float_data = 2.5, .float_delta = 2.5},
	// (t_data){.name = "Photodiode_6", .data_type = FLOATING, .float_data = 2.5, .float_delta = 2.5},
	// (t_data){.name = "Temperature_1", .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	// (t_data){.name = "Temperature_2", .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	// (t_data){.name = "Temperature_3", .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	// (t_data){.name = "Temperature_4", .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	// (t_data){.name = "Temperature_5", .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	// (t_data){.name = "Temperature_6", .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	// (t_data){.name = "Temperature_7", .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	// (t_data){.name = "Temperature_8", .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	// (t_data){.name = "Temperature_9", .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	// (t_data){.name = "Temperature_10", .data_type = FLOATING, .float_data = 0, .float_delta = 90},
	// (t_data){.name = "Lid_state", .data_type = BINARY, .binary = 0},
	// (t_data){.name = "First_sample", .data_type = BINARY, .binary = 0},
	// (t_data){.name = "Spectro_current", .data_type = FLOATING, .float_data = 0.5, .float_delta = 0.5},
	// (t_data){.name = "Electro_current", .data_type = FLOATING, .float_data = 1.5, .float_delta = 1.5},
	// (t_data){.name = "Organ_current", .data_type = FLOATING, .float_data = 0.05, .float_delta = 0.05},
	// (t_data){.name = "Vin_current", .data_type = FLOATING, .float_data = 1.25, .float_delta = 0.75},
	// (t_data){.name = "Q7_current", .data_type = FLOATING, .float_data = 0.1, .float_delta = 0.1},
	// (t_data){.name = "5v_current", .data_type = FLOATING, .float_data = 0.5, .float_delta = 0.5},
	// (t_data){.name = "3.3v_current", .data_type = FLOATING, .float_data = 0.25, .float_delta = 0.25},
	// (t_data){.name = "Motor_current", .data_type = FLOATING, .float_data = 0.025, .float_delta = 0.025},
	// (t_data){.name = "Position_360", .data_type = INTEGER, .int_data = 180, .int_delta = 180},
	// (t_data){.name = "Spectrum", .data_type = INTEGER, .int_data = 10000, .int_delta = 10000},
	// (t_data){.name = "Organ", .data_type = FLOATING, .float_data = 2.5, .float_delta = 2.5}

};

#endif