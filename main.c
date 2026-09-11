#include <stdio.h>
#include "codexion.h"

static void	imprimir_configuracion(struct s_simulacion *datos_simulacion)
{
	printf("cantidad_desarrolladores: %d\n",
		datos_simulacion->recurso_compartido.configuracion.cantidad_desarrolladores);
	printf("tiempo_agotamiento: %d\n",
		datos_simulacion->recurso_compartido.configuracion.tiempo_agotamiento);
	printf("tiempo_compilar: %d\n",
		datos_simulacion->recurso_compartido.configuracion.tiempo_compilar);
	printf("tiempo_depurar: %d\n",
		datos_simulacion->recurso_compartido.configuracion.tiempo_depurar);
	printf("tiempo_refactorizar: %d\n",
		datos_simulacion->recurso_compartido.configuracion.tiempo_refactorizar);
	printf("compilaciones_requeridas: %d\n", datos_simulacion->recurso_compartido
		.configuracion.compilaciones_requeridas);
	printf("enfriamiento_dongle: %d\n",
		datos_simulacion->recurso_compartido.configuracion.enfriamiento_dongle);
	printf("planificador: %s\n",
		datos_simulacion->recurso_compartido.configuracion.planificador);
}

int	main(int argc, char **argv)
{
	struct s_configuracion	configuracion;
	struct s_simulacion	datos_simulacion;

	if (!parseo(argc, argv, &configuracion))
		return (1);
	if (!iniciar_simulacion(&datos_simulacion, &configuracion))
		return (1);
	imprimir_configuracion(&datos_simulacion);
	destruir_simulacion(&datos_simulacion);
	return (0);
}
