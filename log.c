#include "codexion.h"
#include <stdio.h>

void	registrar_estado(t_programador *programador, char *estado)
{
	t_simulacion	*simulacion;

	simulacion = programador->simulacion;
	pthread_mutex_lock(&simulacion->cerrojo_impresion);
	pthread_mutex_lock(&simulacion->cerrojo);
	if (!simulacion->detenida)
		printf("%ld %d %s\n", tiempo_ms() - simulacion->inicio,
			programador->id, estado);
	pthread_mutex_unlock(&simulacion->cerrojo);
	pthread_mutex_unlock(&simulacion->cerrojo_impresion);
}

void	registrar_agotamiento(t_programador *programador)
{
	t_simulacion	*simulacion;

	simulacion = programador->simulacion;
	pthread_mutex_lock(&simulacion->cerrojo_impresion);
	printf("%ld %d se agotó\n", tiempo_ms() - simulacion->inicio, programador->id);
	pthread_mutex_unlock(&simulacion->cerrojo_impresion);
}

void	registrar_finalizacion(t_simulacion *simulacion)
{
	pthread_mutex_lock(&simulacion->cerrojo_impresion);
	printf("%ld simulación completada\n", tiempo_ms() - simulacion->inicio);
	pthread_mutex_unlock(&simulacion->cerrojo_impresion);
}
