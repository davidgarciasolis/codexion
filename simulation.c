/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:34:01 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/26 13:26:55 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdlib.h>
#include <string.h>

static int	asignar(t_simulacion *simulacion, t_configuracion *configuracion)
{
	simulacion->llaves = calloc(configuracion->programadores,
			sizeof(*simulacion->llaves));
	simulacion->programadores = calloc(configuracion->programadores,
			sizeof(*simulacion->programadores));
	simulacion->cola = calloc(configuracion->programadores,
			sizeof(*simulacion->cola));
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
		simulacion->programadores[indice].ultima_compilacion
			= simulacion->inicio;
		simulacion->programadores[indice].simulacion = simulacion;
		indice++;
	}
}

int	inicializar_simulacion(t_simulacion *simulacion,
		t_configuracion *configuracion)
{
	*simulacion = (t_simulacion){0};
	simulacion->configuracion = *configuracion;
	simulacion->inicio = tiempo_ms();
	if (!asignar(simulacion, configuracion))
		return (0);
	if (pthread_mutex_init(&simulacion->cerrojo, NULL))
		return (free(simulacion->cola), free(simulacion->programadores),
			free(simulacion->llaves), 0);
	if (pthread_mutex_init(&simulacion->cerrojo_impresion, NULL))
	{
		return (pthread_mutex_destroy(&simulacion->cerrojo),
			free(simulacion->cola), free(simulacion->programadores),
			free(simulacion->llaves), 0);
	}
	if (pthread_cond_init(&simulacion->cambio, NULL))
	{
		return (pthread_mutex_destroy(&simulacion->cerrojo_impresion),
			pthread_mutex_destroy(&simulacion->cerrojo), free(simulacion->cola),
			free(simulacion->programadores), free(simulacion->llaves), 0);
	}
	inicializar_programadores(simulacion);
	return (1);
}
