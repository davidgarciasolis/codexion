/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseo.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:32:40 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/18 17:32:41 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>
#include <string.h>

static int	numero(char *texto, long *valor)
{
	long	numero;

	if (!*texto)
		return (0);
	numero = 0;
	while (*texto)
	{
		if (*texto < '0' || *texto > '9' || numero > 214748364)
			return (0);
		numero = numero * 10 + *texto - '0';
		texto++;
	}
	if (numero <= 0 || numero > 2147483647)
		return (0);
	*valor = numero;
	return (1);
}

static void	configurar(t_configuracion *configuracion, long *valor,
		char *planificador)
{
	configuracion->programadores = (int)valor[0];
	configuracion->agotamiento = valor[1];
	configuracion->compilar = valor[2];
	configuracion->depurar = valor[3];
	configuracion->refactorizar = valor[4];
	configuracion->requeridos = (int)valor[5];
	configuracion->enfriamiento = valor[6];
	configuracion->edf = !strcmp(planificador, "edf");
}

int	parsear(int argc, char **argv, t_configuracion *configuracion)
{
	long	valor[7];
	int		indice;

	if (argc != 9 || (strcmp(argv[8], "fifo") && strcmp(argv[8], "edf")))
		return (printf("Error: argumentos no válidos.\n"), 0);
	indice = 0;
	while (indice < 7)
	{
		if (!numero(argv[indice + 1], &valor[indice]))
			return (printf("Error: los argumentos deben ser enteros positivos.\n"), 0);
		indice++;
	}
	configurar(configuracion, valor, argv[8]);
	return (1);
}
