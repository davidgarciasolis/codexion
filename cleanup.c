/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:29:38 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/18 17:31:51 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "codexion.h"

int	liberar_asignaciones(t_simulacion *simulacion)
{
	free(simulacion->llaves);
	free(simulacion->programadores);
	free(simulacion->cola);
	return (0);
}

int	limpiar_cerrojo(t_simulacion *simulacion)
{
	pthread_mutex_destroy(&simulacion->cerrojo);
	free(simulacion->llaves);
	free(simulacion->programadores);
	free(simulacion->cola);
	return (0);
}

int	limpiar_cerrojo_impresion(t_simulacion *simulacion)
{
	pthread_mutex_destroy(&simulacion->cerrojo_impresion);
	pthread_mutex_destroy(&simulacion->cerrojo);
	free(simulacion->llaves);
	free(simulacion->programadores);
	free(simulacion->cola);
	return (0);
}

void	destruir_simulacion(t_simulacion *simulacion)
{
	pthread_cond_destroy(&simulacion->cambio);
	limpiar_cerrojo_impresion(simulacion);
}
