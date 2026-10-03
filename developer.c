/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   developer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:32:15 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/18 17:59:42 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <errno.h>
#include <sys/time.h>

static void	deadline_from_now(struct timespec *deadline, long milliseconds)
{
	struct timeval	now;

	gettimeofday(&now, NULL);
	deadline->tv_sec = now.tv_sec + milliseconds / 1000;
	deadline->tv_nsec = now.tv_usec * 1000
		+ (milliseconds % 1000) * 1000000L;
	if (deadline->tv_nsec >= 1000000000L)
	{
		deadline->tv_sec++;
		deadline->tv_nsec -= 1000000000L;
	}
}

static int	sleep_or_stop(t_simulation *simulation, long milliseconds)
{
	struct timespec	deadline;
	int				result;
	int				active;

	deadline_from_now(&deadline, milliseconds);
	pthread_mutex_lock(&simulation->lock);
	result = 0;
	while (!simulation->stopped && result != ETIMEDOUT)
		result = pthread_cond_timedwait(&simulation->changed,
				&simulation->lock, &deadline);
	active = !simulation->stopped;
	pthread_mutex_unlock(&simulation->lock);
	return (active);
}

static int	compile(t_coder *coder)
{
	if (coder->simulation->config.coders == 1)
	{
		log_state(coder, "has taken a dongle");
		while (!is_stopped(coder->simulation))
			sleep_or_stop(coder->simulation,
				coder->simulation->config.time_to_burnout);
		return (0);
	}
	if (!take_dongles(coder))
		return (0);
	pthread_mutex_lock(&coder->simulation->lock);
	coder->last_compile_start = time_ms();
	pthread_mutex_unlock(&coder->simulation->lock);
	log_state(coder, "has taken a dongle");
	if (coder->simulation->config.coders > 1)
		log_state(coder, "has taken a dongle");
	log_state(coder, "is compiling");
	sleep_or_stop(coder->simulation,
		coder->simulation->config.time_to_compile);
	release_dongles(coder);
	return (!is_stopped(coder->simulation));
}

static int	complete_compile(t_coder *coder)
{
	t_simulation	*simulation;
	int				keep_running;

	simulation = coder->simulation;
	pthread_mutex_lock(&simulation->lock);
	coder->compiles++;
	if (coder->compiles == simulation->config.compiles_required)
		simulation->finished++;
	if (simulation->finished == simulation->config.coders)
	{
		simulation->stopped = 1;
		pthread_cond_broadcast(&simulation->changed);
	}
	keep_running = !simulation->stopped
		&& coder->compiles < simulation->config.compiles_required;
	pthread_mutex_unlock(&simulation->lock);
	return (keep_running);
}

void	*coder_routine(void *argument)
{
	t_coder	*coder;

	coder = argument;
	while (!is_stopped(coder->simulation))
	{
		if (!compile(coder) || !complete_compile(coder))
			break ;
		log_state(coder, "is debugging");
		if (!sleep_or_stop(coder->simulation,
				coder->simulation->config.time_to_debug))
			break ;
		log_state(coder, "is refactoring");
		if (!sleep_or_stop(coder->simulation,
				coder->simulation->config.time_to_refactor))
			break ;
	}
	return (NULL);
}
