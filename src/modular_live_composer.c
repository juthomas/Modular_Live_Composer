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
#include "../inc/map_input.h"
#include "../inc/ncurses_utils.h"


#define LOG_ALL 0

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
PmTimestamp current_timestamp = 0;
PortMidiStream *stream;

void wait_ms()
{
	PmTimestamp last_time = current_timestamp;
	while (current_timestamp < last_time + 10)
		;
}

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
	// printf("\033[1;32m WTFFFF-%d-%d-%d-%d-%d- \033[1;37m\n", x, in_min, in_max, out_min, out_max);
	// printf("\033[1;32m OKKAAY-%d-%d-%d-%d-%d- \033[1;37m\n", (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);

	return ((int64_t)x - in_min) * ((int64_t)out_max - out_min) / ((int64_t)in_max - in_min) + (int64_t)out_min;
}

/**
 * @brief Write a note state into "midi_write_measure" function
 * @param [state] Logic state of note (ON/OFF)
 * @param [channel] Selection of midi channel (0-16)
 * @param [note] Selection of midi note (1-127)
 * @param [velocity] Selection of velocity (power) (1-127)
 */
void midi_write_measure_note(t_music_data *music_data, unsigned char state,
							 unsigned char channel, unsigned char note, unsigned char velocity)
{
	// if (LOG_ALL)
	//printf("\033[1;35mwrite measure note : state=%s channel=%d note=%d velocity=%d\033[1;37m\n\n",
	//	   (state == ON ? "ON" : "OFF"), channel, note, velocity);

			char printf_hack[64];
			snprintf(printf_hack, 64, "write measure note : state=%s channel=%d note=%d velocity=%d",
		   (state == ON ? "ON" : "OFF"), channel, note, velocity);
			write_value(&curses_env, printf_hack);



	// MIDI_delta_time(music_data->midi_file, 0);
	// MIDI_delta_time(music_data->midi_file_redundancy, 0);
	// MIDI_Note(music_data->midi_file, state, channel, note, velocity);
	// MIDI_Note(music_data->midi_file_redundancy, state, channel, note, velocity);
	// printf("current_note : 0x%02X (%03d)\n", current_note, current_note);
	if (state == ON)
	{
		PmError pme = Pm_WriteShort(stream, current_timestamp,
									// Pm_Message(type << 4 | chan, byte1, byte2)
									Pm_Message(0x9 << 4, note, velocity)); // status | channel, note, velocity;
	}
	else if (state == OFF)
	{
		PmError pme = Pm_WriteShort(stream, current_timestamp,
									// Pm_Message(type << 4 | chan, byte1, byte2)
									Pm_Message(0x8 << 4, note, 0));
	}

	wait_ms();
}

/**
 * @brief Function to create chord for euclidean composing
 * @param [music_data] Midi struct
 * @param [playing_notes_duration] Tab of current playing notes durations
 * @param [playing_notes] Tab of current playing notes
 * @param [playing_notes_length] Size of 'playing_note_duration' & 'playing_notes'
 * @param [chords_list] Tab of selected mode notes
 * @param [note_i] Index of note in selected mode notes
 * @param [note_offset] Offset of note (starting note of mode)
 * @param [chord_size] Size of chord (simultaneous played notes)
 * @param [velocity] Velocity of chord/note
 * @param [steps_duration] Duration of note converted in euclidean steps
 */
