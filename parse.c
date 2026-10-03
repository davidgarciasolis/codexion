/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:32:40 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/18 17:58:08 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/codexion.h"
#include <stdio.h>
#include <string.h>

static int	number(char *text, long *value)
{
	long	number;

	if (!*text)
		return (0);
	number = 0;
	while (*text)
	{
		if (*text < '0' || *text > '9' || number > 214748364)
			return (0);
		number = number * 10 + *text - '0';
		text++;
	}
	if (number > 2147483647)
		return (0);
	*value = number;
	return (1);
}

static int	valid_argument(char *text, long *value, int index)
{
	if (!number(text, value))
		return (0);
	if (index == 0 || index == 5)
		return (*value > 0);
	return (1);
}

static void	configure(t_config *config, long *value,
		char *scheduler)
{
	config->coders = (int)value[0];
	config->time_to_burnout = value[1];
	config->time_to_compile = value[2];
	config->time_to_debug = value[3];
	config->time_to_refactor = value[4];
	config->compiles_required = (int)value[5];
	config->dongle_cooldown = value[6];
	config->edf = !strcmp(scheduler, "edf");
}

int	parse_args(int argc, char **argv, t_config *config)
{
	long	value[7];
	int	index;

	if (argc != 9 || (strcmp(argv[8], "fifo") && strcmp(argv[8], "edf")))
		return (printf("Error: invalid arguments.\n"), 0);
	index = 0;
	while (index < 7)
	{
		if (!valid_argument(argv[index + 1], &value[index], index))
			return (printf("Error: arguments"
					" must be non-negative integers; coders and"
					" required compilations must be greater than 0.\n"), 0);
		index++;
	}
	configure(config, value, argv[8]);
	return (1);
}
