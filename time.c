#include <sys/time.h>
#include "codexion.h"

long	obtener_tiempo(void)
{
	struct timeval	tiempo;
	long			segundos;
	long			microsegundos;

	gettimeofday(&tiempo, NULL);
	segundos = tiempo.tv_sec * 1000;
	microsegundos = tiempo.tv_usec / 1000;
	return (segundos + microsegundos);
}
