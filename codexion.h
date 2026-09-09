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

struct s_shared_resource
{
	struct s_config	config;
	long			start_time;
	int				stopped;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	state_mutex;
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

#endif
