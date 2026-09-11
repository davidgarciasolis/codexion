#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>

struct s_config
{
	int	number_of_coders;
	int	time_to_burnout;
	int	time_to_compile;
	int	time_to_debug;
	int	time_to_refactor;
	int	number_of_compiles_required;
	int	dongle_cooldown;
	char	*scheduler;
};

struct s_dongle
{
	int				is_available;
	long			cooldown_until;
	pthread_mutex_t	mutex;
};

struct s_task
{
	struct s_developer	*developer;
	long				arrival_order;
	long				deadline;
};

struct s_shared_resource
{
	struct s_config	config;
	struct s_dongle	*dongles;
	struct s_task		*tasks;
	long			start_time;
	long			next_task_order;
	int				stopped;
	int				task_count;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	state_mutex;
	pthread_mutex_t	scheduler_mutex;
	pthread_cond_t	scheduler_cond;
};

struct s_developer
{
	int			id;
	int			compiles_done;
	long		last_compile_start;
	pthread_t	thread;
	struct s_shared_resource	*shared_resource;
};

struct s_simulation
{
	struct s_developer		*developers;
	struct s_shared_resource	shared_resource;
};

int		parseo(int argc, char **argv, struct s_config *config);
int		init_simulation(struct s_simulation *simulation_data,
			struct s_config *config);
void	destroy_simulation(struct s_simulation *simulation_data);
long	get_time(void);
void	*developer_routine(void *argument);
int		is_simulation_stopped(struct s_shared_resource *shared_resource);
int		take_dongles(struct s_developer *developer);
void	release_dongles(struct s_developer *developer);
void	log_status(struct s_developer *developer, char *status);

#endif
