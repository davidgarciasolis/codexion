#include "codexion.h"

int	esta_detenida(t_simulacion *simulacion)
{
	int	detenida;

	pthread_mutex_lock(&simulacion->cerrojo);
	detenida = simulacion->detenida;
	pthread_mutex_unlock(&simulacion->cerrojo);
	return (detenida);
}

void	detener_simulacion(t_simulacion *simulacion)
{
	pthread_mutex_lock(&simulacion->cerrojo);
	simulacion->detenida = 1;
	pthread_cond_broadcast(&simulacion->cambio);
	pthread_mutex_unlock(&simulacion->cerrojo);
}
