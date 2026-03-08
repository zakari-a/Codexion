/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_work.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zael-has <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 22:33:12 by zael-has          #+#    #+#             */
/*   Updated: 2026/03/06 22:33:41 by zael-has         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	wait_start(t_sim *sim)
{
	pthread_mutex_lock(&sim->start_mutex);
	while (!sim->start)
		pthread_cond_wait(&sim->start_cond, &sim->start_mutex);
	pthread_mutex_unlock(&sim->start_mutex);
}

int	single_coder(t_sim *sim)
{
	if (sim->num_coders == 1)
	{
		while (!sim->stop)
			usleep(10);
		return (1);
	}
	return (0);
}

int	wait_queue(t_sim *sim, t_coder *self)
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

void	work_cycle(t_sim *sim, t_coder *self)
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

int	check_compile_limit(t_sim *sim, t_coder *self)
{
	self->compile_count++;
	if (sim->must_compile > 0 && self->compile_count >= sim->must_compile)
	{
		self->finished = 1;
		return (1);
	}
	return (0);
}
