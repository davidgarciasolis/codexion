/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   state.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:34:07 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/18 17:34:08 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

int	is_stopped(t_simulation *simulation)
{
	int	stopped;

	pthread_mutex_lock(&simulation->lock);
	stopped = simulation->stopped;
	pthread_mutex_unlock(&simulation->lock);
	return (stopped);
}

void	wake_neighbors(t_coder *coder)
{
	t_simulation	*simulation;
	int				left;
	int				right;

	simulation = coder->simulation;
	left = (coder->id + simulation->config.coders - 2)
		% simulation->config.coders;
	right = coder->id % simulation->config.coders;
	if (simulation->coders[left].waiting)
		pthread_cond_broadcast(&simulation->coders[left].ready);
	if (right != left && simulation->coders[right].waiting)
		pthread_cond_broadcast(&simulation->coders[right].ready);
}

void	wake_coders(t_simulation *simulation)
{
	int	index;

	index = 0;
	while (index < simulation->config.coders)
	{
		pthread_cond_broadcast(&simulation->coders[index].ready);
		index++;
	}
	pthread_cond_broadcast(&simulation->changed);
}

void	stop_simulation(t_simulation *simulation)
{
	pthread_mutex_lock(&simulation->lock);
	simulation->stopped = 1;
	wake_coders(simulation);
	pthread_mutex_unlock(&simulation->lock);
}
