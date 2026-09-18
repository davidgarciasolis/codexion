
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

void	destruir_simulacion(t_simulacion *simulacion)
{
	pthread_cond_destroy(&simulacion->cambio);
	pthread_mutex_destroy(&simulacion->cerrojo_impresion);
	pthread_mutex_destroy(&simulacion->cerrojo);
	free(simulacion->cola);
	free(simulacion->programadores);
	free(simulacion->llaves);
}
