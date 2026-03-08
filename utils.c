/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zael-has <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 03:06:52 by zael-has          #+#    #+#             */
/*   Updated: 2026/03/08 03:11:37 by zael-has         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	swap_coders(t_coder **heap, int i, int j)
{
	t_coder	*tmp;

	tmp = heap[i];
	heap[i] = heap[j];
	heap[j] = tmp;
}

int	fifo_check(t_coder *a, t_coder *b)
{
	if (a->request_time < b->request_time)
		return (1);
	else if (a-> request_time > b->request_time)
		return (0);
	else
	{
		if (a->id < b->id)
			return (1);
		else
			return (0);
	}
}

int	edf_check(t_sim *sim, t_coder *a, t_coder *b)
{
	long	dl_a;
	long	dl_b;

	dl_a = a->last_compile_start + sim->time_to_burnout;
	dl_b = b->last_compile_start + sim->time_to_burnout;
	if (dl_a < dl_b)
		return (1);
	else if (dl_a > dl_b)
		return (0);
	else
	{
		if (a->id < b->id)
			return (1);
		else
			return (0);
	}
}

void	print_status(t_coder *self, char *message)
{
	long	current_time;

	current_time = get_current_time() - self->sim->start_time;
	pthread_mutex_lock(&self->sim->print_mutex);
	if (!self->sim->stop)
		printf("%ld %d %s\n", current_time, self->id, message);
	pthread_mutex_unlock(&self->sim->print_mutex);
}

long	get_current_time(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}
