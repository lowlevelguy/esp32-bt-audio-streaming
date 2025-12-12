#include <stdlib.h>
#include "sd.h"

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
int parseConfig(SD& sd, char* targets, size_t targets_row_count, size_t targets_row_sz) {
	if (!targets)
		return -1;
	if (!sd.exists(".conf"))
		return -1;
	
	File conf_file = sd.open(".conf", FILE_READ);
	if (!conf_file)
		return -1;

	int bytes_count = 0;
	size_t i = 0, j = 0;
	char buf[128];

	memset(targets, 0, targets_row_count * targets_row_sz);
	while (i < targets_row_count && (bytes_count = conf_file.read(buf, sizeof(buf))) > 0) {
		for (int k = 0; k < bytes_count; k++) {
			// Newline mark new file path, but only if the line was non-empty
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
				targets[i*targets_row_sz + j] = buf[k];
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
