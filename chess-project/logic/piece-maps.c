#include "piece-maps.h"



int getMapValue(int map[], int rank, int file, bool color) {
	if (!color) {
		rank = 7 - rank;
	}

	return map[rank * 8 + file];
}