/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zael-has <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 00:59:56 by zael-has          #+#    #+#             */
/*   Updated: 2026/03/08 03:40:35 by zael-has         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	burn_out(t_sim *sim, int i)
{
	long	current_time;

	pthread_mutex_unlock(&sim->coders[i].c_mutex);
	current_time = get_current_time() - sim->start_time;
	pthread_mutex_lock(&sim->print_mutex);
	printf("%ld %d burned out\n", current_time, sim->coders[i].id);
	pthread_mutex_unlock(&sim->print_mutex);
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	pthread_mutex_lock(&sim->queue_mutex);
	pthread_cond_broadcast(&sim->condition);
	pthread_mutex_unlock(&sim->queue_mutex);
}

void	endof_sim(t_sim *sim)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	pthread_mutex_lock(&sim->queue_mutex);
	pthread_cond_broadcast(&sim->condition);
	pthread_mutex_unlock(&sim->queue_mutex);
}

int	check_coders(t_sim *sim, int *finished_count)
{
	int	i;

	i = 0;
	while (i < sim->num_coders)
	{
		pthread_mutex_lock(&sim->coders[i].c_mutex);
		if (sim->coders[i].finished)
			(*finished_count)++;
		else if (get_current_time() - sim->coders[i].last_compile_start
			> sim->time_to_burnout)
		{
			burn_out(sim, i);
			return (1);
		}
		pthread_mutex_unlock(&sim->coders[i].c_mutex);
		i++;
	}
	return (0);
}

int	stop_check(t_sim *sim)
{
	pthread_mutex_lock(&sim->stop_mutex);
	if (sim->stop)
	{
		pthread_mutex_unlock(&sim->stop_mutex);
		return (1);
	}
	pthread_mutex_unlock(&sim->stop_mutex);
	return (0);
}

void	monitor_simulation(t_sim *sim)
{
	int	finished_count;

	while (1)
	{
		if (stop_check(sim))
			return ;
		pthread_mutex_lock(&sim->start_mutex);
		if (!sim->start)
		{
			pthread_mutex_unlock(&sim->start_mutex);
			usleep(500);
			continue ;
		}
		pthread_mutex_unlock(&sim->start_mutex);
		finished_count = 0;
		if (check_coders(sim, &finished_count))
			return ;
		if (finished_count == sim->num_coders)
		{
			endof_sim(sim);
			return ;
		}
		usleep(500);
	}
}
