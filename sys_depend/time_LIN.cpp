#include "time_LIN.h"

double get_time_milisec()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec*1000.+t.tv_usec/1000.;
}
