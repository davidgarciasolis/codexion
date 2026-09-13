#include "codexion.h"
#include <unistd.h>

static int	dormir_o_detener(t_simulacion *simulacion, long milisegundos)
{
	while (milisegundos-- > 0)
	{
		if (esta_detenida(simulacion))
			return (0);
		usleep(1000);
	}
	return (!esta_detenida(simulacion));
}

static int	compilar(t_programador *programador)
{
	if (!tomar_llaves(programador))
		return (0);
	pthread_mutex_lock(&programador->simulacion->cerrojo);
	programador->ultima_compilacion = tiempo_ms();
	pthread_mutex_unlock(&programador->simulacion->cerrojo);
	registrar_estado(programador, "ha tomado una llave");
	if (programador->simulacion->configuracion.programadores > 1)
		registrar_estado(programador, "ha tomado una llave");
	registrar_estado(programador, "está compilando");
	dormir_o_detener(programador->simulacion, programador->simulacion->configuracion.compilar);
	liberar_llaves(programador);
	return (!esta_detenida(programador->simulacion));
}

static int	completado(t_programador *programador)
{
	t_simulacion	*simulacion;

	simulacion = programador->simulacion;
	pthread_mutex_lock(&simulacion->cerrojo);
	programador->compilaciones++;
	if (programador->compilaciones == simulacion->configuracion.requeridos)
		simulacion->terminados++;
	if (simulacion->terminados == simulacion->configuracion.programadores)
	{
		simulacion->detenida = 1;
		pthread_cond_broadcast(&simulacion->cambio);
	}
	pthread_mutex_unlock(&simulacion->cerrojo);
	return (!esta_detenida(simulacion));
}

void	*rutina_programador(void *argumento)
{
	t_programador	*programador;

	programador = argumento;
	while (!esta_detenida(programador->simulacion))
	{
		if (!compilar(programador) || !completado(programador))
			break ;
		registrar_estado(programador, "está depurando");
		if (!dormir_o_detener(programador->simulacion, programador->simulacion->configuracion.depurar))
			break ;
		registrar_estado(programador, "está refactorizando");
		if (!dormir_o_detener(programador->simulacion, programador->simulacion->configuracion.refactorizar))
			break ;
	}
	return (NULL);
}