void create_chord(t_music_data *music_data, uint8_t *playing_notes_duration, uint8_t *playing_notes,
				  uint8_t playing_notes_length, uint8_t mode, int16_t note_i,
				  uint8_t note_offset, uint8_t chord_size, uint8_t velocity, uint8_t steps_duration)
{
	bool current_note_done = false;

	if (LOG_ALL)
	{
		//printf("\033[1;32mChord to play\n");
			write_value(&curses_env, "Chord to play");

		for (int i = 0; i < chord_size; i++)
		{
			//printf("Note Chord[%d] : %d\n", i, note_offset + ((note_i & 0xFF00) >> 8) * 12 + g_midi_mode[mode].mode_sequence[((note_i & 0xFF) + 2 * i) % 7] + 12 * (((note_i & 0xFF) + 2 * i) / 7));
					char printf_hack[64];
			snprintf(printf_hack, 64, "Note Chord[%d] : %d\n", i, note_offset + ((note_i & 0xFF00) >> 8) * 12 + g_midi_mode[mode].mode_sequence[((note_i & 0xFF) + 2 * i) % 7] + 12 * (((note_i & 0xFF) + 2 * i) / 7));
			write_value(&curses_env, printf_hack);
		}
		//printf("\033[1;37m\n");
	}

	for (uint8_t current_note = 0; current_note < chord_size; current_note++)
	{
		current_note_done = false;
		// If the note is currently played
		// Just add time to that note
		for (uint8_t playing_notes_i = 0; playing_notes_i < playing_notes_length; playing_notes_i++)
		{
			if (playing_notes[playing_notes_i] == note_offset + ((note_i & 0xFF00) >> 8) * 12 + g_midi_mode[mode].mode_sequence[((note_i & 0xFF) + 2 * current_note) % 7] + 12 * (((note_i & 0xFF) + 2 * current_note) / 7))
			{
				// printf("Note_i P1: %d, current_note : %d, calcul : %d, calcul_tab : %d\n", (note_i & 0b1111), current_note,((note_i & 0b1111) + 2 * current_note)% 7,
				// g_midi_mode[mode].mode_sequence[((note_i & 0b1111) + 2 * current_note)% 7] );

				playing_notes_duration[playing_notes_i] = steps_duration;
				current_note_done = true;
			}
		}
		// If the note isnt played
		// Create that note !
		for (uint8_t playing_notes_i = 0; playing_notes_i < playing_notes_length && !current_note_done; playing_notes_i++)
		{
			if (!playing_notes_duration[playing_notes_i])
			{
				// printf("Note_i : %d, current_note : %d, calcul : %d, calcul_tab : %d\n", (note_i & 0b1111), current_note,((note_i & 0b1111) + 2 * current_note)% 7,
				// g_midi_mode[mode].mode_sequence[((note_i & 0b1111) + 2 * current_note)% 7] );

				playing_notes_duration[playing_notes_i] = steps_duration;
				playing_notes[playing_notes_i] = note_offset + ((note_i & 0xFF00) >> 8) * 12 + g_midi_mode[mode].mode_sequence[((note_i & 0xFF) + 2 * current_note) % 7] + 12 * (((note_i & 0xFF) + 2 * current_note) / 7);
				// beg note
				midi_write_measure_note(music_data, ON, 1, playing_notes[playing_notes_i], velocity);
				break;
			}
		}
	}
}

/**
 * @brief Function to get a chords list indexes
 * @param [chords_list] chords list returned
 * @param [chords_size] Number of chords desired
 */
