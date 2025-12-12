#include <stdlib.h>
#include "sd.h"

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
			if (buf[k] == '\n') {
				if (j > 0) {
					j = 0;
					i++;

					if (i == targets_row_count)
						break;
				}
				continue;
			}

			if (j < targets_row_sz-1) {
				targets[i*targets_row_sz + j] = buf[k];
				j++;
			}
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
