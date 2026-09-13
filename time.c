#include "codexion.h"
#include <sys/time.h>

long	tiempo_ms(void)
{
	struct timeval	marca_tiempo;

	gettimeofday(&marca_tiempo, NULL);
	return (marca_tiempo.tv_sec * 1000L + marca_tiempo.tv_usec / 1000);
}
