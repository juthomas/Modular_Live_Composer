#ifndef MIDI_STRUCTS
# define MIDI_STRUCTS
# include <stdint.h>


typedef struct		s_sensors
{	
	uint32_t			date;
	uint32_t			time;
	int16_t				photodiode_1;
	int16_t				photodiode_2;
	int16_t				photodiode_3;
	int16_t				photodiode_4;
	int16_t				photodiode_5;
	int16_t				photodiode_6;
	int16_t				temperature_1;
	int16_t				temperature_2;
	int16_t				temperature_3;
	int16_t				temperature_4;
	int16_t				temperature_5;
	int16_t				temperature_6;
	int16_t				temperature_7;
	int16_t				temperature_8;
	int16_t				temperature_9;
	int16_t				temperature_10;
	int8_t				first_sample;
	float				spectro_current;
	float				organ_current;
	float				vin_current;
	float				q7_current;
	float				t5v_current;
	float				t3_3v_current;
	float				motor_current;
	int16_t				carousel_state;
	int16_t				lid_state;
	int32_t				spectrum;
	int16_t				organ_1;
	int16_t				organ_2;
	int16_t				organ_3;
	int16_t				organ_4;
	int16_t				organ_5;
	int16_t				organ_6;
	struct s_sensors	*next;
}					t_sensors;

typedef struct		s_music_data
{
	uint32_t		quarter_value;// valeur d'une noire pour les metadatas
	// FILE			*midi_file;
	// FILE			*midi_file_redundancy;
	uint32_t		midi_mark;
	uint32_t		midi_mark_redundancy;
	uint32_t		partition_duration;
	uint32_t		measure_value;//
	uint32_t		measures_writed;//
	uint32_t		data_time;
	uint32_t		delta_time; //TODO : replace all measures writed by this
	uint32_t		current_quarter_value;
	uint32_t		quarter_value_goal;
	uint32_t		quarter_value_step;

	float			quarter_value_step_updating;

	uint32_t		entry_data_time;
	// struct timeval	last_measure;//
	// struct timeval	entry_time;//
	
}					t_music_data;

#endif