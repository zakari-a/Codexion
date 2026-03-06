#include "codexion.h"

static void	wait_start(t_sim *sim)
{
	pthread_mutex_lock(&sim->start_mutex);
	while (!sim->start)
		pthread_cond_wait(&sim->start_cond, &sim->start_mutex);
	pthread_mutex_unlock(&sim->start_mutex);
}

static int	single_coder(t_sim *sim)
{
	if (sim->num_coders == 1)
	{
		while (!sim->stop)
			usleep(10);
		return (1);
	}
	return (0);
}

static int	wait_queue(t_sim *sim, t_coder *self)
{
	pthread_mutex_lock(&sim->queue_mutex);
	self->request_time = get_current_time();
	heap_push(sim, self);
	while (sim->heap[0] != self && !sim->stop)
		pthread_cond_wait(&sim->condition, &sim->queue_mutex);
	pthread_mutex_unlock(&sim->queue_mutex);
	if (sim->stop)
		return (0);
	return (1);
}

static void	work_cycle(t_sim *sim, t_coder *self)
{
	self->last_compile_start = get_current_time();
	print_status(self, "is compiling");
	usleep(sim->time_to_compile * 1000);
	release_dongles(sim, self->id,
		(self->id + 1) % sim->num_coders);
	print_status(self, "is debugging");
	usleep(sim->time_to_debug * 1000);
	print_status(self, "is refactoring");
	usleep(sim->time_to_refactor * 1000);
}

static int	check_compile_limit(t_sim *sim, t_coder *self)
{
	self->compile_count++;
	if (sim->must_compile > 0 && self->compile_count >= sim->must_compile)
	{
		self->finished = 1;
		return (1);
	}
	return (0);
}

void	*routine(void *arg)
{
	t_coder	*self;
	t_sim	*sim;
	int		first;
	int		second;

	self = (t_coder *)arg;
	sim = self->sim;
	first = self->id;
	second = (self->id + 1) % sim->num_coders;
	wait_start(sim);
	if (single_coder(sim))
		return (NULL);
	while (!sim->stop)
	{
		if (!wait_queue(sim, self))
			break ;
		if (!acquire_dongle(sim, first))
			break ;
		print_status(self, "has taken a dongle");
		if (!acquire_dongle(sim, second))
		{
			leave_queue(sim, 2, first);
			break ;
		}
		print_status(self, "has taken a dongle");
		leave_queue(sim, 1, 0);
		work_cycle(sim, self);
		if (check_compile_limit(sim, self))
			break ;
	}
	return (NULL);
}