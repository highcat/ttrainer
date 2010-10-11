#include "time_WIN.h"

static int counter = 0;

double get_time_milisec()
{
	return ++counter;
}
