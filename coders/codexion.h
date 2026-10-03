/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:32:01 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/26 16:43:12 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>

typedef struct s_config
{
	int		coders;
	long	time_to_burnout;
	long	time_to_compile;
	long	time_to_debug;
	long	time_to_refactor;
	int		compiles_required;
	long	dongle_cooldown;
	int		edf;
}	t_config;

typedef struct s_dongle
{
	int		available;
	long	ready_at;
}	t_dongle;

typedef struct s_coder	t_coder;

typedef struct s_simulation
{
	t_config		config;
	t_dongle		*dongles;
	t_coder			*coders;
	t_coder			**queue;
	int				queue_size;
	long			next_turn;
	long			start_time;
	int				stopped;
	int				finished;
	pthread_mutex_t	lock;
	pthread_mutex_t	print_lock;
	pthread_cond_t	changed;
}	t_simulation;

struct s_coder
{
	int				id;
	int				compiles;
	int				waiting;
	int				granted;
	long			last_compile_start;
	long			turn;
	pthread_t		thread;
	t_simulation	*simulation;
};

long	time_ms(void);
int		parse_args(int argc, char **argv, t_config *config);
int		init_simulation(t_simulation *simulation,
			t_config *config);
int		free_allocations(t_simulation *simulation);
int		cleanup_lock(t_simulation *simulation);
int		cleanup_print_lock(t_simulation *simulation);
void	destroy_simulation(t_simulation *simulation);
void	*coder_routine(void *argument);
void	*monitor_routine(void *argument);
void	log_state(t_coder *coder, char *state);
void	log_burnout(t_coder *coder);
void	stop_simulation(t_simulation *simulation);
void	queue_insert(t_simulation *simulation, t_coder *coder);
void	queue_remove(t_simulation *simulation, int index);
void	grant_waiting(t_simulation *simulation);
int		can_take(t_coder *coder, long now);
int		has_priority(t_simulation *simulation, t_coder *first,
			t_coder *second);
int		take_dongles(t_coder *coder);
void	release_dongles(t_coder *coder);
int		is_stopped(t_simulation *simulation);

#endif