void get_chords_list(uint8_t *chords_list, uint8_t chords_size)
{
	// uint8_t chords_list[chords_size];

	for (uint8_t i = 0; i < chords_size; i++)
	{ // i * 2
		// chords_list[i] = /* starting_note + */ g_midi_mode[mode].mode_sequence[(i * 2) % 7];
		chords_list[i] = /* starting_note + */ (i * 2) % 7;
	}
	// return (chords_list);
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
 * @brief Function to get a new note in allowed ones
 * @param [chords_list] List of allowed chords
 * @param [chord_list_length] Size of allowed chords list
 * @param [current_step] Current step in euclidean circle
 * @param [euclidean_steps] Euclidean steps (contain notes)
 * @return New Chord
 */
int16_t get_new_chord_from_list(uint8_t *chords_list, uint8_t chord_list_length, uint8_t current_step, int16_t *euclidean_steps)
{
	int16_t chord_to_test = 0;
	uint8_t steps = 0;

	int16_t available_chords_list[chord_list_length];
	uint8_t available_chords_list_len = 0;

	// Check for each chords indexes in mode
	for (uint8_t i = 0; i < chord_list_length; i++)
	{
		chord_to_test = chords_list[i];
		steps = 0;
		// While chords indexes doesnt exist in euclidean steps and dont check for steps not yet attributed
		while (euclidean_steps[steps] != chord_to_test && steps < current_step)
		{
			steps++;
		}
		// If chords indexes doesnt exist in euclidean steps, feed in a chord list
		if (euclidean_steps[steps] != chord_to_test)
		{
			available_chords_list[available_chords_list_len] = chord_to_test;
			available_chords_list_len++;
		}
	}
	// If all possible chords arent taken, take a random chord from the available chord list
	if (available_chords_list_len)
	{
		return (available_chords_list[rand() % available_chords_list_len]);
	}
	// If all chords allready exist in the euclidean cirle, simply get a random chord in basic chord list
	else
	{
		return (chords_list[rand() % chord_list_length]);
	}
}

//TODO: TESTING NOTE SHIFTING AND FIX THIS FCKING SHIT + TROUVER UN FIX POUR LE RESET ETC...
void shift_euclidean_steps(t_euclidean *euclidean, int shift_value)
{
	for (uint8_t steps = 0; steps < euclidean->euclidean_steps_length; steps++)
	{
		if (euclidean->euclidean_steps[steps] != -1)
		{
				char printf_hack[64];
			snprintf(printf_hack, 64,"BEFORE NOTE %x %d\n", euclidean->euclidean_steps[steps], shift_value );
			write_value(&curses_env, printf_hack);		

			uint32_t tmp = 0;

			int tmp_shift = shift_value;
			// printf("Bidule : %d\n", ((euclidean->euclidean_steps[steps] & 0xFF00) >> 8) * 7 + (((euclidean->euclidean_steps[steps] & 0x00FF) + tmp_shift)));
			if (((euclidean->euclidean_steps[steps] & 0xFF00) >> 8) * 7 + (((euclidean->euclidean_steps[steps] & 0x00FF) + tmp_shift)) < 0)
				tmp_shift = tmp_shift - (((euclidean->euclidean_steps[steps] & 0xFF00) >> 8) * 7 + (((euclidean->euclidean_steps[steps] & 0x00FF) + tmp_shift)));
			// printf("New shift : %d\n", tmp_shift);
			tmp = (7 + ((euclidean->euclidean_steps[steps] & 0x00FF) + tmp_shift)) % 7;
			tmp |= ((((euclidean->euclidean_steps[steps] & 0xFF00) >> 8) + ((7 + ((euclidean->euclidean_steps[steps] & 0x00FF) + tmp_shift)) / 7 - 1)) << 8);
			euclidean->euclidean_steps[steps] = tmp;

			snprintf(printf_hack, 64,"After NOTE %x\n", euclidean->euclidean_steps[steps] & 0xFFFF);
			write_value(&curses_env, printf_hack);	
		} 
	}
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
			if (LOG_ALL)
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
		
		//printf("Step value : %d, octave : %d\n", euclidean->euclidean_steps[steps] & 0xFF,
		//	   (euclidean->euclidean_steps[steps] & 0xFF00) >> 8);
	}
	//printf("Chord list : ");
	for (uint8_t chord_list_i = 0; chord_list_i < euclidean->chords_list_length; chord_list_i++)
	{
		//printf("%d,", euclidean->chords_list[chord_list_i]);
	}
	//printf("\n");
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

	if (LOG_ALL)
	{

		printf("\033[1;96mRM func\n");
		for (uint8_t i = 0; i < playing_notes_length; i++)
		{
			printf("Playing notes[%d] : N = %d, D = %d\n", i, playing_notes[i], playing_notes_duration[i]);
		}
		printf("\033[1;37m\n");
	}

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
	usleep(music_data->current_quarter_value / (music_data->quarter_value / divs) * 100);
	//printf("-----SLEEP MS : %u :----\n", music_data->current_quarter_value / (music_data->quarter_value / divs) * 100);

			char printf_hack[64];
			snprintf(printf_hack, 64,"-----SLEEP MS : %u :----\n", music_data->current_quarter_value / (music_data->quarter_value / divs) * 100);
			write_value(&curses_env, printf_hack);


}

