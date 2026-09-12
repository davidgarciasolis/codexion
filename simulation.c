#include <stdlib.h>
#include "codexion.h"

static int	iniciar_recurso_compartido(struct s_recurso_compartido
	*recurso_compartido, struct s_configuracion *configuracion)
{
	recurso_compartido->configuracion = *configuracion;
	recurso_compartido->detenida = 0;
	recurso_compartido->inicio = obtener_tiempo();
	if (pthread_mutex_init(&recurso_compartido->mutex_impresion, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&recurso_compartido->mutex_recurso_compartido,
			NULL) != 0)
	{
		pthread_mutex_destroy(&recurso_compartido->mutex_impresion);
		return (0);
	}
	return (1);
}

static void	iniciar_desarrolladores(struct s_simulacion *datos_simulacion)
{
	int	index;

	index = 0;
	while (index < datos_simulacion->recurso_compartido.configuracion
		.cantidad_desarrolladores)
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
			* datos_simulacion->recurso_compartido.configuracion
			.cantidad_desarrolladores);
	if (datos_simulacion->desarrolladores == NULL)
	{
		pthread_mutex_destroy(&datos_simulacion->recurso_compartido.mutex_impresion);
		pthread_mutex_destroy(&datos_simulacion->recurso_compartido
			.mutex_recurso_compartido);
		return (0);
	}
	iniciar_desarrolladores(datos_simulacion);
	return (1);
}

void	destruir_simulacion(struct s_simulacion *datos_simulacion)
{
	pthread_mutex_destroy(&datos_simulacion->recurso_compartido.mutex_impresion);
	pthread_mutex_destroy(&datos_simulacion->recurso_compartido
		.mutex_recurso_compartido);
	free(datos_simulacion->desarrolladores);
	datos_simulacion->desarrolladores = NULL;
}
