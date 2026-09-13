#include "codexion.h"
#include <unistd.h>

static t_programador	*buscar_agotado(t_simulacion *simulacion)
{
	int	indice;

	indice = 0;
	while (indice < simulacion->configuracion.programadores)
	{
		if (tiempo_ms() > simulacion->programadores[indice].ultima_compilacion
			+ simulacion->configuracion.agotamiento)
		{
			simulacion->detenida = 1;
			pthread_cond_broadcast(&simulacion->cambio);
			return (&simulacion->programadores[indice]);
		}
		indice++;
	}
	return (NULL);
}

void	*rutina_monitor(void *argumento)
{
	t_simulacion	*simulacion;
	t_programador	*agotado;

	simulacion = argumento;
	while (!esta_detenida(simulacion))
	{
		pthread_mutex_lock(&simulacion->cerrojo);
		agotado = buscar_agotado(simulacion);
		pthread_mutex_unlock(&simulacion->cerrojo);
		if (agotado)
		{
			registrar_agotamiento(agotado);
			break ;
		}
		usleep(1000);
	}
	return (NULL);
}