void print_sensors_data(t_sensors *sensors)
{
	printf(" ----------------------------\n");
	printf(" photodiode_1 %d \n", sensors->photodiode_1);
	printf(" photodiode_2 %d \n", sensors->photodiode_2);
	printf(" photodiode_3 %d \n", sensors->photodiode_3);
	printf(" photodiode_4 %d \n", sensors->photodiode_4);
	printf(" photodiode_5 %d \n", sensors->photodiode_5);
	printf(" photodiode_6 %d \n", sensors->photodiode_6);
	printf(" temperature_1 %d \n", sensors->temperature_1);
	printf(" temperature_2 %d \n", sensors->temperature_2);
	printf(" temperature_3 %d \n", sensors->temperature_3);
	printf(" temperature_4 %d \n", sensors->temperature_4);
	printf(" temperature_5 %d \n", sensors->temperature_5);
	printf(" temperature_6 %d \n", sensors->temperature_6);
	printf(" temperature_7 %d \n", sensors->temperature_7);
	printf(" temperature_8 %d \n", sensors->temperature_8);
	printf(" temperature_9 %d \n", sensors->temperature_9);
	printf(" temperature_10 %d \n", sensors->temperature_10);
	printf(" microphone %d \n", sensors->microphone);
	printf(" spectro_current %d \n", sensors->spectro_current);
	printf(" organ_current %d \n", sensors->organ_current);
	printf(" vin_current %d \n", sensors->vin_current);
	printf(" q7_current %d \n", sensors->q7_current);
	printf(" t5v_current %d \n", sensors->t5v_current);
	printf(" t3_3v_current %d \n", sensors->t3_3v_current);
	printf(" motor_current %d \n", sensors->motor_current);
	printf(" carousel_state %d \n", sensors->carousel_state);
	printf(" lid_state %d \n", sensors->lid_state);
	printf(" organ_1 %d \n", sensors->organ_1);
	printf(" organ_2 %d \n", sensors->organ_2);
	printf(" organ_3 %d \n", sensors->organ_3);
	printf(" organ_4 %d \n", sensors->organ_4);
	printf(" organ_5 %d \n", sensors->organ_5);
	printf(" organ_6 %d \n", sensors->organ_6);
	printf(" timestamp %d \n", sensors->timestamp);
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

/**
 * @brief Initialize music_data
 * @param [music_data] Midi struct of midi file
 * @param [partition_duration] Partition duration in minutes
 * @param [quarter_value] Quarter value in micro-seconds
 * @param [tempo_acceleration] Acceleration per measure in percentage (1.0=100%, 0.05=5%)
 *
 */
void init_music_data(t_music_data *music_data, uint32_t partition_duration,
					 uint32_t quarter_value, uint32_t quarter_value_goal,
					 float tempo_acceleration)
{
	music_data->partition_duration = 60000000 * partition_duration; // 10 minutes
	music_data->measure_value = 500000 * 4;							// useless
	music_data->measures_writed = 0;
	music_data->delta_time = 0;
	music_data->quarter_value_step = 100000;
	// music_data->quarter_value_goal = quarter_value;
	music_data->quarter_value_goal = quarter_value_goal;
	//                          100000
	music_data->quarter_value = 500000; // define metadata 500000=120bpm
	music_data->current_quarter_value = quarter_value;
	music_data->quarter_value_step_updating = tempo_acceleration; // Acceleration per measure in percentage (1.0=100%, 0.05=5%)
}

#define EUCLIDEAN_DATAS_LENGTH 4

#define FIX_4096 4096//3686

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
	static uint16_t measure_count_1 = 0;
	static uint16_t measure_count_2 = 0;
	static uint16_t measure_count_3 = 0;


	static int16_t delta_shift = 10;

	static int16_t circle_3_reset_ctdown = 0;

	// Initializing ast reset time with the current timestamp
	if (last_time == 0)
	{
		last_time = time(NULL);
	}

	//print_sensors_data(sensors_data);

	music_data->quarter_value_goal = (uint32_t)map_number((uint32_t)sensors_data->photodiode_1, 0, FIX_4096, 100000000, 35000000);
	// Update Midi quarter value to move towards the quarter goal value
	//printf("\033[1;32mmusic data current quarter value : %d\033[1;37m\n", music_data->current_quarter_value);
	// 5000000
	music_data->current_quarter_value = (uint32_t)map_number((uint32_t)sensors_data->photodiode_1, 0, FIX_4096, 100000000, 3500000); // RM THAT !!

	write_speed(&curses_env, music_data->current_quarter_value);


	//printf("\033[1;32mmusic data current quarter value after  : %d\033[1;37m\n", music_data->current_quarter_value);

	// update_quarter_value(music_data); RM TO FIX
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
								  (uint8_t)map_number(sensors_data->carousel_state, 0, 119, 80, 0), /* mess_chance */
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
				euclidean_datas[current_euclidean_data].euclidean_steps_length = (uint8_t)map_number((uint32_t)sensors_data->temperature_1, 0, FIX_4096, 13, 39);
				euclidean_datas[current_euclidean_data].mode_beg_note = A2 - 12;
				euclidean_datas[current_euclidean_data].octaves_size = 3;
				// euclidean_datas[current_euclidean_data].euclidean_steps_length = 13;
				euclidean_datas[current_euclidean_data].notes_per_cycle = 2;
				euclidean_datas[current_euclidean_data].mess_chance = 100;
			}
			else if (current_euclidean_data == 2)
			{
				euclidean_datas[current_euclidean_data].octaves_size = 3;
				euclidean_datas[current_euclidean_data].euclidean_steps_length = (uint8_t)map_number((uint32_t)sensors_data->temperature_2, 0, FIX_4096, 15, 45);
				// euclidean_datas[current_euclidean_data].euclidean_steps_length = 15;
				euclidean_datas[current_euclidean_data].notes_per_cycle = 4;
				euclidean_datas[current_euclidean_data].step_gap =
					euclidean_datas[current_euclidean_data].euclidean_steps_length / euclidean_datas[current_euclidean_data].notes_per_cycle;
				euclidean_datas[current_euclidean_data].mess_chance = 100;
			}
			else if (current_euclidean_data == 3)
			{
				euclidean_datas[current_euclidean_data].octaves_size = 1;
				euclidean_datas[current_euclidean_data].euclidean_steps_length = 4;
				// euclidean_datas[current_euclidean_data].euclidean_steps_length = 15;
				euclidean_datas[current_euclidean_data].mode_beg_note = A2 - 12;
				euclidean_datas[current_euclidean_data].notes_per_cycle = 2;
				euclidean_datas[current_euclidean_data].step_gap =
				euclidean_datas[current_euclidean_data].euclidean_steps_length / euclidean_datas[current_euclidean_data].notes_per_cycle;
				euclidean_datas[current_euclidean_data].mess_chance = 100;
				euclidean_datas[current_euclidean_data].min_steps_duration = 1;
				euclidean_datas[current_euclidean_data].max_steps_duration = 2;
			}
		}
	}

	char printf_hack[64];

	if (delta_shift != (uint32_t)map_number((uint32_t)sensors_data->spectro_current, 0, 33535, 0, 10))
	{

		int16_t tmp = (uint32_t)map_number((uint32_t)sensors_data->spectro_current, 0,33535, 0, 10) - delta_shift;
		shift_euclidean_steps(&euclidean_datas[3], tmp);
		delta_shift += tmp;
		// reset_needed = 1;
	}

	if (circle_3_reset_ctdown <= 0)
	{
		get_new_euclidean_chords(&euclidean_datas[3]);
		shift_euclidean_steps(&euclidean_datas[3], 10);
		delta_shift = 10;
		circle_3_reset_ctdown = 10;
		snprintf(printf_hack, 64,"4TH CIRCLE RESET <<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
		write_value(&curses_env, printf_hack);
	}
	if (music_data->current_quarter_value < 30000000)
	{
		euclidean_datas[3].mess_chance = 20;
	}
	else
	{
		euclidean_datas[3].mess_chance = 100;
	}

	circle_3_reset_ctdown--;


	euclidean_datas[0].min_chord_size = (sensors_data->vin_current % 4) + 1; //(uint8_t)map_number((uint32_t)sensors_data->temperature_3, 0, FIX_4096 - 400, 1, 7);	//temperature_3
	euclidean_datas[0].max_chord_size = (sensors_data->vin_current % 4) + 1;	//temperature_3
	
	snprintf(printf_hack, 64,"CHORDSIZE CIRCLE 1 %d\n", euclidean_datas[0].min_chord_size );
	write_value(&curses_env, printf_hack);
	
	static uint16_t mode_requested = A2;
	static uint16_t type_mode_requested = M_MODE_MAJOR;


	if (sensors_data->carousel_state < 20)
	{
		mode_requested = A2;
		// if (euclidean_datas[0].mode_beg_note != A2)
		// 	reset_needed = 1;
		// euclidean_datas[0].mode_beg_note = A2;
		// euclidean_datas[1].mode_beg_note = A2;
		// euclidean_datas[2].mode_beg_note = A2;
	}
	else if (sensors_data->carousel_state < 40)
	{
		mode_requested = B2;

		// if (euclidean_datas[0].mode_beg_note != B2)
		// 	reset_needed = 1;
		// euclidean_datas[0].mode_beg_note = B2;
		// euclidean_datas[1].mode_beg_note = B2;
		// euclidean_datas[2].mode_beg_note = B2;
	}
	else if (sensors_data->carousel_state < 60)
	{
		mode_requested = C2;

		// if (euclidean_datas[0].mode_beg_note != C2)
		// 	reset_needed = 1;
		// euclidean_datas[0].mode_beg_note = C2;
		// euclidean_datas[1].mode_beg_note = C2;
		// euclidean_datas[2].mode_beg_note = C2;
	}
	else if (sensors_data->carousel_state < 80)
	{
		mode_requested = D2;

		// if (euclidean_datas[0].mode_beg_note != D2)
		// 	reset_needed = 1;
		// euclidean_datas[0].mode_beg_note = D2;
		// euclidean_datas[1].mode_beg_note = D2;
		// euclidean_datas[2].mode_beg_note = D2;
	}
	else if (sensors_data->carousel_state < 100)
	{
		mode_requested = E2;

		// if (euclidean_datas[0].mode_beg_note != E2)
		// 	reset_needed = 1;
		// euclidean_datas[0].mode_beg_note = E2;
		// euclidean_datas[1].mode_beg_note = E2;
		// euclidean_datas[2].mode_beg_note = E2;
	}
	else if (sensors_data->carousel_state < 110)
	{
		mode_requested = F2;

		// if (euclidean_datas[0].mode_beg_note != F2)
		// 	reset_needed = 1;
		// euclidean_datas[0].mode_beg_note = F2;
		// euclidean_datas[1].mode_beg_note = F2;
		// euclidean_datas[2].mode_beg_note = F2;
	}
	else
	{
		mode_requested = G2;

		// if (euclidean_datas[0].mode_beg_note != G2)
		// 	reset_needed = 1;
		// euclidean_datas[0].mode_beg_note = G2;
		// euclidean_datas[1].mode_beg_note = G2;
		// euclidean_datas[2].mode_beg_note = G2;
	}

	type_mode_requested = sensors_data->lid_state / 5;

	if (sensors_data->photodiode_1 < 500)
	{
		if (euclidean_datas[0].mode != type_mode_requested)
			reset_needed = 1;
		if (euclidean_datas[0].mode_beg_note != mode_requested)
			reset_needed = 1;
		euclidean_datas[0].mode_beg_note = mode_requested;
		euclidean_datas[1].mode_beg_note = mode_requested;
		euclidean_datas[2].mode_beg_note = mode_requested;

		euclidean_datas[0].mode = type_mode_requested;
		euclidean_datas[1].mode = type_mode_requested;
		euclidean_datas[2].mode = type_mode_requested;

	}

	write_mode(&curses_env, g_midi_mode[euclidean_datas[0].mode].name , g_notes_definitions[euclidean_datas[0].mode_beg_note - A0].name);
	// write_mode(&curses_env, g_midi_mode[euclidean_datas[0].mode].name , g_notes_definitions[1].name);
	// write_mode(&curses_env, g_midi_mode[euclidean_datas[0].mode].name , "bonjour");




	// Change euclidean datas with sensors values
	// \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/ \/
	if ((uint32_t)sensors_data->photodiode_2 > 1024)
	{
		euclidean_datas[1].mess_chance = 20; //(uint32_t)map_number((uint32_t)sensors_data->photodiode_2, 0, 4096, 60, 20);

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
	else
	{
		euclidean_datas[1].mess_chance = 100;
	}

	if ((uint32_t)sensors_data->photodiode_2 > 2048)
	{
		euclidean_datas[2].mess_chance = 20; //(uint32_t)map_number((uint32_t)sensors_data->photodiode_2, 2048, 4096, 80, 20);
	}
	else
	{
		euclidean_datas[2].mess_chance = 100;
	}
	// /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\.

	// Each 30-60 seconds, request to get new notes in euclidean circles
	if (LOG_ALL)
	{

		printf("Time : %d", time(NULL));
		printf("Last Time : %d", last_time);
	}
	// if (time(NULL) - last_time > 30 + rand() % 30)
	// {
	// 	reset_needed = 1;
	// 	last_time = time(NULL);
	// 	printf("\n\n\n\n\n! RESETING !\n\n\n\n\n\n");
	// }

	if (measure_count_1 > 32)
	{
		get_new_euclidean_chords(&euclidean_datas[0]);
		measure_count_1 = 0;
		//printf("\n\n\n\n\n! RESETING 0 !\n\n\n\n\n\n");

			write_value(&curses_env, "! RESETING 0!");


	}

	if (measure_count_2 > 55)
	{
		get_new_euclidean_chords(&euclidean_datas[1]);
		measure_count_2 = 0;
		//printf("\n\n\n\n\n! RESETING 1 !\n\n\n\n\n\n");
			write_value(&curses_env, "! RESETING 1 !");


	}

	if (measure_count_3 > 83)
	{
		get_new_euclidean_chords(&euclidean_datas[2]);
		measure_count_3 = 0;
		//printf("\n\n\n\n\n! RESETING 2 !\n\n\n\n\n\n");

			write_value(&curses_env, "! RESETING 2 !");

	}



	// Initialize notes or if requested to get new note, pick new random notes from allowed ones
	if (reset_needed)
	{
		for (uint8_t current_euclidean_data = 0; current_euclidean_data < EUCLIDEAN_DATAS_LENGTH - 1; current_euclidean_data++)
		{
			get_new_euclidean_chords(&euclidean_datas[current_euclidean_data]);
		}

		//printf("\n\n\n\n\n! RESETING !\n\n\n\n\n\n");

			write_value(&curses_env, "! FULL RESETING !");
// delta_shift = 0;
		reset_needed = 0;
	}

	if (LOG_ALL)
	{
		// Print the current euclidean circle values (Step value = index of note in chord list, octave = offset of note)
		for (uint8_t current_euclidean_data = 0; current_euclidean_data < EUCLIDEAN_DATAS_LENGTH; current_euclidean_data++)
		{
			printf("\nEuclidean Cirle %d :\n", current_euclidean_data);
			print_euclidean_steps(&euclidean_datas[current_euclidean_data]);
		}
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
			// if (current_euclidean_data == 3)
			// {


			// char printf_hack[64];
					// snprintf(printf_hack, 64,"BEG WRITING CHORD %d\n", current_euclidean_data);

					// write_value(&curses_env, printf_hack);
					write_euclidean_step(music_data, &euclidean_datas[current_euclidean_data]);
					// snprintf(printf_hack, 64,"END WRITING CHORD %d\n", current_euclidean_data);
					// write_value(&curses_env, printf_hack);
			// }
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
	measure_count_1++;
	measure_count_2++;
	measure_count_3++;



}

static PmTimestamp portmidi_timeproc(void *time_info)
{
	(void)time_info;
	return (current_timestamp);
}

void process_midi(PtTimestamp timestamp, void *userData)
{
	current_timestamp++;
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

void get_sensors_data(t_sensors *sensors)
{
	uint16_t sensor_number = sizeof(g_map_input) / sizeof(t_map_input);

	// printf("addr of 1: %d|", sensors);
	// printf("addr of 2: %d|", &sensors->date);
	// printf("addr of 3: %d|", &sensors->time);

	// printf("Offsetof time : %d", offsetof(t_sensors, time));
	// printf("addr of 3: %d|", (uint32_t)sensors + offsetof(t_sensors, time));
	int *tmp = (int *)((uint32_t)sensors + offsetof(t_sensors, time));
	*tmp = 42;
	// printf("Time : %d ||", sensors->time);
	
	//  wclear(curses_env.top);
	for (uint16_t i = 0; i < sensor_number; i++)
	{
		switch (g_map_input[i].data_type)
		{
		case INTEGER:
		{
			int16_t *tmp = (int16_t *)((uint32_t)sensors + g_map_input[i].offset);
			float voltage_value = get_voltage_value(g_map_input[i].input_nu);
			*tmp = map_number(((uint32_t)((float)voltage_value * 100)), 0, 1000, 0, g_map_input[i].int_max);
			draw_sensors_infos(&curses_env, i, voltage_value, *tmp, g_map_input[i].name, g_map_input[i].int_max, g_map_input[i].input_nu);
		}
		break;
		case CHAR:
		{
			int8_t *tmp = (int8_t *)((uint32_t)sensors + g_map_input[i].offset);
			float voltage_value = get_voltage_value(g_map_input[i].input_nu);
			*tmp = map_number(((uint32_t)((float)voltage_value * 100)), 0, 1000, 0, g_map_input[i].char_max);
			draw_sensors_infos(&curses_env, i, voltage_value, *tmp, g_map_input[i].name, g_map_input[i].char_max, g_map_input[i].input_nu);
		}
		break;
		case BINARY:
		{
			int8_t *tmp = (int8_t *)((uint32_t)sensors + g_map_input[i].offset);
			float voltage_value = get_voltage_value(g_map_input[i].input_nu);
			*tmp = voltage_value < 5 ? 0 : 1;
			draw_sensors_infos(&curses_env, i, voltage_value, *tmp, g_map_input[i].name, 1, g_map_input[i].input_nu);
		}
		break;
		default:
			break;
		}
	}
	if (LOG_ALL)
	{
		print_sensors_data(sensors);
	}
}

void test_all_notes(PortMidiStream *stream)
{
	PmTimestamp last_time = current_timestamp;
	PmTimestamp last_timestamp = current_timestamp;
	uint8_t current_note = 0;

	while (current_timestamp < last_timestamp + 60000)
	{
		last_time = last_time + 300;
		while (last_time > current_timestamp)
			;
		//printf("current_note : 0x%02X (%03d)\n", current_note, current_note);
		PmError pme = Pm_WriteShort(stream, current_timestamp,
									// Pm_Message(type << 4 | chan, byte1, byte2)
									Pm_Message(0x9 << 4, current_note, 100)); // status | channel, note, velocity;
		(void)pme;
		if (current_note > 0)
		{
			// Wait ms make sure that the last signal is correctly received by the Midi output device
			wait_ms();
			PmError pme = Pm_WriteShort(stream, current_timestamp,
										// Pm_Message(type << 4 | chan, byte1, byte2)
										Pm_Message(0x8 << 4, current_note - 1, 0));
			(void)pme;
		}
		current_note = current_note < 0xFF ? current_note + 1 : 0;
	}
}

int main(void)
{
	init_curses(&curses_env);
	PmTimestamp last_time = 0;
	// PortMidiStream *stream;
	Pt_Start(1, &process_midi, 0);
	int midi_count = Pm_CountDevices();
	//printf("Device number : %d\n", midi_count);

	// durée d'une partition 40 000 000us
	t_music_data music_data = {0};
	// init_music_data(&music_data, 10, 1000000, 250000, 0.03);
	init_music_data(&music_data, 10, 100000000, 250000, 0.33);

	t_sensors sensorsData = {0};

	for (int i = 0; i < midi_count; i++)
	{
		PmDeviceInfo const *device_info = Pm_GetDeviceInfo(i);
		if (device_info && device_info->output)
		{
			char printf_hack[64];
			snprintf(printf_hack, 64, "id = %d, name : %s\n", i, device_info->name);
			write_value(&curses_env, printf_hack);

			//printf("id = %d, name : %s\n", i, device_info->name);
		}
	}

	Pm_OpenOutput(&stream, 2, NULL, 128, portmidi_timeproc, NULL, 0);
	// test_all_notes(stream);
	for (;;)
	{
		for (int i = 0; i < 16; i++)
		{
			// //printf("%.2f ", get_voltage_value(i));
			get_sensors_data(&sensorsData);
		}
		// //printf("\n");
		midi_write_multiple_euclidean(&music_data, &sensorsData);
		// sleep(1);
	}
	Pm_Close(&stream);
	Pm_Terminate();
	exit_curses2(&curses_env);
	return (0);
}
