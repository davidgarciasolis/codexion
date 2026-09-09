#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "codexion.h"

static int	is_positive_int(char *argument, int argument_index)
{
	int		value;
	long	next_value;
	int		index;

	if (argument[0] == '\0')
	{
		printf("Error: el argumento %d no puede estar vacío.\n", argument_index);
		return (0);
	}
	value = 0;
	index = 0;
	while (argument[index] != '\0')
	{
		if (argument[index] < '0' || argument[index] > '9')
		{
			printf("Error: el argumento %d debe contener solo dígitos.\n",
				argument_index);
			return (0);
		}
		next_value = (long)value * 10 + (argument[index] - '0');
		if (next_value > INT_MAX)
		{
			printf("Error: el argumento %d no puede superar INT_MAX (%d).\n",
				argument_index, INT_MAX);
			return (0);
		}
		value = (int)next_value;
		index++;
	}
	if (value == 0)
	{
		printf("Error: el argumento %d debe ser mayor que 0.\n",
			argument_index);
		return (0);
	}
	return (1);
}

static void	fill_config(struct s_config *config, char **argv)
{
	config->number_of_coders = atoi(argv[1]);
	config->time_to_burnout = atoi(argv[2]);
	config->time_to_compile = atoi(argv[3]);
	config->time_to_debug = atoi(argv[4]);
	config->time_to_refactor = atoi(argv[5]);
	config->number_of_compiles_required = atoi(argv[6]);
	config->dongle_cooldown = atoi(argv[7]);
	config->scheduler = argv[8];
}

int	parseo(int argc, char **argv, struct s_config *config)
{
	int	index;

	if (argc != 9)
	{
		printf("Error: número de argumentos incorrecto.\n");
		printf("Uso: %s number_of_coders time_to_burnout ", argv[0]);
		printf("time_to_compile time_to_debug time_to_refactor ");
		printf("number_of_compiles_required dongle_cooldown scheduler\n");
		return (0);
	}
	index = 1;
	while (index < 8)
	{
		if (!is_positive_int(argv[index], index))
			return (0);
		index++;
	}
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
	{
		printf("Error: scheduler debe ser fifo o edf.\n");
		return (0);
	}
	fill_config(config, argv);
	return (1);
}
