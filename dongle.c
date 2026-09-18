
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:32:21 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/18 17:32:22 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <sys/time.h>

static long	siguiente_enfriamiento(t_simulacion *simulacion, long ahora)
{
	long	proximo;
	int		indice;

	proximo = 0;
	indice = 0;
	while (indice < simulacion->configuracion.programadores)
	{
		if (simulacion->llaves[indice].lista_en > ahora
			&& (!proximo || simulacion->llaves[indice].lista_en < proximo))
			proximo = simulacion->llaves[indice].lista_en;
		indice++;
	}
	return (proximo);
}

int	puede_tomar(t_programador *programador, long ahora)
{
	t_simulacion	*simulacion;
	int		izquierda;
	int		derecha;

	simulacion = programador->simulacion;
	izquierda = programador->id - 1;
	derecha = programador->id % simulacion->configuracion.programadores;
	if (!simulacion->llaves[izquierda].libre
		|| simulacion->llaves[izquierda].lista_en > ahora)
		return (0);
	if (izquierda != derecha && (!simulacion->llaves[derecha].libre
			|| simulacion->llaves[derecha].lista_en > ahora))
		return (0);
	return (1);
}

static void	esperar_turno(t_programador *programador)
{
	t_simulacion			*simulacion;
	struct timespec	limite;
	long				proximo;

	simulacion = programador->simulacion;
	while (!programador->concedido && !simulacion->detenida)
	{
		proximo = siguiente_enfriamiento(simulacion, tiempo_ms());
		if (proximo)
		{
			limite.tv_sec = proximo / 1000;
			limite.tv_nsec = (proximo % 1000) * 1000000L;
			pthread_cond_timedwait(&simulacion->cambio, &simulacion->cerrojo,
				&limite);
		}
		else
			pthread_cond_wait(&simulacion->cambio, &simulacion->cerrojo);
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
