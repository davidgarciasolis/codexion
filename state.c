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

#include "codexion.h"

int	is_stopped(t_simulation *simulation)
{
	int	stopped;

	pthread_mutex_lock(&simulation->lock);
	stopped = simulation->stopped;
	pthread_mutex_unlock(&simulation->lock);
	return (stopped);
}

void	stop_simulation(t_simulation *simulation)
{
	pthread_mutex_lock(&simulation->lock);
	simulation->stopped = 1;
	pthread_cond_broadcast(&simulation->changed);
	pthread_mutex_unlock(&simulation->lock);
}
