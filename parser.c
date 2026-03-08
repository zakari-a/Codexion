/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zael-has <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 10:49:01 by zael-has          #+#    #+#             */
/*   Updated: 2026/03/08 02:21:03 by zael-has         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_positive(char *str)
{
	int	i;

	if (!str[0])
		return (1);
	i = 0;
	while (str[i])
	{
		if (!(str[i] >= '0' && str[i] <= '9'))
			return (1);
		i++;
	}
	return (0);
}

int	parse(char **args, int size)
{
	int	i;

	if (size != 9)
		return (1);
	i = 1;
	while (i < size - 1)
	{
		if (is_positive(args[i]))
			return (1);
		i++;
	}
	if (strcmp(args[size - 1], "fifo") != 0
		&& strcmp(args[size - 1], "edf") != 0)
		return (1);
	return (0);
}
