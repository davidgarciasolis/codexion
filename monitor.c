/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:32:36 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/18 17:32:37 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"
#include <unistd.h>

static t_coder	*find_burned_out(t_simulation *simulation)
{
	int	index;

	index = 0;
	while (index < simulation->config.coders)
	{
		if (simulation->coders[index].compiles
			< simulation->config.compiles_required
			&& time_ms() > simulation->coders[index].last_compile_start
			+ simulation->config.time_to_burnout)
		{
			simulation->stopped = 1;
			wake_coders(simulation);
			return (&simulation->coders[index]);
		}
		index++;
	}
	return (NULL);
}

void	*monitor_routine(void *argument)
{
	t_simulation	*simulation;
	t_coder			*burned_out;

	simulation = argument;
	while (!is_stopped(simulation))
	{
		pthread_mutex_lock(&simulation->lock);
		burned_out = find_burned_out(simulation);
		pthread_mutex_unlock(&simulation->lock);
		if (burned_out)
		{
			log_burnout(burned_out);
			break ;
		}
		usleep(1000);
	}
	return (NULL);
}
