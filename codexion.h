#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>

struct s_configuracion
{
	int	cantidad_desarrolladores;
	int	tiempo_agotamiento;
	int	tiempo_compilar;
	int	tiempo_depurar;
	int	tiempo_refactorizar;
	int	compilaciones_requeridas;
	int	enfriamiento_dongle;
	char	*planificador;
};

struct s_dongle
{
	int			disponible;
	long			enfriamiento_hasta;
	pthread_mutex_t	mutex;
};

struct s_tarea
{
	struct s_desarrollador	*desarrollador;
	long				orden_llegada;
	long				fecha_limite;
};

struct s_recurso_compartido
{
	struct s_configuracion	configuracion;
	struct s_dongle		*dongles;
	struct s_tarea		*tareas;
	long				inicio;
	long				siguiente_orden_tarea;
	int				detenida;
	int				cantidad_tareas;
	pthread_mutex_t		mutex_impresion;
	pthread_mutex_t		mutex_estado;
	pthread_mutex_t		mutex_planificador;
	pthread_cond_t		condicion_planificador;
};

struct s_desarrollador
{
	int				id;
	int				compilaciones_hechas;
	long				inicio_ultima_compilacion;
	pthread_t		thread;
	struct s_recurso_compartido	*recurso_compartido;
};

struct s_simulacion
{
	struct s_desarrollador		*desarrolladores;
	struct s_recurso_compartido	recurso_compartido;
};

int		parseo(int argc, char **argv, struct s_configuracion *configuracion);
int		iniciar_simulacion(struct s_simulacion *datos_simulacion,
			struct s_configuracion *configuracion);
void	destruir_simulacion(struct s_simulacion *datos_simulacion);
long	obtener_tiempo(void);
void	*rutina_desarrollador(void *argumento);
int		simulacion_detenida(struct s_recurso_compartido *recurso_compartido);
int		tomar_dongles(struct s_desarrollador *desarrollador);
void	liberar_dongles(struct s_desarrollador *desarrollador);
void	registrar_estado(struct s_desarrollador *desarrollador, char *estado);

#endif
