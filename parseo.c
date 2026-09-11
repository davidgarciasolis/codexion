#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "codexion.h"

static int	es_entero_positivo(char *argumento, int indice_argumento)
{
	int		valor;
	long	siguiente_valor;
	int		indice;

	if (argumento[0] == '\0')
	{
		printf("Error: el argumento %d no puede estar vacío.\n", indice_argumento);
		return (0);
	}
	valor = 0;
	indice = 0;
	while (argumento[indice] != '\0')
	{
		if (argumento[indice] < '0' || argumento[indice] > '9')
		{
			printf("Error: el argumento %d debe contener solo dígitos.\n",
				indice_argumento);
			return (0);
		}
		siguiente_valor = (long)valor * 10 + (argumento[indice] - '0');
		if (siguiente_valor > INT_MAX)
		{
			printf("Error: el argumento %d no puede superar INT_MAX (%d).\n",
				indice_argumento, INT_MAX);
			return (0);
		}
		valor = (int)siguiente_valor;
		indice++;
	}
	if (valor == 0)
	{
		printf("Error: el argumento %d debe ser mayor que 0.\n",
			indice_argumento);
		return (0);
	}
	return (1);
}

static void	rellenar_configuracion(struct s_configuracion *configuracion,
	char **argumentos)
{
	configuracion->cantidad_desarrolladores = atoi(argumentos[1]);
	configuracion->tiempo_agotamiento = atoi(argumentos[2]);
	configuracion->tiempo_compilar = atoi(argumentos[3]);
	configuracion->tiempo_depurar = atoi(argumentos[4]);
	configuracion->tiempo_refactorizar = atoi(argumentos[5]);
	configuracion->compilaciones_requeridas = atoi(argumentos[6]);
	configuracion->enfriamiento_dongle = atoi(argumentos[7]);
	configuracion->planificador = argumentos[8];
}

int	parseo(int cantidad_argumentos, char **argumentos,
	struct s_configuracion *configuracion)
{
	int	indice;

	if (cantidad_argumentos != 9)
	{
		printf("Error: número de argumentos incorrecto.\n");
		printf("Uso: %s cantidad_desarrolladores tiempo_agotamiento ", argumentos[0]);
		printf("tiempo_compilar tiempo_depurar tiempo_refactorizar ");
		printf("compilaciones_requeridas enfriamiento_dongle planificador\n");
		return (0);
	}
	indice = 1;
	while (indice < 8)
	{
		if (!es_entero_positivo(argumentos[indice], indice))
			return (0);
		indice++;
	}
	if (strcmp(argumentos[8], "fifo") != 0 && strcmp(argumentos[8], "edf") != 0)
	{
		printf("Error: planificador debe ser fifo o edf.\n");
		return (0);
	}
	rellenar_configuracion(configuracion, argumentos);
	return (1);
}
