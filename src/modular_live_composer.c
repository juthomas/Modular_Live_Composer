#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <spidev_lib.h>
#include <unistd.h>
// #include <portmidi.h>
// #include <porttime.h>
#include "../inc/midi_notes.h"
#include "../inc/midi_modes.h"
#include "../inc/midi_structs.h"
#include "../inc/midi_euclidean.h"

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

void midi_write_multiple_euclidean(t_music_data *music_data, t_sensors *sensors_data)
{
	// Number of euclidean "Circles"
	const uint8_t euclidean_datas_length = 3;
	// Initializing euclidean "Circles" datas with NULL
	static t_euclidean euclidean_datas[euclidean_datas_length] = {0};
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
	for (uint8_t current_euclidean_data = 0; current_euclidean_data < euclidean_datas_length; current_euclidean_data++)
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
		for (uint8_t current_euclidean_data = 0; current_euclidean_data < euclidean_datas_length; current_euclidean_data++)
		{
			get_new_euclidean_chords(&euclidean_datas[current_euclidean_data]);
		}
		reset_needed = 0;
	}

	// Print the current euclidean circle values (Step value = index of note in chord list, octave = offset of note)
	for (uint8_t current_euclidean_data = 0; current_euclidean_data < euclidean_datas_length; current_euclidean_data++)
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
		for (uint8_t current_euclidean_data = 0; current_euclidean_data < euclidean_datas_length; current_euclidean_data++)
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
