#include <stdlib.h>
#include "codexion.h"

static int	init_shared_resource(struct s_shared_resource *shared_resource,
	struct s_config *config)
{
	shared_resource->config = *config;
	shared_resource->stopped = 0;
	shared_resource->start_time = get_time();
	if (pthread_mutex_init(&shared_resource->print_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&shared_resource->state_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&shared_resource->print_mutex);
		return (0);
	}
	return (1);
}

static void	init_developers(struct s_simulation *simulation_data)
{
	int	index;

	index = 0;
	while (index < simulation_data->shared_resource.config.number_of_coders)
	{
		simulation_data->developers[index].id = index + 1;
		simulation_data->developers[index].compiles_done = 0;
		simulation_data->developers[index].last_compile_start =
			simulation_data->shared_resource.start_time;
		simulation_data->developers[index].shared_resource =
			&simulation_data->shared_resource;
		index++;
	}
}

int	init_simulation(struct s_simulation *simulation_data,
	struct s_config *config)
{
	if (!init_shared_resource(&simulation_data->shared_resource, config))
		return (0);
	simulation_data->developers = malloc(sizeof(struct s_developer)
			* simulation_data->shared_resource.config.number_of_coders);
	if (simulation_data->developers == NULL)
	{
		pthread_mutex_destroy(&simulation_data->shared_resource.print_mutex);
		pthread_mutex_destroy(&simulation_data->shared_resource.state_mutex);
		return (0);
	}
	init_developers(simulation_data);
	return (1);
}

void	destroy_simulation(struct s_simulation *simulation_data)
{
	pthread_mutex_destroy(&simulation_data->shared_resource.print_mutex);
	pthread_mutex_destroy(&simulation_data->shared_resource.state_mutex);
	free(simulation_data->developers);
	simulation_data->developers = NULL;
}
