#include <stdio.h>
#include "codexion.h"

void	registrar_estado(struct s_desarrollador *desarrollador, char *estado)
{
	struct s_recurso_compartido	*recurso_compartido;
	long					marca_tiempo;

	recurso_compartido = desarrollador->recurso_compartido;
	pthread_mutex_lock(&recurso_compartido->mutex_recurso_compartido);
	if (!recurso_compartido->detenida)
	{
		pthread_mutex_lock(&recurso_compartido->mutex_impresion);
		marca_tiempo = obtener_tiempo() - recurso_compartido->inicio;
		printf("%ld %d %s\n", marca_tiempo, desarrollador->id, estado);
		pthread_mutex_unlock(&recurso_compartido->mutex_impresion);
	}
	pthread_mutex_unlock(&recurso_compartido->mutex_recurso_compartido);
}
