#include "codexion.h"
#include <string.h>
#include <time.h>

void	add_task(struct s_shared_resource *shared_resource,
	struct s_developer *developer)
{
	int	index;
	int	parent;
	int	must_swap;
	struct s_task	temporary;

	index = shared_resource->task_count;
	pthread_mutex_lock(&shared_resource->state_mutex);
	shared_resource->tasks[index].developer = developer;
	shared_resource->tasks[index].arrival_order =
		shared_resource->next_task_order++;
	shared_resource->tasks[index].deadline = developer->last_compile_start
		+ shared_resource->config.time_to_burnout;
	pthread_mutex_unlock(&shared_resource->state_mutex);
	shared_resource->task_count++;
	while (index > 0)
	{
		parent = (index - 1) / 2;
		must_swap = shared_resource->tasks[index].arrival_order
			< shared_resource->tasks[parent].arrival_order;
		if (strcmp(shared_resource->config.scheduler, "edf") == 0)
			must_swap = (shared_resource->tasks[index].deadline
				< shared_resource->tasks[parent].deadline || (shared_resource->tasks[index].deadline
				== shared_resource->tasks[parent].deadline && must_swap));
		if (!must_swap)
			break ;
		temporary = shared_resource->tasks[index];
		shared_resource->tasks[index] = shared_resource->tasks[parent];
		shared_resource->tasks[parent] = temporary;
		index = parent;
	}
}

int	is_first_task(struct s_shared_resource *shared_resource,
	struct s_developer *developer)
{
	if (shared_resource->task_count == 0)
		return (0);
	if (shared_resource->tasks[0].developer == developer)
		return (1);
	return (0);
}

int	dongles_are_ready(struct s_shared_resource *shared_resource,
	int left_dongle, int right_dongle)
{
	int		are_ready;
	long	current_time;

	pthread_mutex_lock(&shared_resource->dongles[left_dongle].mutex);
	if (left_dongle != right_dongle)
		pthread_mutex_lock(&shared_resource->dongles[right_dongle].mutex);
	current_time = get_time();
	are_ready = (shared_resource->dongles[left_dongle].is_available
		&& shared_resource->dongles[left_dongle].cooldown_until <= current_time
		&& shared_resource->dongles[right_dongle].is_available
		&& shared_resource->dongles[right_dongle].cooldown_until <= current_time);
	if (left_dongle != right_dongle)
		pthread_mutex_unlock(&shared_resource->dongles[right_dongle].mutex);
	pthread_mutex_unlock(&shared_resource->dongles[left_dongle].mutex);
	return (are_ready);
}

void	reserve_dongles(struct s_shared_resource *shared_resource,
	int left_dongle, int right_dongle)
{
	pthread_mutex_lock(&shared_resource->dongles[left_dongle].mutex);
	if (left_dongle != right_dongle)
		pthread_mutex_lock(&shared_resource->dongles[right_dongle].mutex);
	shared_resource->dongles[left_dongle].is_available = 0;
	if (left_dongle != right_dongle)
		shared_resource->dongles[right_dongle].is_available = 0;
	if (left_dongle != right_dongle)
		pthread_mutex_unlock(&shared_resource->dongles[right_dongle].mutex);
	pthread_mutex_unlock(&shared_resource->dongles[left_dongle].mutex);
}

void	wait_for_dongles(struct s_shared_resource *shared_resource,
	int left_dongle, int right_dongle)
{
	long			wait_until;
	long			current_time;
	struct timespec	timeout;

	pthread_mutex_lock(&shared_resource->dongles[left_dongle].mutex);
	if (left_dongle != right_dongle)
		pthread_mutex_lock(&shared_resource->dongles[right_dongle].mutex);
	wait_until = shared_resource->dongles[left_dongle].cooldown_until;
	if (shared_resource->dongles[right_dongle].cooldown_until > wait_until)
		wait_until = shared_resource->dongles[right_dongle].cooldown_until;
	if (left_dongle != right_dongle)
		pthread_mutex_unlock(&shared_resource->dongles[right_dongle].mutex);
	pthread_mutex_unlock(&shared_resource->dongles[left_dongle].mutex);
	current_time = get_time();
	if (wait_until <= current_time)
		wait_until = current_time + 1;
	timeout.tv_sec = wait_until / 1000;
	timeout.tv_nsec = (wait_until % 1000) * 1000000;
	pthread_cond_timedwait(&shared_resource->scheduler_cond,
		&shared_resource->scheduler_mutex, &timeout);
}

