#include "codexion.h"

static int	iniciar_programadores(t_simulacion *simulacion)
{
	int	indice;

	indice = 0;
	while (indice < simulacion->configuracion.programadores)
	{
		if (pthread_create(&simulacion->programadores[indice].hilo, NULL,
				rutina_programador,
				&simulacion->programadores[indice]))
		{
			detener_simulacion(simulacion);
			break ;
		}
		indice++;
	}
	return (indice);
}

int	main(int argc, char **argv)
{
	t_configuracion	configuracion;
	t_simulacion		simulacion;
	pthread_t	monitor;
	int			indice;

	if (!parsear(argc, argv, &configuracion)
		|| !inicializar_simulacion(&simulacion, &configuracion))
		return (1);
	indice = iniciar_programadores(&simulacion);
	pthread_create(&monitor, NULL, rutina_monitor, &simulacion);
	while (indice-- > 0)
		pthread_join(simulacion.programadores[indice].hilo, NULL);
	detener_simulacion(&simulacion);
	pthread_join(monitor, NULL);
	destruir_simulacion(&simulacion);
	return (0);
}
