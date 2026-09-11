#include <stdlib.h>
#include "codexion.h"

static void	destruir_tareas(struct s_recurso_compartido *recurso_compartido)
{
	free(recurso_compartido->tareas);
	recurso_compartido->tareas = NULL;
	recurso_compartido->cantidad_tareas = 0;
}

static int	iniciar_tareas(struct s_recurso_compartido *recurso_compartido)
{
	int	indice;

	recurso_compartido->tareas = malloc(sizeof(struct s_tarea)
			* recurso_compartido->configuracion.cantidad_desarrolladores);
	if (recurso_compartido->tareas == NULL)
		return (0);
	indice = 0;
	while (indice < recurso_compartido->configuracion.cantidad_desarrolladores)
	{
		recurso_compartido->tareas[indice].desarrollador = NULL;
		recurso_compartido->tareas[indice].orden_llegada = 0;
		recurso_compartido->tareas[indice].fecha_limite = 0;
		indice++;
	}
	recurso_compartido->cantidad_tareas = 0;
	recurso_compartido->siguiente_orden_tarea = 0;
	return (1);
}

static void	destruir_dongles(struct s_recurso_compartido *recurso_compartido)
{
	int	indice;

	if (recurso_compartido->dongles == NULL)
		return ;
	indice = 0;
	while (indice < recurso_compartido->configuracion.cantidad_desarrolladores)
	{
		pthread_mutex_destroy(&recurso_compartido->dongles[indice].mutex);
		indice++;
	}
	free(recurso_compartido->dongles);
	recurso_compartido->dongles = NULL;
}

static int	iniciar_dongles(struct s_recurso_compartido *recurso_compartido)
{
	int	indice;

	recurso_compartido->dongles = malloc(sizeof(struct s_dongle)
			* recurso_compartido->configuracion.cantidad_desarrolladores);
	if (recurso_compartido->dongles == NULL)
		return (0);
	indice = 0;
	while (indice < recurso_compartido->configuracion.cantidad_desarrolladores)
	{
		recurso_compartido->dongles[indice].disponible = 1;
		recurso_compartido->dongles[indice].enfriamiento_hasta = 0;
		if (pthread_mutex_init(&recurso_compartido->dongles[indice].mutex, NULL) != 0)
		{
			while (indice > 0)
				pthread_mutex_destroy(&recurso_compartido->dongles[--indice].mutex);
			free(recurso_compartido->dongles);
			recurso_compartido->dongles = NULL;
			return (0);
		}
		indice++;
	}
	return (1);
}

static int	iniciar_recurso_compartido(struct s_recurso_compartido
	*recurso_compartido, struct s_configuracion *configuracion)
{
	recurso_compartido->configuracion = *configuracion;
	recurso_compartido->detenida = 0;
	recurso_compartido->dongles = NULL;
	recurso_compartido->tareas = NULL;
	recurso_compartido->cantidad_tareas = 0;
	recurso_compartido->siguiente_orden_tarea = 0;
	recurso_compartido->inicio = obtener_tiempo();
	if (pthread_mutex_init(&recurso_compartido->mutex_impresion, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&recurso_compartido->mutex_estado, NULL) != 0)
	{
		pthread_mutex_destroy(&recurso_compartido->mutex_impresion);
		return (0);
	}
	if (pthread_mutex_init(&recurso_compartido->mutex_planificador, NULL) != 0)
	{
		pthread_mutex_destroy(&recurso_compartido->mutex_estado);
		pthread_mutex_destroy(&recurso_compartido->mutex_impresion);
		return (0);
	}
	if (pthread_cond_init(&recurso_compartido->condicion_planificador, NULL) != 0)
	{
		pthread_mutex_destroy(&recurso_compartido->mutex_planificador);
		pthread_mutex_destroy(&recurso_compartido->mutex_estado);
		pthread_mutex_destroy(&recurso_compartido->mutex_impresion);
		return (0);
	}
	if (!iniciar_dongles(recurso_compartido))
	{
		pthread_cond_destroy(&recurso_compartido->condicion_planificador);
		pthread_mutex_destroy(&recurso_compartido->mutex_planificador);
		pthread_mutex_destroy(&recurso_compartido->mutex_estado);
		pthread_mutex_destroy(&recurso_compartido->mutex_impresion);
		return (0);
	}
	if (recurso_compartido->configuracion.cantidad_desarrolladores > 1
		&& !iniciar_tareas(recurso_compartido))
	{
		destruir_dongles(recurso_compartido);
		pthread_cond_destroy(&recurso_compartido->condicion_planificador);
		pthread_mutex_destroy(&recurso_compartido->mutex_planificador);
		pthread_mutex_destroy(&recurso_compartido->mutex_estado);
		pthread_mutex_destroy(&recurso_compartido->mutex_impresion);
		return (0);
	}
	return (1);
}

static void	iniciar_desarrolladores(struct s_simulacion *datos_simulacion)
{
	int	index;

	index = 0;
	while (index < datos_simulacion->recurso_compartido.configuracion.cantidad_desarrolladores)
	{
		datos_simulacion->desarrolladores[index].id = index + 1;
		datos_simulacion->desarrolladores[index].compilaciones_hechas = 0;
		datos_simulacion->desarrolladores[index].inicio_ultima_compilacion =
			datos_simulacion->recurso_compartido.inicio;
		datos_simulacion->desarrolladores[index].recurso_compartido =
			&datos_simulacion->recurso_compartido;
		index++;
	}
}

int	iniciar_simulacion(struct s_simulacion *datos_simulacion,
	struct s_configuracion *configuracion)
{
	if (!iniciar_recurso_compartido(&datos_simulacion->recurso_compartido,
			configuracion))
		return (0);
	datos_simulacion->desarrolladores = malloc(sizeof(struct s_desarrollador)
			* datos_simulacion->recurso_compartido.configuracion.cantidad_desarrolladores);
	if (datos_simulacion->desarrolladores == NULL)
	{
		destruir_tareas(&datos_simulacion->recurso_compartido);
		destruir_dongles(&datos_simulacion->recurso_compartido);
		pthread_cond_destroy(&datos_simulacion->recurso_compartido.condicion_planificador);
		pthread_mutex_destroy(&datos_simulacion->recurso_compartido.mutex_planificador);
		pthread_mutex_destroy(&datos_simulacion->recurso_compartido.mutex_impresion);
		pthread_mutex_destroy(&datos_simulacion->recurso_compartido.mutex_estado);
		return (0);
	}
	iniciar_desarrolladores(datos_simulacion);
	return (1);
}

void	destruir_simulacion(struct s_simulacion *datos_simulacion)
{
	destruir_tareas(&datos_simulacion->recurso_compartido);
	destruir_dongles(&datos_simulacion->recurso_compartido);
	pthread_cond_destroy(&datos_simulacion->recurso_compartido.condicion_planificador);
	pthread_mutex_destroy(&datos_simulacion->recurso_compartido.mutex_planificador);
	pthread_mutex_destroy(&datos_simulacion->recurso_compartido.mutex_impresion);
	pthread_mutex_destroy(&datos_simulacion->recurso_compartido.mutex_estado);
	free(datos_simulacion->desarrolladores);
	datos_simulacion->desarrolladores = NULL;
}
