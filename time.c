#include <sys/time.h>
#include "codexion.h"

long	get_time(void)
{
	struct timeval	time;
	long			seconds;
	long			microseconds;

	gettimeofday(&time, NULL);
	seconds= time.tv_sec * 1000;
	microseconds = time.tv_usec / 1000;
	return (seconds + microseconds);
}
