/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zael-has <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 00:10:20 by zael-has          #+#    #+#             */
/*   Updated: 2026/03/08 02:00:06 by zael-has         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->num_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				routine, &sim->coders[i]) != 0)
		{
			printf("Error creating thread %d\n", i);
			return (1);
		}
		i++;
	}
	return (0);
}

void	set_starttime(t_sim *sim)
{
	pthread_mutex_lock(&sim->start_mutex);
	sim->start_time = get_current_time();
	sim->start = 1;
	pthread_cond_broadcast(&sim->start_cond);
	pthread_mutex_unlock(&sim->start_mutex);
}
	
void	set_compiletime(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->num_coders)
	{
		sim->coders[i].last_compile_start = sim->start_time;
		i++;
	}
}

void	join_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->num_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

int	main(int ac, char **av)
{
	t_sim	sim;

	if (parse(av, ac))
	{
		printf("Please provide a valid arguments");
		return (1);
	}
	if (init_simulation(&sim, av))
		return (1);
	if (init_coders(&sim))
		return (1);
	set_compiletime(&sim);
	monitor_simulation(&sim);
	join_coders(&sim);
	cleanup_simulation(&sim);
}
