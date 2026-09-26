/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   developer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:32:15 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/18 17:59:42 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <errno.h>
#include <sys/time.h>

static void	limite_desde_ahora(struct timespec *limite, long milisegundos)
{
	struct timeval	ahora;

	gettimeofday(&ahora, NULL);
	limite->tv_sec = ahora.tv_sec + milisegundos / 1000;
	limite->tv_nsec = ahora.tv_usec * 1000
		+ (milisegundos % 1000) * 1000000L;
	if (limite->tv_nsec >= 1000000000L)
	{
		limite->tv_sec++;
		limite->tv_nsec -= 1000000000L;
	}
}

static int	dormir_o_detener(t_simulacion *simulacion, long milisegundos)
{
	struct timespec	limite;
	int				resultado;
	int				activa;

	limite_desde_ahora(&limite, milisegundos);
	pthread_mutex_lock(&simulacion->cerrojo);
	resultado = 0;
	while (!simulacion->detenida && resultado != ETIMEDOUT)
		resultado = pthread_cond_timedwait(&simulacion->cambio,
				&simulacion->cerrojo, &limite);
	activa = !simulacion->detenida;
	pthread_mutex_unlock(&simulacion->cerrojo);
	return (activa);
}

static int	compilar(t_programador *programador)
{
	if (programador->simulacion->configuracion.programadores == 1)
	{
		registrar_estado(programador, "ha tomado una llave");
		while (!esta_detenida(programador->simulacion))
			dormir_o_detener(programador->simulacion,
				programador->simulacion->configuracion.agotamiento);
		return (0);
	}
	if (!tomar_llaves(programador))
		return (0);
	pthread_mutex_lock(&programador->simulacion->cerrojo);
	programador->ultima_compilacion = tiempo_ms();
	pthread_mutex_unlock(&programador->simulacion->cerrojo);
	registrar_estado(programador, "ha tomado una llave");
	if (programador->simulacion->configuracion.programadores > 1)
		registrar_estado(programador, "ha tomado una llave");
	registrar_estado(programador, "está compilando");
	dormir_o_detener(programador->simulacion,
		programador->simulacion->configuracion.compilar);
	liberar_llaves(programador);
	return (!esta_detenida(programador->simulacion));
}

static int	completado(t_programador *programador)
{
	t_simulacion	*simulacion;
	int				continuar;

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
	continuar = !simulacion->detenida
		&& programador->compilaciones < simulacion->configuracion.requeridos;
	pthread_mutex_unlock(&simulacion->cerrojo);
	return (continuar);
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
		if (!dormir_o_detener(programador->simulacion,
				programador->simulacion->configuracion.depurar))
			break ;
		registrar_estado(programador, "está refactorizando");
		if (!dormir_o_detener(programador->simulacion,
				programador->simulacion->configuracion.refactorizar))
			break ;
	}
	return (NULL);
}
