#include <stdlib.h>
#include "codexion.h"

static void	destroy_tasks(struct s_shared_resource *shared_resource)
{
	free(shared_resource->tasks);
	shared_resource->tasks = NULL;
	shared_resource->task_count = 0;
}

static int	init_tasks(struct s_shared_resource *shared_resource)
{
	int	index;

	shared_resource->tasks = malloc(sizeof(struct s_task)
			* shared_resource->config.number_of_coders);
	if (shared_resource->tasks == NULL)
		return (0);
	index = 0;
	while (index < shared_resource->config.number_of_coders)
	{
		shared_resource->tasks[index].developer = NULL;
		shared_resource->tasks[index].arrival_order = 0;
		shared_resource->tasks[index].deadline = 0;
		index++;
	}
	shared_resource->task_count = 0;
	shared_resource->next_task_order = 0;
	return (1);
}

static void	destroy_dongles(struct s_shared_resource *shared_resource)
{
	int	index;

	if (shared_resource->dongles == NULL)
		return ;
	index = 0;
	while (index < shared_resource->config.number_of_coders)
	{
		pthread_mutex_destroy(&shared_resource->dongles[index].mutex);
		index++;
	}
	free(shared_resource->dongles);
	shared_resource->dongles = NULL;
}

static int	init_dongles(struct s_shared_resource *shared_resource)
{
	int	index;

	shared_resource->dongles = malloc(sizeof(struct s_dongle)
			* shared_resource->config.number_of_coders);
	if (shared_resource->dongles == NULL)
		return (0);
	index = 0;
	while (index < shared_resource->config.number_of_coders)
	{
		shared_resource->dongles[index].is_available = 1;
		shared_resource->dongles[index].cooldown_until = 0;
		if (pthread_mutex_init(&shared_resource->dongles[index].mutex, NULL) != 0)
		{
			while (index > 0)
				pthread_mutex_destroy(&shared_resource->dongles[--index].mutex);
			free(shared_resource->dongles);
			shared_resource->dongles = NULL;
			return (0);
		}
		index++;
	}
	return (1);
}

static int	init_shared_resource(struct s_shared_resource *shared_resource,
	struct s_config *config)
{
	shared_resource->config = *config;
	shared_resource->stopped = 0;
	shared_resource->dongles = NULL;
	shared_resource->tasks = NULL;
	shared_resource->task_count = 0;
	shared_resource->next_task_order = 0;
	shared_resource->start_time = get_time();
	if (pthread_mutex_init(&shared_resource->print_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&shared_resource->state_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&shared_resource->print_mutex);
		return (0);
	}
	if (pthread_mutex_init(&shared_resource->scheduler_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&shared_resource->state_mutex);
		pthread_mutex_destroy(&shared_resource->print_mutex);
		return (0);
	}
	if (pthread_cond_init(&shared_resource->scheduler_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&shared_resource->scheduler_mutex);
		pthread_mutex_destroy(&shared_resource->state_mutex);
		pthread_mutex_destroy(&shared_resource->print_mutex);
		return (0);
	}
	if (!init_dongles(shared_resource))
	{
		pthread_cond_destroy(&shared_resource->scheduler_cond);
		pthread_mutex_destroy(&shared_resource->scheduler_mutex);
		pthread_mutex_destroy(&shared_resource->state_mutex);
		pthread_mutex_destroy(&shared_resource->print_mutex);
		return (0);
	}
	if (!init_tasks(shared_resource))
	{
		destroy_dongles(shared_resource);
		pthread_cond_destroy(&shared_resource->scheduler_cond);
		pthread_mutex_destroy(&shared_resource->scheduler_mutex);
		pthread_mutex_destroy(&shared_resource->state_mutex);
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
		destroy_tasks(&simulation_data->shared_resource);
		destroy_dongles(&simulation_data->shared_resource);
		pthread_cond_destroy(&simulation_data->shared_resource.scheduler_cond);
		pthread_mutex_destroy(&simulation_data->shared_resource.scheduler_mutex);
		pthread_mutex_destroy(&simulation_data->shared_resource.print_mutex);
		pthread_mutex_destroy(&simulation_data->shared_resource.state_mutex);
		return (0);
	}
	init_developers(simulation_data);
	return (1);
}

void	destroy_simulation(struct s_simulation *simulation_data)
{
	destroy_tasks(&simulation_data->shared_resource);
	destroy_dongles(&simulation_data->shared_resource);
	pthread_cond_destroy(&simulation_data->shared_resource.scheduler_cond);
	pthread_mutex_destroy(&simulation_data->shared_resource.scheduler_mutex);
	pthread_mutex_destroy(&simulation_data->shared_resource.print_mutex);
	pthread_mutex_destroy(&simulation_data->shared_resource.state_mutex);
	free(simulation_data->developers);
	simulation_data->developers = NULL;
}
