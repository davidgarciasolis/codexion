/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:34:14 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/18 17:34:15 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <sys/time.h>

long	tiempo_ms(void)
{
	struct timeval	marca_tiempo;

	gettimeofday(&marca_tiempo, NULL);
	return (marca_tiempo.tv_sec * 1000L + marca_tiempo.tv_usec / 1000);
}
