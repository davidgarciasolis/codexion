/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:32:31 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/18 18:02:42 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>

static int	iniciar_programadores(t_simulacion *simulacion)
{
	int	indice;

	indice = 0;
	while (indice < simulacion->configuracion.programadores)
	{
		if (pthread_create(&simulacion->programadores[indice].hilo, NULL,
				rutina_programador,
				&simulacion->programadores[indice]))
		{
			detener_simulacion(simulacion);
			break ;
		}
		indice++;
	}
	return (indice);
}

static int	iniciar_monitor(t_simulacion *simulacion, pthread_t *monitor)
{
	if (pthread_create(monitor, NULL, rutina_monitor, simulacion))
	{
		printf("Error: no se pudo crear el hilo monitor.\n");
		detener_simulacion(simulacion);
		return (0);
	}
	return (1);
}

int	main(int argc, char **argv)
{
	t_configuracion	configuracion;
	t_simulacion	simulacion;
	pthread_t		monitor;
	int				indice;
	int				monitor_creado;

	if (!parsear(argc, argv, &configuracion)
		|| !inicializar_simulacion(&simulacion, &configuracion))
		return (1);
	indice = iniciar_programadores(&simulacion);
	monitor_creado = iniciar_monitor(&simulacion, &monitor);
	while (indice > 0)
	{
		indice--;
		pthread_join(simulacion.programadores[indice].hilo, NULL);
	}
	detener_simulacion(&simulacion);
	if (monitor_creado)
		pthread_join(monitor, NULL);
	destruir_simulacion(&simulacion);
	return (!monitor_creado);
}
