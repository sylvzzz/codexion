/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbotelho <dbotelho@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 14:18:04 by dbotelho          #+#    #+#             */
/*   Updated: 2026/09/02 23:09:03 by dbotelho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

static int	abort_threads(t_sim *sim, long n)
{
	long	i;

	pthread_mutex_lock(&sim->stop_lock);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_lock);
	i = 0;
	while (i < n)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	return (1);
}

static int	create_coders(t_sim *sim)
{
	long	i;

	i = 0;
	while (i < sim->config.num_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]))
			return (abort_threads(sim, i), print_error("pthread_create"), 1);
		i++;
	}
	return (0);
}

static int	create_monitor(t_sim *sim)
{
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim))
		return (abort_threads(sim, sim->config.num_coders),
			print_error("pthread_create"), 1);
	return (0);
}

static void	join_threads(t_sim *sim)
{
	long	i;

	i = 0;
	while (i < sim->config.num_coders)
	{
		if (pthread_join(sim->coders[i].thread, NULL))
			print_error("pthread_join");
		i++;
	}
	if (pthread_join(sim->monitor_thread, NULL))
		print_error("pthread_join");
}

int	main(int ac, char **av)
{
	t_config	config;
	t_sim		sim;

	if (parse_args(ac, av, &config))
		return (1);
	if (init_sim(&sim, config))
		return (print_error("failed to initialise simulation"), 1);
	if (create_coders(&sim))
		return (destroy_sim(&sim), 1);
	if (create_monitor(&sim))
		return (destroy_sim(&sim), 1);
	join_threads(&sim);
	destroy_sim(&sim);
	return (0);
}
