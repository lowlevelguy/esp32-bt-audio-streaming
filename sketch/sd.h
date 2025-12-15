#ifndef ESP32BTAUDIO_SD
#define ESP32BTAUDIO_SD

#include <SdFat.h>

extern SdFat sd;
extern char targets[];
extern const size_t targets_row_count, targets_row_sz;

/*
 * Parses the .conf file at the root of the SD card's filesystem, every line of which contains
 * the relative path to an audio file to be played. The configuration file must consist of at
 * most targets_row_count lines, each line with a length of at most targets_row_sz-1 characters.
 * The filenames are saved to the targets 2D array, and the function returns the number of rows
 * written on success, or -1 if an error occurs.
 *
 * In the case that a file path is of length greater than that which is allowed, it will be
 * truncated, and only targets_row_sz-1 characters will be read.
 * Likewise, in the case that there are more lines than that which is allowed, only the first
 * targets_row_count lines will be read.
 */
int parse_config();

/*
 * Reads at most len bytes of PCM data from file to data.
 *
 * @param file: reference to File object to read data from
 * @param data: buffer to write the data to
 * @param len: maximum size of data to write to buffer in bytes
 */
int32_t get_sound(File& file, uint8_t* data, int32_t len);

/*
 * Wrapper function for get_sound to be used for A2DP's get_data_cb callback function.
 * Internally keeps track of the file to play from the targets specificed in the config file.
 * When the counter keeps goes out of scope, it resets back to 0.
 *
 * @param data: buffer to write the data to
 * @param len: maximum size of data to write to buffer in bytes
 */
int32_t get_sound_wrapper(uint8_t* data, int32_t len);


#endif
