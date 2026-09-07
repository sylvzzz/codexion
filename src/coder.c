/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbotelho <dbotelho@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 14:18:04 by dbotelho          #+#    #+#             */
/*   Updated: 2026/09/02 23:09:23 by dbotelho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void	set_state(t_sim *sim, t_coder *coder, t_state state)
{
	pthread_mutex_lock(&sim->state_lock);
	coder->state = state;
	pthread_mutex_unlock(&sim->state_lock);
}

static void	grab_dongles(t_sim *sim, t_coder *coder, long now)
{
	coder->left->in_use = 1;
	coder->left->held_by = coder->id;
	coder->right->in_use = 1;
	coder->right->held_by = coder->id;
	pthread_mutex_lock(&sim->state_lock);
	coder->last_compile_start_ms = now;
	coder->state = STATE_COMPILING;
	pthread_mutex_unlock(&sim->state_lock);
}

static void	wait_for_dongles(t_sim *sim, struct timespec *ts)
{
	long	target;

	target = get_time_ms() + 1;
	ts->tv_sec = target / 1000;
	ts->tv_nsec = (target % 1000) * 1000000L;
	pthread_cond_timedwait(&sim->arbitration_cond,
		&sim->arbitration_lock, ts);
}

static int	acquire_dongles(t_sim *sim, t_coder *coder)
{
	struct timespec	ts;
	long			now;
	int				acquired;

	acquired = 0;
	pthread_mutex_lock(&sim->arbitration_lock);
	coder->request_order = sim->request_counter++;
	set_state(sim, coder, STATE_WAITING_DONGLE);
	pq_push(sim, coder->id - 1);
	while (!sim_should_stop(sim) && !acquired)
	{
		now = get_time_ms();
		if (can_grab(sim, coder, now))
		{
			grab_dongles(sim, coder, now);
			pq_remove(sim, coder->id - 1);
			acquired = 1;
		}
		else
			wait_for_dongles(sim, &ts);
	}
	pthread_mutex_unlock(&sim->arbitration_lock);
	return (acquired);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_sim	*sim;

	coder = (t_coder *)arg;
	sim = coder->sim;
	while (!sim_should_stop(sim)
		&& coder->compiles_done < sim->config.compiles_required)
	{
		if (!acquire_dongles(sim, coder))
			return (NULL);
		compile_phase(sim, coder);
		if (sim_should_stop(sim))
			return (NULL);
		debug_phase(sim, coder);
		if (sim_should_stop(sim))
			return (NULL);
		refactor_phase(sim, coder);
	}
	return (NULL);
}
