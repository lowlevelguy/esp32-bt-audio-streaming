#include <stdlib.h>
#include "sd.h"

#define PCM_START 44


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
 * @param targets: array of size targets_row_count*targets_row_sz in bytes
 * @param targets_row_count: number of rows in the 2D array
 * @param targets_row_sz: size of each row of the 2D array in bytes
 * @return int: -1 on failure, number of rows written on success.
 */
int parse_config() {
	File conf_file = sd.open(".conf");
	if (!conf_file)
		return -1;

	int bytes_count = 0;
	size_t i = 0, j = 0;
	char buf[128];

	memset(targets, 0, targets_row_count * targets_row_sz);
	while (i < targets_row_count && (bytes_count = conf_file.read(buf, sizeof(buf))) > 0) {
		for (int k = 0; k < bytes_count; k++) {
			// Newline marks new file path, but only if the line was non-empty
			if (buf[k] == '\n') {
				if (j > 0) {
					j = 0;
					i++;

					// Ignore lines further than the max allowed
					if (i == targets_row_count)
						break;
				}
				continue;
			}

			if (j < targets_row_sz-1) {
				targets[i * targets_row_sz + j] = buf[k];
				j++;
			}
			// Ignore characters further than the max allowed
			else
				continue;
		}
	}
	
	// Count uncounted row, if the file doesn't end with a newline
	if (j > 0)
		i++;

	conf_file.close();
	return i;
}

/*
 * Reads at most len bytes of PCM data from file to data.
 *
 * @param file: reference to File object to read data from
 * @param seek: boolean to indicate whether we should seek to PCM_START
 * @param data: buffer to write the data to
 * @param len: maximum size of data to write to buffer in bytes
 */
int32_t get_sound(File& file, bool seek, uint8_t* data, int32_t len) {
	if (seek)
		file.seekSet(PCM_START);
	return file.read(data, len);
}

/*
 * Wrapper function for get_sound to be used for A2DP's get_data_cb callback function.
 * Internally keeps track of the file to play from the targets specificed in the config file.
 * When the counter keeps goes out of scope, it resets back to 0.
 *
 * @param data: buffer to write the data to
 * @param len: maximum size of data to write to buffer in bytes
 */
int32_t get_sound_wrapper(uint8_t* data, int32_t len) {
	static size_t counter = 0;
	static bool counter_changed = true;
	static File target;

	if (counter_changed) {
		if (target)
			target.close();
		target = sd.open(&targets[counter * targets_row_sz]);
		if (!target)
			return 0;
	}

	// If the counter has changed from the previous run, that means we're now about to start
	// reading from a new target file; hence get_sound should seek.
	int32_t ret = get_sound(target, counter_changed, data, len);

	// If the file contained less data than requested, it means we've reached EOF
	if (ret < len) {
		counter++;
		counter %= targets_row_count;
		counter_changed = true;
	} else {
		counter_changed = false;
	}

	return ret;
}
