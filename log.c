#include <stdio.h>
#include "codexion.h"

void	log_status(struct s_developer *developer, char *status)
{
	struct s_shared_resource	*shared_resource;
	long				timestamp;

	shared_resource = developer->shared_resource;
	pthread_mutex_lock(&shared_resource->state_mutex);
	if (!shared_resource->stopped)
	{
		pthread_mutex_lock(&shared_resource->print_mutex);
		timestamp = get_time() - shared_resource->start_time;
		printf("%ld %d %s\n", timestamp, developer->id, status);
		pthread_mutex_unlock(&shared_resource->print_mutex);
	}
	pthread_mutex_unlock(&shared_resource->state_mutex);
}
