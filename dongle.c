/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:32:21 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/27 13:01:00 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <sys/time.h>

static long	next_cooldown(t_simulation *simulation, long now)
{
	long	next;
	int		index;

	next = 0;
	index = 0;
	while (index < simulation->config.coders)
	{
		if (simulation->dongles[index].ready_at > now
			&& (!next || simulation->dongles[index].ready_at < next))
			next = simulation->dongles[index].ready_at;
		index++;
	}
	return (next);
}

int	can_take(t_coder *coder, long now)
{
	t_simulation	*simulation;
	int				left;
	int				right;

	simulation = coder->simulation;
	left = coder->id - 1;
	right = coder->id % simulation->config.coders;
	if (!simulation->dongles[left].available
		|| simulation->dongles[left].ready_at > now)
		return (0);
	if (!simulation->dongles[right].available
		|| simulation->dongles[right].ready_at > now)
		return (0);
	return (1);
}

static void	wait_for_turn(t_coder *coder)
{
	t_simulation	*simulation;
	struct timespec	deadline;
	long			next;

	simulation = coder->simulation;
	while (!coder->granted && !simulation->stopped)
	{
		next = next_cooldown(simulation, time_ms());
		if (next)
		{
			deadline.tv_sec = next / 1000;
			deadline.tv_nsec = (next % 1000) * 1000000L;
			pthread_cond_timedwait(&simulation->changed, &simulation->lock,
				&deadline);
		}
		else
			pthread_cond_wait(&simulation->changed, &simulation->lock);
		grant_waiting(simulation);
	}
}

int	take_dongles(t_coder *coder)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->lock);
	coder->turn = simulation->next_turn++;
	coder->waiting = 1;
	coder->granted = 0;
	queue_insert(simulation, coder);
	grant_waiting(simulation);
	wait_for_turn(coder);
	pthread_mutex_unlock(&simulation->lock);
	return (coder->granted);
}

void	release_dongles(t_coder *coder)
{
	t_simulation	*simulation;
	int				left;
	int				right;
	long			ready_at;

	simulation = coder->simulation;
	left = coder->id - 1;
	right = coder->id % simulation->config.coders;
	pthread_mutex_lock(&simulation->lock);
	ready_at = time_ms() + simulation->config.dongle_cooldown;
	simulation->dongles[left].available = 1;
	simulation->dongles[left].ready_at = ready_at;
	simulation->dongles[right].available = 1;
	simulation->dongles[right].ready_at = ready_at;
	grant_waiting(simulation);
	pthread_cond_broadcast(&simulation->changed);
	pthread_mutex_unlock(&simulation->lock);
}
