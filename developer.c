#include "codexion.h"
#include <unistd.h>

int	simulacion_detenida(struct s_recurso_compartido *recurso_compartido)
{
	int	detenida;

	pthread_mutex_lock(&recurso_compartido->mutex_recurso_compartido);
	detenida = recurso_compartido->detenida;
	pthread_mutex_unlock(&recurso_compartido->mutex_recurso_compartido);
	return (detenida);
}

static void	iniciar_compilacion(struct s_desarrollador *desarrollador)
{
	pthread_mutex_lock(&desarrollador->recurso_compartido
		->mutex_recurso_compartido);
	desarrollador->inicio_ultima_compilacion = obtener_tiempo();
	pthread_mutex_unlock(&desarrollador->recurso_compartido
		->mutex_recurso_compartido);
}

static int	registrar_compilacion(struct s_desarrollador *desarrollador)
{
	int	terminado;

	pthread_mutex_lock(&desarrollador->recurso_compartido
		->mutex_recurso_compartido);
	desarrollador->compilaciones_hechas++;
	terminado = (desarrollador->compilaciones_hechas
		>= desarrollador->recurso_compartido->configuracion
		.compilaciones_requeridas);
	pthread_mutex_unlock(&desarrollador->recurso_compartido
		->mutex_recurso_compartido);
	return (!terminado);
}

static int	compilar(struct s_desarrollador *desarrollador)
{
	if (!tomar_dongles(desarrollador))
		return (0);
	iniciar_compilacion(desarrollador);
	registrar_estado(desarrollador, "ha tomado un dongle");
	if (desarrollador->recurso_compartido->configuracion
		.cantidad_desarrolladores > 1)
		registrar_estado(desarrollador, "ha tomado un dongle");
	registrar_estado(desarrollador, "está compilando");
	usleep(desarrollador->recurso_compartido->configuracion.tiempo_compilar
		* 1000);
	liberar_dongles(desarrollador);
	return (!simulacion_detenida(desarrollador->recurso_compartido));
}

void	*rutina_desarrollador(void *argumento)
{
	struct s_desarrollador	*desarrollador;

	desarrollador = (struct s_desarrollador *)argumento;
	while (!simulacion_detenida(desarrollador->recurso_compartido))
	{
		if (!compilar(desarrollador) || !registrar_compilacion(desarrollador))
			break ;
		registrar_estado(desarrollador, "está depurando");
		usleep(desarrollador->recurso_compartido->configuracion.tiempo_depurar
			* 1000);
		if (simulacion_detenida(desarrollador->recurso_compartido))
			break ;
		registrar_estado(desarrollador, "está refactorizando");
		usleep(desarrollador->recurso_compartido->configuracion
			.tiempo_refactorizar * 1000);
	}
	return (NULL);
}
