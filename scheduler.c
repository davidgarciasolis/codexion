/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:33:54 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/18 17:58:41 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

static void	grant(t_simulation *simulation, int index)
{
	int	left;
	int	right;

	left = simulation->queue[index]->id - 1;
	right = simulation->queue[index]->id
		% simulation->config.coders;
	simulation->dongles[left].available = 0;
	simulation->dongles[right].available = 0;
	simulation->queue[index]->waiting = 0;
	simulation->queue[index]->granted = 1;
	pthread_cond_broadcast(&simulation->queue[index]->ready);
	queue_remove(simulation, index);
}

static int	best_available(t_simulation *simulation, long now)
{
	int	index;
	int	best;

	index = 0;
	best = -1;
	while (index < simulation->queue_size)
	{
		if (can_take(simulation->queue[index], now))
		{
			if (best == -1)
				best = index;
			else if (has_priority(simulation, simulation->queue[index],
					simulation->queue[best]))
				best = index;
		}
		index++;
	}
	return (best);
}

void	grant_waiting(t_simulation *simulation)
{
	int		best;
	long	now;

	now = time_ms();
	best = best_available(simulation, now);
	while (best != -1)
	{
		grant(simulation, best);
		best = best_available(simulation, now);
	}
}
