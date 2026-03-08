/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zael-has <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 23:34:59 by zael-has          #+#    #+#             */
/*   Updated: 2026/03/07 23:46:06 by zael-has         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_more_urgent(t_sim *sim, t_coder *a, t_coder *b)
{
	if (sim->scheduler == 0)
		return (fifo_check(a, b));
	else
		return (edf_check(sim, a, b));
}

void	heap_push(t_sim *sim, t_coder *new_coder)
{
	int	index;

	sim->heap[sim->heap_size] = new_coder;
	index = sim->heap_size;
	while (index > 0)
	{
		if (is_more_urgent(sim, new_coder, sim->heap[(index - 1) / 2]) == 1)
		{
			swap_coders(sim->heap, index, (index - 1) / 2);
			index = (index - 1) / 2;
		}
		else
			break ;
	}
	sim->heap_size++;
}

void	heap_pop(t_sim *sim)
{
	int	i;
	int	left;
	int	right;
	int	target;

	if (sim->heap_size == 0)
	{
		pthread_mutex_unlock(&sim->queue_mutex);
		return ;
	}
	sim->heap[0] = sim->heap[sim->heap_size - 1];
	sim->heap_size--;
	i = 0;
	while ((2 * i) + 1 < sim->heap_size)
	{
		left = (i * 2) + 1;
		right = (i * 2) + 2;
		target = left;
		if (right < sim->heap_size && is_more_urgent(sim,
				sim->heap[right], sim->heap[left]))
			target = right;
		if (is_more_urgent(sim, sim->heap[target], sim->heap[i]))
		{
			swap_coders(sim->heap, target, i);
			i = target;
		}
		else
			break ;
	}
}
