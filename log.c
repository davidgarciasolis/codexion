/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:32:26 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/18 17:56:29 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>

void	log_state(t_coder *coder, char *state)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->print_lock);
	pthread_mutex_lock(&simulation->lock);
	if (!simulation->stopped)
		printf("%ld %d %s\n", time_ms() - simulation->start_time,
			coder->id, state);
	pthread_mutex_unlock(&simulation->lock);
	pthread_mutex_unlock(&simulation->print_lock);
}

void	log_burnout(t_coder *coder)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->print_lock);
	printf("%ld %d burned out\n",
		time_ms() - simulation->start_time, coder->id);
	pthread_mutex_unlock(&simulation->print_lock);
}
