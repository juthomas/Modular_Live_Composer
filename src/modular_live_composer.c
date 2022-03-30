#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spidev_lib.h>
#include <unistd.h>
#include <portmidi.h>
#include <porttime.h>
#include "../inc/midi_notes.h"
#include "../inc/midi_modes.h"
// #include "../inc/midi_structs.h"
#include "../inc/midi_euclidean.h"
#include "../inc/midi.h"

// 							//durée d'une partition 40 000 000us
// static t_music_data music_data = {.partition_duration = 40000000,
// 								//Measure value = quarter value * 4 (4/4) (4 noires par mesure)
// 							   .measure_value = 500000 * 4,
// 							   .measures_writed = 0,
// 							   // valeur d'une noire en us (pour le tempo)
// 							   .quarter_value = 500000 };

static uint8_t playing_notes_length = 24;
static uint8_t playing_notes[24];
static uint8_t playing_notes_duration[24];

/**
 * @brief Map number from [in_min]-[in_max] to [out_min]-[out_max]
 * @param [x] Number to map
 * @param [in_min] Minimum of input number
 * @param [in_max] Maximum of input number
 * @param [out_min] Minimum of output number
 * @param [out_max] Maximum of ouput number
 * @return New number well mapped
 */
int32_t map_number(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
 * @brief Initializing euclidean struct
 * @param [euclidean] Euclidean Circle struct
 * @param [steps_length] Number of steps in euclidean circle
 * @param [octave_size] Range of playable notes
 * @param [chord_list_length] Number of different possible chords in mode
 * @param [mode] Music mode (e_midi_modes)
 * @param [mode_beg_note] Reference note for the mode (e_notes)
 * @param [notes_per_cycle] Number of notes playables in euclidean circle
 * @param [mess_chance] Chance to skip a note/chord in euclidean circle (0-100)%
 * @param [min_chord_size] Minimum length of chord/note played in euclidean step
 * @param [max_chord_size] Maximum length of chord/note played in euclidean step
 * @param [min_velocity] Minimum velocity for chord/note played
 * @param [max_velocity] Maximum velocity for chord/note played
 * @param [min_steps_duration] Minimum duration for chord/note played in euclidean step
 * @param [max_steps_duration] Maximum duration for chord/note played in euclidean step
 */
void init_euclidean_struct(t_euclidean *euclidean, uint8_t steps_length,
						   uint8_t octave_size, uint8_t chord_list_length,
						   uint8_t mode, uint8_t mode_beg_note,
						   uint8_t notes_per_cycle, uint8_t mess_chance,
						   uint8_t min_chord_size, uint8_t max_chord_size,
						   uint8_t min_velocity, uint8_t max_velocity,
						   uint8_t min_steps_duration, uint8_t max_steps_duration)
{
	euclidean->euclidean_steps_length = steps_length;
	euclidean->euclidean_steps = (int16_t *)malloc(sizeof(int16_t) * steps_length);
	euclidean->octaves_size = octave_size;
	euclidean->chords_list_length = chord_list_length;
	euclidean->chords_list = (uint8_t *)malloc(sizeof(uint8_t) * chord_list_length);
	euclidean->mode = mode;
	euclidean->mode_beg_note = mode_beg_note;
	euclidean->notes_per_cycle = notes_per_cycle;
	euclidean->step_gap = steps_length / notes_per_cycle;
	euclidean->mess_chance = mess_chance;
	euclidean->min_chord_size = min_chord_size;
	euclidean->max_chord_size = max_chord_size;
	get_chords_list(euclidean->chords_list, chord_list_length);
	euclidean->min_velocity = min_velocity;
	euclidean->max_velocity = max_velocity;
	euclidean->min_steps_duration = min_steps_duration;
	euclidean->max_steps_duration = max_steps_duration;
	euclidean->current_step = 0;
	euclidean->initialized = 1;
}

/**
 * @brief Getting a new chord from chord list (Randomly)
 *        and ajusting his pitch (Randomly)
 *        this new chords are passing to the "euclidean_steps[]" variable
 * @param [euclidean] Euclidean Circle struct
 */
void get_new_euclidean_chords(t_euclidean *euclidean)
{
	for (uint8_t steps = 0; steps < euclidean->euclidean_steps_length; steps++)
	{
		if (steps % euclidean->step_gap == 0)
		{
			euclidean->euclidean_steps[steps] = get_new_chord_from_list(euclidean->chords_list,
																		euclidean->chords_list_length, steps, euclidean->euclidean_steps);
			euclidean->euclidean_steps[steps] |= (rand() % euclidean->octaves_size) << 8; // add octave property
			printf("New step : %d\n", euclidean->euclidean_steps[steps]);
		}
		else
		{
			euclidean->euclidean_steps[steps] = -1;
		}
	}
}

/**
 * @brief Print the content of euclidean Steps (Variable euclidean_steps[])
 * @param [euclidean] Euclidean Circle struct
 */
void print_euclidean_steps(t_euclidean *euclidean)
{
	for (uint8_t steps = 0; steps < euclidean->euclidean_steps_length; steps++)
	{
		printf("Step value : %d, octave : %d\n", euclidean->euclidean_steps[steps] & 0xFF,
			   (euclidean->euclidean_steps[steps] & 0xFF00) >> 8);
	}
	printf("Chord list : ");
	for (uint8_t chord_list_i = 0; chord_list_i < euclidean->chords_list_length; chord_list_i++)
	{
		printf("%d,", euclidean->chords_list[chord_list_i]);
	}
	printf("\n");
}

/**
 * @brief Function to write an multiple Euclidean midi step
 * @param [music_data] Midi struct
 * @param [euclidean] Struct that contain current euclidean values
 */
void write_euclidean_step(t_music_data *music_data, t_euclidean *euclidean)
{
	// Create chord if the current euclidean step contain note and the mess chance dont mess
	if (euclidean->euclidean_steps[euclidean->current_step] != -1 && rand() % 100 >= euclidean->mess_chance)
	{
		create_chord(music_data, playing_notes_duration, playing_notes, playing_notes_length,
					 euclidean->mode, euclidean->euclidean_steps[euclidean->current_step], euclidean->mode_beg_note,
					 map_number(rand() % 100, 0, 100, euclidean->min_chord_size, euclidean->max_chord_size),		  /*chord size*/
					 map_number(rand() % 100, 0, 100, euclidean->min_velocity, euclidean->max_velocity),			  /*velocity*/
					 map_number(rand() % 100, 0, 100, euclidean->min_steps_duration, euclidean->max_steps_duration)); /*note duration in steps*/
	}
	// Update the current euclidean step
	euclidean->current_step = (euclidean->current_step + 1) % euclidean->euclidean_steps_length;
}

/**
 * @brief Function to remove chord for euclidean composing
 * @param [music_data] Midi struct
 * @param [playing_notes_duration] Tab of current playing notes durations
 * @param [playing_notes] Tab of current playing notes
 * @param [playing_notes_length] Size of 'playing_note_duration' & 'playing_notes'
 */
void remove_chord(t_music_data *music_data, uint8_t *playing_notes_duration,
				  uint8_t *playing_notes, uint8_t playing_notes_length)
{

	printf("\033[1;96mRM func\n");
	for (uint8_t i = 0; i < playing_notes_length; i++)
	{
		printf("Playing notes[%d] : N = %d, D = %d\n", i, playing_notes[i], playing_notes_duration[i]);
	}
	printf("\033[1;37m\n");

	for (uint8_t playing_notes_i = 0; playing_notes_i < playing_notes_length; playing_notes_i++)
	{
		if (playing_notes_duration[playing_notes_i])
		{
			if (playing_notes_duration[playing_notes_i] == 1)
			{
				// end note
				midi_write_measure_note(music_data, OFF, 1, playing_notes[playing_notes_i], 0);
				playing_notes[playing_notes_i] = 0;
			}
			playing_notes_duration[playing_notes_i]--;
		}
	}
}

void midi_delay_divs(t_music_data *music_data, uint16_t divs)
{
	usleep(music_data->current_quarter_value / (music_data->quarter_value / divs));
}

void update_quarter_value(t_music_data *music_data)
{
	// printf("\n\n\n\nIn update quarter value func\n\n\n\n\n\n\n");

	if (music_data->current_quarter_value != music_data->quarter_value_goal)
	{
		if (music_data->current_quarter_value < music_data->quarter_value_goal)
		{
			// if (music_data->quarter_value_goal - music_data->current_quarter_value < music_data->quarter_value_step)
			if (music_data->quarter_value_goal < music_data->current_quarter_value + (uint32_t)((float)music_data->current_quarter_value * music_data->quarter_value_step_updating))
			{
				// printf("++Updated current quarter value :%d\n", music_data->current_quarter_value);
				// printf("++Updated NOT added :%d\n", (uint32_t)((float)music_data->current_quarter_value * music_data->quarter_value_step_updating));

				// printf("--Updated add :", (uint32_t)((float)music_data->current_quarter_value * music_data->quarter_value_step_updating));
				music_data->current_quarter_value = music_data->quarter_value_goal;
			}
			else
			{
				// printf("++Updated current quarter value :%d\n", music_data->current_quarter_value);
				// printf("++Updated add :%d\n", (uint32_t)((float)music_data->current_quarter_value * music_data->quarter_value_step_updating));
				music_data->current_quarter_value += (uint32_t)((float)music_data->current_quarter_value * music_data->quarter_value_step_updating);
			}
		}
		else
		{
			// if (music_data->current_quarter_value - music_data->quarter_value_goal < music_data->quarter_value_step)
			if (music_data->quarter_value_goal > music_data->current_quarter_value - (uint32_t)((float)music_data->current_quarter_value * music_data->quarter_value_step_updating))
			{
				// printf("--Updated current quarter value :%d\n", music_data->current_quarter_value);
				// printf("--Updated goal quarter value :%d\n", music_data->quarter_value_goal);
				// printf("--Updated NOT add :%d\n", (uint32_t)((float)music_data->current_quarter_value * music_data->quarter_value_step_updating));
				music_data->current_quarter_value = music_data->quarter_value_goal;

				// printf("fhew fhewiu fheiwuhf uiewhf iuewhf iewhifh ewiuf hiuewh fiuewhiuf ewiuh fhiufew");
			}
			else
			{
				// printf("--Updated current quarter value :%d\n", music_data->current_quarter_value);
				// printf("--Updated add :%d\n", (uint32_t)((float)music_data->current_quarter_value * music_data->quarter_value_step_updating));
				music_data->current_quarter_value -= (uint32_t)((float)music_data->current_quarter_value * music_data->quarter_value_step_updating);
			}
		}
	}
}

#define EUCLIDEAN_DATAS_LENGTH 3

void midi_write_multiple_euclidean(t_music_data *music_data, t_sensors *sensors_data)
{
	// Number of euclidean "Circles"
	// const uint8_t EUCLIDEAN_DATAS_LENGTH = 3;
	// Initializing euclidean "Circles" datas with NULL
	static t_euclidean euclidean_datas[EUCLIDEAN_DATAS_LENGTH];
	// Start with an reatribution of midi notes in euclidean Circle
	static uint8_t reset_needed = 1;
	// Variable to check last reset time (to reset notes in euclidan circle)
	static uint32_t last_time = 0;
	// Initializing ast reset time with the current timestamp
	if (last_time == 0)
	{
		last_time = time(NULL);
	}

	// Update Midi quarter value to move towards the quarter goal value
	update_quarter_value(music_data);
	// Iterate for each euclidean circle
	for (uint8_t current_euclidean_data = 0; current_euclidean_data < EUCLIDEAN_DATAS_LENGTH; current_euclidean_data++)
	{
		// Initialize euclidean datas with sensors current values
		if (!euclidean_datas[current_euclidean_data].initialized)
		{
			init_euclidean_struct(&euclidean_datas[current_euclidean_data],
								  20,																/* steps_length */
								  2,																/* octave_size */
								  7,																/* chord_list_length */
								  M_MODE_MAJOR,														/* mode */
								  A2,																/* mode_beg_note */
								  4,																/* notes_per_cycle */
								  (uint8_t)map_number(sensors_data->carousel_state, 0, 180, 80, 0), /* mess_chance */
								  1,																/* min_chord_size */
								  1,																/* max_chord_size */
								  (uint8_t)map_number(sensors_data->organ_1, 0, 1024, 48, 35),		/* min_velocity */
								  (uint8_t)map_number(sensors_data->organ_1, 0, 1024, 70, 74),		/* max_velocity */
								  10,																/* min_steps_duration */
								  14																/* max_steps_duration */
			);
			if (current_euclidean_data == 0)
			{
				euclidean_datas[current_euclidean_data].octaves_size = 3;
				euclidean_datas[current_euclidean_data].euclidean_steps_length = 24;
				euclidean_datas[current_euclidean_data].notes_per_cycle = 4;
				euclidean_datas[current_euclidean_data].step_gap =
					euclidean_datas[current_euclidean_data].euclidean_steps_length / euclidean_datas[current_euclidean_data].notes_per_cycle;

				euclidean_datas[current_euclidean_data].mess_chance = 30;
			}
			else if (current_euclidean_data == 1)
			{
				euclidean_datas[current_euclidean_data].euclidean_steps_length = 12;
				euclidean_datas[current_euclidean_data].notes_per_cycle = 2;
				euclidean_datas[current_euclidean_data].mess_chance = 100;
			}
			else if (current_euclidean_data == 2)
			{
				euclidean_datas[current_euclidean_data].octaves_size = 3;
				euclidean_datas[current_euclidean_data].euclidean_steps_length = 14;
				euclidean_datas[current_euclidean_data].notes_per_cycle = 4;
				euclidean_datas[current_euclidean_data].step_gap =
					euclidean_datas[current_euclidean_data].euclidean_steps_length / euclidean_datas[current_euclidean_data].notes_per_cycle;
				euclidean_datas[current_euclidean_data].mess_chance = 100;
			}
		}
	}

	// Change euclidean datas with sensors values
	// \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/
	if ((uint32_t)sensors_data->photodiode_1 > 1024)
	{
		euclidean_datas[1].mess_chance = (uint32_t)map_number((uint32_t)sensors_data->photodiode_1, 0, 4096, 60, 20);

		if (euclidean_datas[1].notes_per_cycle != (uint8_t)map_number(sensors_data->organ_1, 0, 1024, 2, 5))
		{
			reset_needed = 1;
		}
		euclidean_datas[1].notes_per_cycle = (uint8_t)map_number(sensors_data->organ_1, 0, 1024, 2, 5);
		euclidean_datas[1].step_gap =
			euclidean_datas[1].euclidean_steps_length / euclidean_datas[1].notes_per_cycle;

		if (euclidean_datas[0].notes_per_cycle != (uint8_t)map_number(sensors_data->organ_1, 0, 1024, 4, 9))
		{
			reset_needed = 1;
		}
		euclidean_datas[0].notes_per_cycle = (uint8_t)map_number(sensors_data->organ_1, 0, 1024, 4, 9);
		euclidean_datas[0].step_gap =
			euclidean_datas[0].euclidean_steps_length / euclidean_datas[0].notes_per_cycle;

		euclidean_datas[0].min_steps_duration = (uint8_t)map_number(sensors_data->organ_1, 0, 1024, 10, 2);
		euclidean_datas[1].min_steps_duration = (uint8_t)map_number(sensors_data->organ_1, 0, 1024, 10, 2);
		euclidean_datas[2].min_steps_duration = (uint8_t)map_number(sensors_data->organ_1, 0, 1024, 10, 2);

		euclidean_datas[0].max_steps_duration = (uint8_t)map_number(sensors_data->organ_1, 0, 1024, 14, 3);
		euclidean_datas[1].max_steps_duration = (uint8_t)map_number(sensors_data->organ_1, 0, 1024, 14, 3);
		euclidean_datas[2].max_steps_duration = (uint8_t)map_number(sensors_data->organ_1, 0, 1024, 14, 3);
		// (uint32_t)map_number((uint32_t)sensors_data->photodiode_1, 0, 4096, 60, 0);
	}

	if ((uint32_t)sensors_data->photodiode_1 > 2048)
	{
		euclidean_datas[2].mess_chance = (uint32_t)map_number((uint32_t)sensors_data->photodiode_1, 2048, 4096, 80, 20);
	}
	// /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\.

	// Each 30-60 seconds, request to get new notes in euclidean circles
	printf("Time : %d", time(NULL));
	printf("Last Time : %d", last_time);
	if (time(NULL) - last_time > 30 + rand() % 30)
	{
		reset_needed = 1;
		last_time = time(NULL);
		printf("\n\n\n\n\n! RESETING !\n\n\n\n\n\n");
	}

	// If request to get new note, pick new random notes from allowed ones
	if (reset_needed)
	{
		for (uint8_t current_euclidean_data = 0; current_euclidean_data < EUCLIDEAN_DATAS_LENGTH; current_euclidean_data++)
		{
			get_new_euclidean_chords(&euclidean_datas[current_euclidean_data]);
		}
		reset_needed = 0;
	}

	// Print the current euclidean circle values (Step value = index of note in chord list, octave = offset of note)
	for (uint8_t current_euclidean_data = 0; current_euclidean_data < EUCLIDEAN_DATAS_LENGTH; current_euclidean_data++)
	{
		printf("\nEuclidean Cirle %d :\n", current_euclidean_data);
		print_euclidean_steps(&euclidean_datas[current_euclidean_data]);
	}

	uint16_t div_counter = 0;
	uint16_t div_goal = 512; // Whole division (quarter * 4)
	uint16_t looseness = 40; // Humanization in divisions delta, cannot be superior of divgoal / 8

	// Write a midi measure (iterate on each quarter)
	for (uint8_t current_quarter = 0; current_quarter < 4; current_quarter++)
	{
		uint16_t current_div_duration;
		// For each euclidean circle, create corresponding chord
		for (uint8_t current_euclidean_data = 0; current_euclidean_data < EUCLIDEAN_DATAS_LENGTH; current_euclidean_data++)
		{
			write_euclidean_step(music_data, &euclidean_datas[current_euclidean_data]);
		}
		// Remove chords that end this quarter division
		remove_chord(music_data, playing_notes_duration, playing_notes, playing_notes_length);
		if (current_quarter == 3)
		{
			current_div_duration = div_goal - div_counter;
		}
		else
		{
			current_div_duration = div_goal / 4 - looseness + rand() % (looseness * 2);
		}
		// Create a MIDI delay of one division quarter
		midi_delay_divs(music_data, current_div_duration);
		div_counter += current_div_duration;
	}
}

PmTimestamp current_timestamp = 0;

static PmTimestamp portmidi_timeproc(void *time_info)
{
	(void)time_info;
	return (current_timestamp);
}

void process_midi(PtTimestamp timestamp, void *userData)
{
	current_timestamp++;
}

void wait_ms()
{
	PmTimestamp last_time = current_timestamp;
	while (current_timestamp < last_time + 10)
		;
}

float get_voltage_value(uint8_t channel)
{
	spi_config_t spi_config;
	uint8_t tx_buffer[3] = {0};
	uint8_t rx_buffer[3] = {0};
	int spifd;
	float voltage_value;

	spi_config.mode = 0;
	spi_config.speed = 1000000;
	spi_config.delay = 0;
	spi_config.bits_per_word = 8;

	spifd = spi_open(channel < 8 ? "/dev/spidev0.0" : "/dev/spidev0.1", spi_config);
	if (spifd < 0)
	{
		printf("make sure that \"/dev/spidev0.0\" and \"/dev/spidev0.1\" are available\n");
		return (-1);
	}
	tx_buffer[0] = 1;
	tx_buffer[1] = ((8 + channel - (channel < 8 ? 0 : 8)) << 4);
	tx_buffer[2] = 0;
	spi_xfer(spifd, tx_buffer, 3, rx_buffer, 3);
	voltage_value = (float)(((rx_buffer[1] & 3) << 8) + rx_buffer[2]) / 1023.0 * 9.9;
	spi_close(spifd);
	return (voltage_value);
}

int main(void)
{
	PmTimestamp last_time = 0;
	PortMidiStream *stream;
	Pt_Start(1, &process_midi, 0);
	int midi_count = Pm_CountDevices();
	printf("Device number : %d\n", midi_count);
	for (int i = 0; i < midi_count; i++)
	{
		PmDeviceInfo const *device_info = Pm_GetDeviceInfo(i);
		if (device_info && device_info->output)
			printf("id = %d, name : %s\n", i, device_info->name);
	}

	Pm_OpenOutput(&stream, 2, NULL, 128, portmidi_timeproc, NULL, 0);

	for (;;)
	{
		for (int i = 0; i < 16; i++)
		{
			printf("%.2f ", get_voltage_value(i));
		}
		printf("\n");
		sleep(1);
	}
	Pm_Close(&stream);
	Pm_Terminate();
	return (0);
}
