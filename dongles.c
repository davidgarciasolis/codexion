#include "codexion.h"
#include <string.h>
#include <time.h>

static int	debe_priorizarse(struct s_recurso_compartido *recurso,
	struct s_tarea *primera, struct s_tarea *segunda)
{
	if (strcmp(recurso->configuracion.planificador, "edf") == 0)
		return (primera->fecha_limite < segunda->fecha_limite || (primera->fecha_limite
			== segunda->fecha_limite && primera->orden_llegada < segunda->orden_llegada));
	return (primera->orden_llegada < segunda->orden_llegada);
}

static void	agregar_tarea(struct s_recurso_compartido *recurso,
	struct s_desarrollador *desarrollador)
{
	int indice, padre;
	struct s_tarea temporal;

	indice = recurso->cantidad_tareas;
	pthread_mutex_lock(&recurso->mutex_estado);
	recurso->tareas[indice].desarrollador = desarrollador;
	recurso->tareas[indice].orden_llegada = recurso->siguiente_orden_tarea++;
	recurso->tareas[indice].fecha_limite = desarrollador->inicio_ultima_compilacion
		+ recurso->configuracion.tiempo_agotamiento;
	pthread_mutex_unlock(&recurso->mutex_estado);
	recurso->cantidad_tareas++;
	while (indice > 0)
	{
		padre = (indice - 1) / 2;
		if (!debe_priorizarse(recurso, &recurso->tareas[indice], &recurso->tareas[padre]))
			break ;
		temporal = recurso->tareas[indice];
		recurso->tareas[indice] = recurso->tareas[padre];
		recurso->tareas[padre] = temporal;
		indice = padre;
	}
}

static int	es_primera_tarea(struct s_recurso_compartido *recurso,
	struct s_desarrollador *desarrollador)
{
	return (recurso->cantidad_tareas > 0 && recurso->tareas[0].desarrollador == desarrollador);
}

static int	dongles_listos(struct s_recurso_compartido *recurso,
	int izquierdo, int derecho)
{
	int listos;
	long tiempo_actual;

	pthread_mutex_lock(&recurso->dongles[izquierdo].mutex);
	pthread_mutex_lock(&recurso->dongles[derecho].mutex);
	tiempo_actual = obtener_tiempo();
	listos = (recurso->dongles[izquierdo].disponible
		&& recurso->dongles[izquierdo].enfriamiento_hasta <= tiempo_actual
		&& recurso->dongles[derecho].disponible
		&& recurso->dongles[derecho].enfriamiento_hasta <= tiempo_actual);
	pthread_mutex_unlock(&recurso->dongles[derecho].mutex);
	pthread_mutex_unlock(&recurso->dongles[izquierdo].mutex);
	return (listos);
}

static void	reservar_dongles(struct s_recurso_compartido *recurso,
	int izquierdo, int derecho)
{
	pthread_mutex_lock(&recurso->dongles[izquierdo].mutex);
	pthread_mutex_lock(&recurso->dongles[derecho].mutex);
	recurso->dongles[izquierdo].disponible = 0;
	recurso->dongles[derecho].disponible = 0;
	pthread_mutex_unlock(&recurso->dongles[derecho].mutex);
	pthread_mutex_unlock(&recurso->dongles[izquierdo].mutex);
}

static void	esperar_dongles(struct s_recurso_compartido *recurso,
	int izquierdo, int derecho)
{
	long esperar_hasta, tiempo_actual;
	struct timespec limite;

	pthread_mutex_lock(&recurso->dongles[izquierdo].mutex);
	pthread_mutex_lock(&recurso->dongles[derecho].mutex);
	esperar_hasta = recurso->dongles[izquierdo].enfriamiento_hasta;
	if (recurso->dongles[derecho].enfriamiento_hasta > esperar_hasta)
		esperar_hasta = recurso->dongles[derecho].enfriamiento_hasta;
	pthread_mutex_unlock(&recurso->dongles[derecho].mutex);
	pthread_mutex_unlock(&recurso->dongles[izquierdo].mutex);
	tiempo_actual = obtener_tiempo();
	if (esperar_hasta <= tiempo_actual)
		esperar_hasta = tiempo_actual + 1;
	limite.tv_sec = esperar_hasta / 1000;
	limite.tv_nsec = (esperar_hasta % 1000) * 1000000;
	pthread_cond_timedwait(&recurso->condicion_planificador,
		&recurso->mutex_planificador, &limite);
}

static int	tomar_dongles_unico(struct s_desarrollador *desarrollador)
{
	struct s_recurso_compartido	*recurso;
	long					fecha_limite;
	struct timespec			limite;

	recurso = desarrollador->recurso_compartido;
	pthread_mutex_lock(&recurso->mutex_estado);
	fecha_limite = desarrollador->inicio_ultima_compilacion
		+ recurso->configuracion.tiempo_agotamiento;
	pthread_mutex_unlock(&recurso->mutex_estado);
	limite.tv_sec = fecha_limite / 1000;
	limite.tv_nsec = (fecha_limite % 1000) * 1000000;
	pthread_mutex_lock(&recurso->mutex_planificador);
	while (!simulacion_detenida(recurso) && obtener_tiempo() < fecha_limite)
		pthread_cond_timedwait(&recurso->condicion_planificador,
			&recurso->mutex_planificador, &limite);
	if (!simulacion_detenida(recurso) && obtener_tiempo() >= fecha_limite)
	{
		pthread_mutex_lock(&recurso->mutex_estado);
		recurso->detenida = 1;
		pthread_mutex_unlock(&recurso->mutex_estado);
		pthread_cond_broadcast(&recurso->condicion_planificador);
	}
	pthread_mutex_unlock(&recurso->mutex_planificador);
	return (0);
}

