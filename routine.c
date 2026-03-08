/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zael-has <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 22:34:29 by zael-has          #+#    #+#             */
/*   Updated: 2026/03/08 03:21:12 by zael-has         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	main_loop(t_sim *sim, t_coder *self, int first, int second)
{
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
}

void	*routine(void *arg)
{
	t_coder	*self;
	t_sim	*sim;

	int (first), (second);
	self = (t_coder *)arg;
	sim = self->sim;
	first = self->id;
	second = (self->id + 1) % sim->num_coders;
	wait_start(sim);
	if (single_coder(sim))
		return (NULL);
	main_loop(sim, self, first, second);
	return (NULL);
}
