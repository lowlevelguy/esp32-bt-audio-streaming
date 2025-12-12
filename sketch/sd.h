#ifndef ESP32BTAUDIO_SD
#define ESP32BTAUDIO_SD

#include <SPI.h>
#include <SD.h>

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
 *
 *
 * @param sd: reference to a previously setup SD object
 * @param 
 * @param targets_row_count: number of rows in the 2D array
 * @param targets_row_sz: size of each row of the 2D array
 * @return int: -1 on failure, number of rows written on success.
 */
int parseConfig(SD& sd, char* targets, size_t targets_row_count, size_t targets_row_sz);

#endif
