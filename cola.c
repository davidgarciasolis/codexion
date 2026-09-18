#include "codexion.h"

int	va_antes(t_simulacion *simulacion, t_programador *primero,
		t_programador *segundo)
{
	long	primer_limite;
	long	segundo_limite;

	primer_limite = primero->ultima_compilacion
		+ simulacion->configuracion.agotamiento;
	segundo_limite = segundo->ultima_compilacion
		+ simulacion->configuracion.agotamiento;
	if (simulacion->configuracion.edf)
		return (primer_limite < segundo_limite || (primer_limite
				== segundo_limite && primero->turno < segundo->turno));
	return (primero->turno < segundo->turno);
}

static void	intercambiar(t_programador **primero, t_programador **segundo)
{
	t_programador	*temporal;

	temporal = *primero;
	*primero = *segundo;
	*segundo = temporal;
}

void	cola_insertar(t_simulacion *simulacion, t_programador *programador)
{
	int	indice;

	indice = simulacion->tamano_cola++;
	simulacion->cola[indice] = programador;
	while (indice && va_antes(simulacion, simulacion->cola[indice],
			simulacion->cola[(indice - 1) / 2]))
	{
		intercambiar(&simulacion->cola[indice], &simulacion->cola[(indice - 1) / 2]);
		indice = (indice - 1) / 2;
	}
}

void	cola_eliminar(t_simulacion *simulacion, int indice)
{
	int	hijo;

	simulacion->cola[indice] = simulacion->cola[--simulacion->tamano_cola];
	while (indice && va_antes(simulacion, simulacion->cola[indice],
			simulacion->cola[(indice - 1) / 2]))
	{
		intercambiar(&simulacion->cola[indice], &simulacion->cola[(indice - 1) / 2]);
		indice = (indice - 1) / 2;
	}
	while (indice * 2 + 1 < simulacion->tamano_cola)
	{
		hijo = indice * 2 + 1;
		if (hijo + 1 < simulacion->tamano_cola
			&& va_antes(simulacion, simulacion->cola[hijo + 1],
				simulacion->cola[hijo]))
			hijo++;
		if (!va_antes(simulacion, simulacion->cola[hijo], simulacion->cola[indice]))
			break ;
		intercambiar(&simulacion->cola[indice], &simulacion->cola[hijo]);
		indice = hijo;
	}
}
