#include <stdio.h>
#include "codexion.h"

static void	print_config(struct s_simulation *simulation_data)
{
	printf("number_of_coders: %d\n",
		simulation_data->shared_resource.config.number_of_coders);
	printf("time_to_burnout: %d\n",
		simulation_data->shared_resource.config.time_to_burnout);
	printf("time_to_compile: %d\n",
		simulation_data->shared_resource.config.time_to_compile);
	printf("time_to_debug: %d\n",
		simulation_data->shared_resource.config.time_to_debug);
	printf("time_to_refactor: %d\n",
		simulation_data->shared_resource.config.time_to_refactor);
	printf("number_of_compiles_required: %d\n",
		simulation_data->shared_resource.config.number_of_compiles_required);
	printf("dongle_cooldown: %d\n",
		simulation_data->shared_resource.config.dongle_cooldown);
	printf("scheduler: %s\n",
		simulation_data->shared_resource.config.scheduler);
}

int	main(int argc, char **argv)
{
	struct s_config		config;
	struct s_simulation	simulation_data;

	if (!parseo(argc, argv, &config))
		return (1);
	if (!init_simulation(&simulation_data, &config))
		return (1);
	print_config(&simulation_data);
	destroy_simulation(&simulation_data);
	return (0);
}