static void	reparar_monticulo(struct s_recurso_compartido *recurso, int indice)
{
	int izquierdo, derecho, prioritario;
	struct s_tarea temporal;

	while (1)
	{
		izquierdo = indice * 2 + 1;
		derecho = indice * 2 + 2;
		prioritario = indice;
		if (izquierdo < recurso->cantidad_tareas && debe_priorizarse(recurso,
				&recurso->tareas[izquierdo], &recurso->tareas[prioritario]))
			prioritario = izquierdo;
		if (derecho < recurso->cantidad_tareas && debe_priorizarse(recurso,
				&recurso->tareas[derecho], &recurso->tareas[prioritario]))
			prioritario = derecho;
		if (prioritario == indice)
			break ;
		temporal = recurso->tareas[indice];
		recurso->tareas[indice] = recurso->tareas[prioritario];
		recurso->tareas[prioritario] = temporal;
		indice = prioritario;
	}
}

static void	quitar_tarea(struct s_recurso_compartido *recurso,
	struct s_desarrollador *desarrollador)
{
	int indice, padre;
	struct s_tarea temporal;

	indice = 0;
	while (indice < recurso->cantidad_tareas && recurso->tareas[indice].desarrollador != desarrollador)
		indice++;
	if (indice == recurso->cantidad_tareas)
		return ;
	recurso->cantidad_tareas--;
	if (indice == recurso->cantidad_tareas)
		return ;
	recurso->tareas[indice] = recurso->tareas[recurso->cantidad_tareas];
	while (indice > 0)
	{
		padre = (indice - 1) / 2;
		if (!debe_priorizarse(recurso, &recurso->tareas[indice], &recurso->tareas[padre]))
			break ;
		temporal = recurso->tareas[indice];
		recurso->tareas[indice] = recurso->tareas[padre];
		recurso->tareas[padre] = temporal;
		indice = padre;
	}
	reparar_monticulo(recurso, indice);
}

static void	quitar_primera_tarea(struct s_recurso_compartido *recurso)
{
	if (recurso->cantidad_tareas == 0)
		return ;
	recurso->cantidad_tareas--;
	if (recurso->cantidad_tareas == 0)
		return ;
	recurso->tareas[0] = recurso->tareas[recurso->cantidad_tareas];
	reparar_monticulo(recurso, 0);
}

void	liberar_dongles(struct s_desarrollador *desarrollador)
{
	struct s_recurso_compartido *recurso;
	int izquierdo, derecho;
	long enfriamiento_hasta;

	recurso = desarrollador->recurso_compartido;
	if (recurso->configuracion.cantidad_desarrolladores == 1)
		return ;
	izquierdo = desarrollador->id - 1;
	derecho = desarrollador->id % recurso->configuracion.cantidad_desarrolladores;
	pthread_mutex_lock(&recurso->mutex_planificador);
	pthread_mutex_lock(&recurso->dongles[izquierdo].mutex);
	pthread_mutex_lock(&recurso->dongles[derecho].mutex);
	enfriamiento_hasta = obtener_tiempo() + recurso->configuracion.enfriamiento_dongle;
	recurso->dongles[izquierdo].disponible = 1;
	recurso->dongles[izquierdo].enfriamiento_hasta = enfriamiento_hasta;
	recurso->dongles[derecho].disponible = 1;
	recurso->dongles[derecho].enfriamiento_hasta = enfriamiento_hasta;
	pthread_mutex_unlock(&recurso->dongles[derecho].mutex);
	pthread_mutex_unlock(&recurso->dongles[izquierdo].mutex);
	pthread_cond_broadcast(&recurso->condicion_planificador);
	pthread_mutex_unlock(&recurso->mutex_planificador);
}

int	tomar_dongles(struct s_desarrollador *desarrollador)
{
	struct s_recurso_compartido *recurso;
	int izquierdo, derecho;

	recurso = desarrollador->recurso_compartido;
	if (recurso->configuracion.cantidad_desarrolladores == 1)
		return (tomar_dongles_unico(desarrollador));
	izquierdo = desarrollador->id - 1;
	derecho = desarrollador->id % recurso->configuracion.cantidad_desarrolladores;
	pthread_mutex_lock(&recurso->mutex_planificador);
	agregar_tarea(recurso, desarrollador);
	while (!simulacion_detenida(recurso))
	{
		if (es_primera_tarea(recurso, desarrollador)
			&& dongles_listos(recurso, izquierdo, derecho))
		{
			reservar_dongles(recurso, izquierdo, derecho);
			quitar_primera_tarea(recurso);
			pthread_mutex_unlock(&recurso->mutex_planificador);
			return (1);
		}
		esperar_dongles(recurso, izquierdo, derecho);
	}
	quitar_tarea(recurso, desarrollador);
	pthread_mutex_unlock(&recurso->mutex_planificador);
	return (0);
}
