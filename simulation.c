#include "codexion.h"
#include <stdlib.h>

static int	asignar(t_simulacion *simulacion, t_configuracion *configuracion)
{
	simulacion->llaves = malloc(sizeof(*simulacion->llaves) * configuracion->programadores);
	simulacion->programadores = malloc(sizeof(*simulacion->programadores) * configuracion->programadores);
	simulacion->cola = malloc(sizeof(*simulacion->cola) * configuracion->programadores);
	if (!simulacion->llaves || !simulacion->programadores || !simulacion->cola)
	{
		free(simulacion->llaves);
		free(simulacion->programadores);
		free(simulacion->cola);
		return (0);
	}
	return (1);
}

static void	inicializar_programadores(t_simulacion *simulacion)
{
	int	indice;

	indice = 0;
	while (indice < simulacion->configuracion.programadores)
	{
		simulacion->llaves[indice].libre = 1;
		simulacion->programadores[indice].id = indice + 1;
		simulacion->programadores[indice].ultima_compilacion = simulacion->inicio;
		simulacion->programadores[indice].simulacion = simulacion;
		indice++;
	}
}

int	inicializar_simulacion(t_simulacion *simulacion, t_configuracion *configuracion)
{
	*simulacion = (t_simulacion){0};
	simulacion->configuracion = *configuracion;
	simulacion->inicio = tiempo_ms();
	if (!asignar(simulacion, configuracion))
		return (0);
	if (pthread_mutex_init(&simulacion->cerrojo, NULL)
		|| pthread_mutex_init(&simulacion->cerrojo_impresion, NULL)
		|| pthread_cond_init(&simulacion->cambio, NULL))
		return (0);
	inicializar_programadores(simulacion);
	return (1);
}
