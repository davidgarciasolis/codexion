/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davgarc4 <davgarc4@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 17:32:01 by davgarc4          #+#    #+#             */
/*   Updated: 2026/09/18 17:40:41 by davgarc4         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>

typedef struct s_configuracion
{
	int		programadores;
	long	agotamiento;
	long	compilar;
	long	depurar;
	long	refactorizar;
	int		requeridos;
	long	enfriamiento;
	int		edf;
}	t_configuracion;

typedef struct s_llave
{
	int		libre;
	long	lista_en;
}	t_llave;

typedef struct s_programador	t_programador;

typedef struct s_simulacion
{
	t_configuracion		configuracion;
	t_llave				*llaves;
	t_programador		*programadores;
	t_programador		**cola;
	int					tamano_cola;
	long				siguiente_turno;
	long				inicio;
	int					detenida;
	int					terminados;
	pthread_mutex_t		cerrojo;
	pthread_mutex_t		cerrojo_impresion;
	pthread_cond_t		cambio;
}	t_simulacion;

struct s_programador
{
	int				id;
	int				compilaciones;
	int				esperando;
	int				concedido;
	long			ultima_compilacion;
	long			turno;
	pthread_t		hilo;
	t_simulacion	*simulacion;
};

long	tiempo_ms(void);
int		parsear(int argc, char **argv, t_configuracion *configuracion);
int		inicializar_simulacion(t_simulacion *simulacion,
			t_configuracion *configuracion);
int	liberar_asignaciones(t_simulacion *simulacion);
int	limpiar_cerrojo(t_simulacion *simulacion);
int	limpiar_cerrojo_impresion(t_simulacion *simulacion);
void	destruir_simulacion(t_simulacion *simulacion);
void	*rutina_programador(void *argumento);
void	*rutina_monitor(void *argumento);
void	registrar_estado(t_programador *programador, char *estado);
void	registrar_agotamiento(t_programador *programador);
void	registrar_finalizacion(t_simulacion *simulacion);
void	detener_simulacion(t_simulacion *simulacion);
void	cola_insertar(t_simulacion *simulacion, t_programador *programador);
void	cola_eliminar(t_simulacion *simulacion, int indice);
void	conceder_esperando(t_simulacion *simulacion);
int		puede_tomar(t_programador *programador, long ahora);
int		va_antes(t_simulacion *simulacion, t_programador *primero,
			t_programador *segundo);
int		tomar_llaves(t_programador *programador);
void	liberar_llaves(t_programador *programador);
int		esta_detenida(t_simulacion *simulacion);

#endif
