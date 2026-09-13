#include "codexion.h"
#include <sys/time.h>

int	puede_tomar(t_programador *programador, long ahora)
{
	t_simulacion	*simulacion;
	int		izquierda;
	int		derecha;

	simulacion = programador->simulacion;
	izquierda = programador->id - 1;
	derecha = programador->id % simulacion->configuracion.programadores;
	if (!simulacion->llaves[izquierda].libre || simulacion->llaves[izquierda].lista_en > ahora)
		return (0);
	if (izquierda != derecha && (!simulacion->llaves[derecha].libre
			|| simulacion->llaves[derecha].lista_en > ahora))
		return (0);
	return (1);
}

static void	esperar_turno(t_programador *programador)
{
	t_simulacion			*simulacion;
	struct timeval	tiempo;
	struct timespec	limite;

	simulacion = programador->simulacion;
	while (!programador->concedido && !simulacion->detenida)
	{
		gettimeofday(&tiempo, NULL);
		limite.tv_sec = tiempo.tv_sec;
		limite.tv_nsec = (tiempo.tv_usec + 1000) * 1000;
		if (limite.tv_nsec >= 1000000000)
		{
			limite.tv_sec++;
			limite.tv_nsec -= 1000000000;
		}
		pthread_cond_timedwait(&simulacion->cambio, &simulacion->cerrojo, &limite);
		conceder_esperando(simulacion);
	}

}

int	tomar_llaves(t_programador *programador)
{
	t_simulacion	*simulacion;

	simulacion = programador->simulacion;
	pthread_mutex_lock(&simulacion->cerrojo);
	programador->turno = simulacion->siguiente_turno++;
	programador->esperando = 1;
	programador->concedido = 0;
	cola_insertar(simulacion, programador);
	conceder_esperando(simulacion);
	esperar_turno(programador);
	pthread_mutex_unlock(&simulacion->cerrojo);
	return (programador->concedido);
}

void	liberar_llaves(t_programador *programador)
{
	t_simulacion	*simulacion;
	int		izquierda;
	int		derecha;
	long	lista_en;

	simulacion = programador->simulacion;
	izquierda = programador->id - 1;
	derecha = programador->id % simulacion->configuracion.programadores;
	pthread_mutex_lock(&simulacion->cerrojo);
	lista_en = tiempo_ms() + simulacion->configuracion.enfriamiento;
	simulacion->llaves[izquierda].libre = 1;
	simulacion->llaves[izquierda].lista_en = lista_en;
	simulacion->llaves[derecha].libre = 1;
	simulacion->llaves[derecha].lista_en = lista_en;
	conceder_esperando(simulacion);
	pthread_cond_broadcast(&simulacion->cambio);
	pthread_mutex_unlock(&simulacion->cerrojo);
}
