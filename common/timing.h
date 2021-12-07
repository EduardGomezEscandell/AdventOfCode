#ifndef TIMING_INCLUDED_H
#define TIMING_INCLUDED_H

typedef struct
{
	long start;
	long end;
} Timer;


Timer StartTimer();
long StopTimer(Timer * timer);


#endif