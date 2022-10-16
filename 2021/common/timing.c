#include "timing.h"

#include <time.h>


long get_nanos(void) {
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	return (long)ts.tv_sec * 1000000000L + ts.tv_nsec;
}

Timer StartTimer()
{
	Timer t;
	t.start = get_nanos();
	t.end = 0;
	return t;
}

long StopTimer(Timer * timer)
{
	timer->end = get_nanos();
	return timer->end - timer->start;
}
