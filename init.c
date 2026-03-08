/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zael-has <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 23:56:59 by zael-has          #+#    #+#             */
/*   Updated: 2026/03/08 00:07:43 by zael-has         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	init_configs(t_sim *sim, char **args)
{
	sim->ready_count = 0;
	sim->start = 0;
	sim->start_time = get_current_time();
	sim->stop = 0;
	sim->heap_size = 0;
	sim->num_coders = atoi(args[1]);
	sim->time_to_burnout = atoi(args[2]);
	sim->time_to_compile = atoi(args[3]);
	sim->time_to_debug = atoi(args[4]);
	sim->time_to_refactor = atoi(args[5]);
	sim->must_compile = atoi(args[6]);
	sim->cooldown = atoi(args[7]);
	if (strcmp(args[8], "fifo") == 0)
		sim->scheduler = 0;
	else
		sim->scheduler = 1;
}

int	allocate_configs(t_sim *sim)
{
	sim->heap = malloc(sizeof(t_coder *) * sim->num_coders);
	if (!sim->heap)
		return (1);
	sim->coders = malloc(sizeof(t_coder) * sim->num_coders);
	if (!sim->coders)
	{
		if (sim->heap)
			free(sim->heap);
		return (1);
	}
	sim->dongles = malloc(sizeof(t_dongle) * sim->num_coders);
	if (!sim->dongles)
	{
		if (sim->coders)
			free(sim->coders);
		if (sim->heap)
			free(sim->heap);
		return (1);
	}
	return (0);
}

void	init_syncs(t_sim *sim)
{
	pthread_mutex_init(&sim->stop_mutex, NULL);
	pthread_mutex_init(&sim->print_mutex, NULL);
	pthread_mutex_init(&sim->queue_mutex, NULL);
	pthread_mutex_init(&sim->start_mutex, NULL);
	pthread_cond_init(&sim->start_cond, NULL);
	pthread_cond_init(&sim->condition, NULL);
}

int	init_simulation(t_sim *sim, char **args)
{
	int	i;

	init_configs(sim, args);
	if (allocate_configs(sim))
		return (1);
	init_syncs(sim);
	i = 0;
	while (i < sim->num_coders)
	{
		pthread_mutex_init(&sim->dongles[i].mutex, NULL);
		pthread_cond_init(&sim->dongles[i].cond, NULL);
		sim->dongles[i].last_release = 0;
		sim->coders[i].id = i + 1;
		sim->coders[i].compile_count = 0;
		sim->coders[i].sim = sim;
		sim->coders[i].finished = 0;
		sim->coders[i].last_compile_start = 0;
		sim->coders[i].request_time = 0;
		sim->coders[i].finished = 0;
		pthread_mutex_init(&sim->coders[i].c_mutex, NULL);
		i++;
	}
	return (0);
}
