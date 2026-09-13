#include "codexion.h"
#include <stdlib.h>

void	destruir_simulacion(t_simulacion *simulacion)
{
	pthread_cond_destroy(&simulacion->cambio);
	pthread_mutex_destroy(&simulacion->cerrojo_impresion);
	pthread_mutex_destroy(&simulacion->cerrojo);
	free(simulacion->cola);
	free(simulacion->programadores);
	free(simulacion->llaves);
}
