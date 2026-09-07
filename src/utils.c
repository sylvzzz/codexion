/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbotelho <dbotelho@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 14:18:04 by dbotelho          #+#    #+#             */
/*   Updated: 2026/09/07 23:12:22 by dbotelho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

int	ft_atoi(const char *s, long *out)
{
	long	result;

	result = 0;
	if (*s == '-')
		return (1);
	if (*s == '+')
		s++;
	if (*s < '0' || *s > '9')
		return (1);
	while (*s >= '0' && *s <= '9')
	{
		result = result * 10 + (*s - '0');
		if (result < 0)
			return (1);
		s++;
	}
	if (*s != '\0')
		return (1);
	*out = result;
	return (0);
}

int	sim_should_stop(t_sim *sim)
{
	int	stop;

	pthread_mutex_lock(&sim->stop_lock);
	stop = sim->stop;
	pthread_mutex_unlock(&sim->stop_lock);
	return (stop);
}
