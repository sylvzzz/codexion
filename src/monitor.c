/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbotelho <dbotelho@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 14:18:04 by dbotelho          #+#    #+#             */
/*   Updated: 2026/09/02 23:08:56 by dbotelho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

static int	all_coders_done(t_sim *sim)
{
	int	i;
	int	done;

	i = 0;
	done = 1;
	pthread_mutex_lock(&sim->state_lock);
	while (i < sim->config.num_coders)
	{
		if (sim->coders[i].compiles_done < sim->config.compiles_required)
			done = 0;
		i++;
	}
	pthread_mutex_unlock(&sim->state_lock);
	return (done);
}

static void	stop_if_done(t_sim *sim)
{
	if (all_coders_done(sim))
	{
		pthread_mutex_lock(&sim->stop_lock);
		sim->stop = 1;
		pthread_mutex_unlock(&sim->stop_lock);
	}
}

static bool	check_burnout(t_sim *sim, int i, long *now, long *deadline)
{
	int	burned;

	burned = 0;
	pthread_mutex_lock(&sim->state_lock);
	*now = get_time_ms();
	*deadline = sim->coders[i].last_compile_start_ms
		+ sim->config.time_to_burnout;
	if (*now >= *deadline && sim->coders[i].state != STATE_BURNED_OUT
		&& sim->coders[i].compiles_done < sim->config.compiles_required)
	{
		sim->coders[i].state = STATE_BURNED_OUT;
		burned = 1;
	}
	pthread_mutex_unlock(&sim->state_lock);
	if (burned)
	{
		log_state(sim, i + 1, "burned out");
		pthread_mutex_lock(&sim->stop_lock);
		sim->stop = 1;
		pthread_mutex_unlock(&sim->stop_lock);
		return true;
	}
	return false;
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		i;
	long	now;
	long	deadline;

	sim = (t_sim *)arg;
	while (!sim_should_stop(sim))
	{
		i = 0;
		while (i < sim->config.num_coders)
		{
			if (check_burnout(sim, i, &now, &deadline))
				return (NULL);
			i++;
		}
		stop_if_done(sim);
		usleep(1000);
	}
	return (NULL);
}
