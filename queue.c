/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:32:10 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/18 17:56:01 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"

int	has_priority(t_simulation *simulation, t_coder *first,
		t_coder *second)
{
	long	first_deadline;
	long	second_deadline;

	first_deadline = first->last_compile_start
		+ simulation->config.time_to_burnout;
	second_deadline = second->last_compile_start
		+ simulation->config.time_to_burnout;
	if (simulation->config.edf)
		return (first_deadline < second_deadline || (first_deadline
				== second_deadline && first->id < second->id));
	return (first->turn < second->turn);
}

static void	swap(t_coder **first, t_coder **second)
{
	t_coder	*temp;

	temp = *first;
	*first = *second;
	*second = temp;
}

void	queue_insert(t_simulation *simulation, t_coder *coder)
{
	int	index;

	index = simulation->queue_size++;
	simulation->queue[index] = coder;
	while (index && has_priority(simulation, simulation->queue[index],
			simulation->queue[(index - 1) / 2]))
	{
		swap(&simulation->queue[index],
			&simulation->queue[(index - 1) / 2]);
		index = (index - 1) / 2;
	}
}

void	queue_remove(t_simulation *simulation, int index)
{
	int	child;

	simulation->queue[index] = simulation->queue[--simulation->queue_size];
	while (index && has_priority(simulation, simulation->queue[index],
			simulation->queue[(index - 1) / 2]))
	{
		swap(&simulation->queue[index],
			&simulation->queue[(index - 1) / 2]);
		index = (index - 1) / 2;
	}
	while (index * 2 + 1 < simulation->queue_size)
	{
		child = index * 2 + 1;
		if (child + 1 < simulation->queue_size
			&& has_priority(simulation, simulation->queue[child + 1],
				simulation->queue[child]))
			child++;
		if (!has_priority(simulation, simulation->queue[child],
				simulation->queue[index]))
			break ;
		swap(&simulation->queue[index], &simulation->queue[child]);
		index = child;
	}
}
