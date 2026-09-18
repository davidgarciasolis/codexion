
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   planificador.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:33:54 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/18 17:33:55 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	conceder(t_simulacion *simulacion, int indice)
{
	int	izquierda;
	int	derecha;

	izquierda = simulacion->cola[indice]->id - 1;
	derecha = simulacion->cola[indice]->id
		% simulacion->configuracion.programadores;
	simulacion->llaves[izquierda].libre = 0;
	simulacion->llaves[derecha].libre = 0;
	simulacion->cola[indice]->esperando = 0;
	simulacion->cola[indice]->concedido = 1;
	cola_eliminar(simulacion, indice);
	pthread_cond_broadcast(&simulacion->cambio);
}

static int	mejor_disponible(t_simulacion *simulacion, long ahora)
{
	int	indice;
	int	mejor;

	indice = 0;
	mejor = -1;
	while (indice < simulacion->tamano_cola)
	{
		if (puede_tomar(simulacion->cola[indice], ahora) && (mejor == -1
				|| va_antes(simulacion, simulacion->cola[indice], simulacion->cola[mejor])))
			mejor = indice;
		indice++;
	}
	return (mejor);
}

void	conceder_esperando(t_simulacion *simulacion)
{
	int		mejor;
	long	ahora;

	ahora = tiempo_ms();
	mejor = mejor_disponible(simulacion, ahora);
	while (mejor != -1)
	{
		conceder(simulacion, mejor);
		mejor = mejor_disponible(simulacion, ahora);
	}
}