void	remove_first_task(struct s_shared_resource *shared_resource)
{
	int	index;
	int	left_child;
	int	right_child;
	int	first_task;
	int	must_swap;
	struct s_task	temporary;

	if (shared_resource->task_count == 0)
		return ;
	shared_resource->task_count--;
	if (shared_resource->task_count == 0)
		return ;
	shared_resource->tasks[0] =
		shared_resource->tasks[shared_resource->task_count];
	index = 0;
	while (1)
	{
		left_child = index * 2 + 1;
		right_child = index * 2 + 2;
		first_task = index;
		if (left_child < shared_resource->task_count)
		{
			must_swap = shared_resource->tasks[left_child].arrival_order
				< shared_resource->tasks[first_task].arrival_order;
			if (strcmp(shared_resource->config.scheduler, "edf") == 0)
				must_swap = (shared_resource->tasks[left_child].deadline
					< shared_resource->tasks[first_task].deadline || (shared_resource->tasks[left_child].deadline
					== shared_resource->tasks[first_task].deadline && must_swap));
			if (must_swap)
				first_task = left_child;
		}
		if (right_child < shared_resource->task_count)
		{
			must_swap = shared_resource->tasks[right_child].arrival_order
				< shared_resource->tasks[first_task].arrival_order;
			if (strcmp(shared_resource->config.scheduler, "edf") == 0)
				must_swap = (shared_resource->tasks[right_child].deadline
					< shared_resource->tasks[first_task].deadline || (shared_resource->tasks[right_child].deadline
					== shared_resource->tasks[first_task].deadline && must_swap));
			if (must_swap)
				first_task = right_child;
		}
		if (first_task == index)
			break ;
		temporary = shared_resource->tasks[index];
		shared_resource->tasks[index] = shared_resource->tasks[first_task];
		shared_resource->tasks[first_task] = temporary;
		index = first_task;
	}
}

void	remove_task(struct s_shared_resource *shared_resource,
	struct s_developer *developer)
{
	int			index;
	int			parent;
	int			left_child;
	int			right_child;
	int			first_task;
	int			must_swap;
	struct s_task	temporary;

