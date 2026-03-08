/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_helpers.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zael-has <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 03:00:37 by zael-has          #+#    #+#             */
/*   Updated: 2026/03/08 04:57:23 by zael-has         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_timespec	get_wakeup_time(long remaining)
{
	t_timespec	ts;
	long		wakeup_time;

	wakeup_time = get_current_time() + remaining;
	ts.tv_sec = wakeup_time / 1000;
	ts.tv_nsec = (wakeup_time % 1000) * 1000000;
	return (ts);
}

long	dongle_cooldown(t_sim *sim, long last_release)
{
	long	elapsed;
	long	remaining;

	if (last_release == 0)
		return (0);
	elapsed = get_current_time() - last_release;
	if (elapsed < sim->cooldown)
		remaining = sim->cooldown - elapsed;
	else
		remaining = 0;
	return (remaining);
}

void	release_dongles(t_sim *sim, int left, int right)
{
	sim->dongles[left].last_release = get_current_time();
	pthread_mutex_unlock(&sim->dongles[left].mutex);
	pthread_cond_broadcast(&sim->dongles[left].cond);
	sim->dongles[right].last_release = get_current_time();
	pthread_mutex_unlock(&sim->dongles[right].mutex);
	pthread_cond_broadcast(&sim->dongles[right].cond);
}

int	acquire_dongle(t_sim *sim, int idx)
{
	t_timespec	ts;

	pthread_mutex_lock(&sim->dongles[idx].mutex);
	while (dongle_cooldown(sim, sim->dongles[idx].last_release) > 0)
	{
		if (sim->stop)
		{
			pthread_mutex_unlock(&sim->dongles[idx].mutex);
			return (0);
		}
		ts = get_wakeup_time(dongle_cooldown(sim,
					sim->dongles[idx].last_release));
		pthread_cond_timedwait(&sim->dongles[idx].cond,
			&sim->dongles[idx].mutex, &ts);
	}
	if (sim->stop)
	{
		pthread_mutex_unlock(&sim->dongles[idx].mutex);
		return (0);
	}
	return (1);
}

void	leave_queue(t_sim *sim, int flag, int dongle_id)
{
	if (flag == 1)
	{
		pthread_mutex_lock(&sim->queue_mutex);
		heap_pop(sim);
		pthread_cond_broadcast(&sim->condition);
		pthread_mutex_unlock(&sim->queue_mutex);
	}
	else
	{
		pthread_mutex_unlock(&sim->dongles[dongle_id].mutex);
		pthread_mutex_lock(&sim->queue_mutex);
		heap_pop(sim);
		pthread_cond_broadcast(&sim->condition);
		pthread_mutex_unlock(&sim->queue_mutex);
	}
}
