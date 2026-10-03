/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:29:38 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/18 17:31:51 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "coders/codexion.h"

int	free_allocations(t_simulation *simulation)
{
	free(simulation->dongles);
	free(simulation->coders);
	free(simulation->queue);
	return (0);
}

int	cleanup_lock(t_simulation *simulation)
{
	pthread_mutex_destroy(&simulation->lock);
	free(simulation->dongles);
	free(simulation->coders);
	free(simulation->queue);
	return (0);
}

int	cleanup_print_lock(t_simulation *simulation)
{
	pthread_mutex_destroy(&simulation->print_lock);
	pthread_mutex_destroy(&simulation->lock);
	free(simulation->dongles);
	free(simulation->coders);
	free(simulation->queue);
	return (0);
}

int	cleanup_conditions(t_simulation *simulation, int count)
{
	while (count > 0)
	{
		count--;
		pthread_cond_destroy(&simulation->coders[count].ready);
	}
	pthread_cond_destroy(&simulation->changed);
	return (cleanup_print_lock(simulation));
}

void	destroy_simulation(t_simulation *simulation)
{
	cleanup_conditions(simulation, simulation->config.coders);
}
