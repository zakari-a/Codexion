/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zael-has <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 22:34:29 by zael-has          #+#    #+#             */
/*   Updated: 2026/03/06 22:34:44 by zael-has         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
