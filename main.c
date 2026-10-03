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

#include "coders/codexion.h"
#include <stdio.h>

static int	start_coders(t_simulation *simulation)
{
	int	index;

	index = 0;
	while (index < simulation->config.coders)
	{
		if (pthread_create(&simulation->coders[index].thread, NULL,
				coder_routine,
				&simulation->coders[index]))
		{
			stop_simulation(simulation);
			break ;
		}
		index++;
	}
	return (index);
}

static int	start_monitor(t_simulation *simulation, pthread_t *monitor)
{
	if (pthread_create(monitor, NULL, monitor_routine, simulation))
	{
		printf("Error: could not create the monitor thread.\n");
		stop_simulation(simulation);
		return (0);
	}
	return (1);
}

int	main(int argc, char **argv)
{
	t_config		config;
	t_simulation	simulation;
	pthread_t		monitor;
	int				index;
	int				monitor_created;

	if (!parse_args(argc, argv, &config)
		|| !init_simulation(&simulation, &config))
		return (1);
	index = start_coders(&simulation);
	monitor_created = start_monitor(&simulation, &monitor);
	while (index > 0)
	{
		index--;
		pthread_join(simulation.coders[index].thread, NULL);
	}
	stop_simulation(&simulation);
	if (monitor_created)
		pthread_join(monitor, NULL);
	destroy_simulation(&simulation);
	return (!monitor_created);
}
