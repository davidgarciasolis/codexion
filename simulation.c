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

#include "coders/codexion.h"
#include <stdlib.h>
#include <string.h>

static void	*ft_calloc(size_t size)
{
	void	*memory;

	memory = malloc(size);
	if (memory)
		memset(memory, 0, size);
	return (memory);
}

static int	allocate(t_simulation *simulation, t_config *config)
{
	simulation->dongles = ft_calloc(config->coders
			* sizeof(*simulation->dongles));
	simulation->coders = ft_calloc(config->coders
			* sizeof(*simulation->coders));
	simulation->queue = ft_calloc(config->coders
			* sizeof(*simulation->queue));
	if (!simulation->dongles || !simulation->coders || !simulation->queue)
		return (free_allocations(simulation));
	return (1);
}

static void	init_coders(t_simulation *simulation)
{
	int	index;

	index = 0;
	while (index < simulation->config.coders)
	{
		simulation->dongles[index].available = 1;
		simulation->coders[index].id = index + 1;
		simulation->coders[index].last_compile_start
			= simulation->start_time;
		simulation->coders[index].simulation = simulation;
		index++;
	}
}

int	init_simulation(t_simulation *simulation,
		t_config *config)
{
	memset(simulation, 0, sizeof(*simulation));
	simulation->config = *config;
	simulation->start_time = time_ms();
	if (!allocate(simulation, config))
		return (0);
	if (pthread_mutex_init(&simulation->lock, NULL))
		return (free_allocations(simulation));
	if (pthread_mutex_init(&simulation->print_lock, NULL))
		return (cleanup_lock(simulation));
	if (pthread_cond_init(&simulation->changed, NULL))
		return (cleanup_print_lock(simulation));
	init_coders(simulation);
	return (1);
}