	index = 0;
	while (index < shared_resource->task_count
		&& shared_resource->tasks[index].developer != developer)
		index++;
	if (index == shared_resource->task_count)
		return ;
	shared_resource->task_count--;
	if (index == shared_resource->task_count)
		return ;
	shared_resource->tasks[index] =
		shared_resource->tasks[shared_resource->task_count];
	while (index > 0)
	{
		parent = (index - 1) / 2;
		must_swap = (strcmp(shared_resource->config.scheduler, "fifo") == 0
			&& shared_resource->tasks[index].arrival_order
			< shared_resource->tasks[parent].arrival_order)
			|| (strcmp(shared_resource->config.scheduler, "edf") == 0
			&& (shared_resource->tasks[index].deadline
			< shared_resource->tasks[parent].deadline
			|| (shared_resource->tasks[index].deadline
			== shared_resource->tasks[parent].deadline
			&& shared_resource->tasks[index].arrival_order
			< shared_resource->tasks[parent].arrival_order)));
		if (!must_swap)
			break ;
		temporary = shared_resource->tasks[index];
		shared_resource->tasks[index] = shared_resource->tasks[parent];
		shared_resource->tasks[parent] = temporary;
		index = parent;
	}
	while (1)
	{
		left_child = index * 2 + 1;
		right_child = index * 2 + 2;
		first_task = index;
		if (left_child < shared_resource->task_count)
		{
			must_swap = (strcmp(shared_resource->config.scheduler, "fifo") == 0
				&& shared_resource->tasks[left_child].arrival_order
				< shared_resource->tasks[first_task].arrival_order)
				|| (strcmp(shared_resource->config.scheduler, "edf") == 0
				&& (shared_resource->tasks[left_child].deadline
				< shared_resource->tasks[first_task].deadline
				|| (shared_resource->tasks[left_child].deadline
				== shared_resource->tasks[first_task].deadline
				&& shared_resource->tasks[left_child].arrival_order
				< shared_resource->tasks[first_task].arrival_order)));
			if (must_swap)
				first_task = left_child;
		}
		if (right_child < shared_resource->task_count)
		{
			must_swap = (strcmp(shared_resource->config.scheduler, "fifo") == 0
				&& shared_resource->tasks[right_child].arrival_order
				< shared_resource->tasks[first_task].arrival_order)
				|| (strcmp(shared_resource->config.scheduler, "edf") == 0
				&& (shared_resource->tasks[right_child].deadline
				< shared_resource->tasks[first_task].deadline
				|| (shared_resource->tasks[right_child].deadline
				== shared_resource->tasks[first_task].deadline
				&& shared_resource->tasks[right_child].arrival_order
				< shared_resource->tasks[first_task].arrival_order)));
			if (must_swap)
				first_task = right_child;
		}
		if (first_task == index)
			break ;
		temporary = shared_resource->tasks[index];
		shared_resource->tasks[index] = shared_resource->tasks[first_task];
		shared_resource->tasks[first_task] = temporary;
		index = first_task;
	}
}
void	release_dongles(struct s_developer *developer)
{
	struct s_shared_resource	*shared_resource;
	int					left_dongle;
	int					right_dongle;
	long					cooldown_until;

	shared_resource = developer->shared_resource;
	left_dongle = developer->id - 1;
	right_dongle = developer->id
		% shared_resource->config.number_of_coders;
	pthread_mutex_lock(&shared_resource->scheduler_mutex);
	pthread_mutex_lock(&shared_resource->dongles[left_dongle].mutex);
	if (left_dongle != right_dongle)
		pthread_mutex_lock(&shared_resource->dongles[right_dongle].mutex);
	cooldown_until = get_time() + shared_resource->config.dongle_cooldown;
	shared_resource->dongles[left_dongle].is_available = 1;
	shared_resource->dongles[left_dongle].cooldown_until = cooldown_until;
	if (left_dongle != right_dongle)
	{
		shared_resource->dongles[right_dongle].is_available = 1;
		shared_resource->dongles[right_dongle].cooldown_until = cooldown_until;
		pthread_mutex_unlock(&shared_resource->dongles[right_dongle].mutex);
	}
	pthread_mutex_unlock(&shared_resource->dongles[left_dongle].mutex);
	pthread_cond_broadcast(&shared_resource->scheduler_cond);
	pthread_mutex_unlock(&shared_resource->scheduler_mutex);
}

int	take_dongles(struct s_developer *developer)
{
	struct s_shared_resource	*shared_resource;
	int					left_dongle;
	int					right_dongle;

	shared_resource = developer->shared_resource;
	left_dongle = developer->id - 1;
	right_dongle = developer->id
		% shared_resource->config.number_of_coders;
	pthread_mutex_lock(&shared_resource->scheduler_mutex);
	add_task(shared_resource, developer);
	while (!is_simulation_stopped(shared_resource))
	{
		if (is_first_task(shared_resource, developer)
			&& dongles_are_ready(shared_resource, left_dongle, right_dongle))
		{
			reserve_dongles(shared_resource, left_dongle, right_dongle);
			remove_first_task(shared_resource);
			pthread_mutex_unlock(&shared_resource->scheduler_mutex);
			return (1);
		}
		wait_for_dongles(shared_resource, left_dongle, right_dongle);
	}
	remove_task(shared_resource, developer);
	pthread_mutex_unlock(&shared_resource->scheduler_mutex);
	return (0);
}
