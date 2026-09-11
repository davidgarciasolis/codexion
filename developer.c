#include "codexion.h"
#include <unistd.h>

int	is_simulation_stopped(struct s_shared_resource *shared_resource)
{
	int	stopped;

	pthread_mutex_lock(&shared_resource->state_mutex);
	stopped = shared_resource->stopped;
	pthread_mutex_unlock(&shared_resource->state_mutex);
	return (stopped);
}

void	*developer_routine(void *argument)
{
	struct s_developer	*developer;
	int				finished;

	developer = (struct s_developer *)argument;
	if (developer == NULL)
		return (NULL);
	while (!is_simulation_stopped(developer->shared_resource))
	{
		if (!take_dongles(developer))
			break ;
		pthread_mutex_lock(&developer->shared_resource->state_mutex);
		developer->last_compile_start = get_time();
		pthread_mutex_unlock(&developer->shared_resource->state_mutex);
		log_status(developer, "has taken a dongle");
		log_status(developer, "has taken a dongle");
		log_status(developer, "is compiling");
		usleep(developer->shared_resource->config.time_to_compile * 1000);
		release_dongles(developer);
		if (is_simulation_stopped(developer->shared_resource))
			break ;
		pthread_mutex_lock(&developer->shared_resource->state_mutex);
		developer->compiles_done++;
		finished = (developer->compiles_done
			>= developer->shared_resource->config.number_of_compiles_required);
		pthread_mutex_unlock(&developer->shared_resource->state_mutex);
		if (finished)
			break ;
		log_status(developer, "is debugging");
		usleep(developer->shared_resource->config.time_to_debug * 1000);
		if (is_simulation_stopped(developer->shared_resource))
			break ;
		log_status(developer, "is refactoring");
		usleep(developer->shared_resource->config.time_to_refactor * 1000);
	}
	return (NULL);
}
