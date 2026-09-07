/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbotelho <dbotelho@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 14:18:04 by dbotelho          #+#    #+#             */
/*   Updated: 2026/08/06 19:48:43 by dbotelho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

static void	init_dongles(t_sim *sim)
{
	long	i;

	i = 0;
	while (i < sim->config.num_coders)
	{
		sim->dongles[i].id = i + 1;
		sim->dongles[i].in_use = 0;
		sim->dongles[i].available_at_ms = 0;
		sim->dongles[i].held_by = -1;
		i++;
	}
}

static void	init_coders(t_sim *sim)
{
	long	i;
	long	n;

	n = sim->config.num_coders;
	i = 0;
	while (i < n)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].left = &sim->dongles[i];
		sim->coders[i].right = &sim->dongles[(i - 1 + n) % n];
		sim->coders[i].request_order = 0;
		sim->coders[i].last_compile_start_ms = sim->start_time_ms;
		sim->coders[i].compiles_done = 0;
		sim->coders[i].state = STATE_IDLE;
		sim->coders[i].heap_pos = -1;
		sim->coders[i].sim = sim;
		i++;
	}
}

static int	init_locks(t_sim *sim)
{
	if (pthread_mutex_init(&sim->log_lock, NULL))
		return (1);
	if (pthread_mutex_init(&sim->stop_lock, NULL))
		return (1);
	if (pthread_mutex_init(&sim->arbitration_lock, NULL))
		return (1);
	if (pthread_cond_init(&sim->arbitration_cond, NULL))
		return (1);
	if (pthread_mutex_init(&sim->state_lock, NULL))
		return (1);
	return (0);
}

int	init_sim(t_sim *sim, t_config config)
{
	sim->config = config;
	sim->start_time_ms = get_time_ms();
	sim->request_counter = 1;
	sim->stop = 0;
	sim->coders = malloc(sizeof(t_coder) * config.num_coders);
	if (!sim->coders)
		return (1);
	sim->dongles = malloc(sizeof(t_dongle) * config.num_coders);
	if (!sim->dongles)
		return (free(sim->coders), 1);
	sim->wait_queue = malloc(sizeof(int) * config.num_coders);
	if (!sim->wait_queue)
		return (free(sim->coders), free(sim->dongles), 1);
	sim->wait_size = 0;
	init_dongles(sim);
	init_coders(sim);
	if (init_locks(sim))
		return (destroy_sim(sim), 1);
	return (0);
}

void	destroy_sim(t_sim *sim)
{
	pthread_mutex_destroy(&sim->log_lock);
	pthread_mutex_destroy(&sim->stop_lock);
	pthread_mutex_destroy(&sim->arbitration_lock);
	pthread_cond_destroy(&sim->arbitration_cond);
	pthread_mutex_destroy(&sim->state_lock);
	if (sim->coders)
		free(sim->coders);
	if (sim->dongles)
		free(sim->dongles);
	if (sim->wait_queue)
		free(sim->wait_queue);
}
