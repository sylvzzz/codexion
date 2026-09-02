/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbotelho <dbotelho@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 14:18:04 by dbotelho          #+#    #+#             */
/*   Updated: 2026/09/02 23:06:31 by dbotelho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

int	coder_has_priority(t_sim *sim, int a, int b)
{
	long	da;
	long	db;
	long	aa;
	long	ab;

	if (sim->config.scheduler == SCHEDULER_EDF)
	{
		da = sim->coders[a].last_compile_start_ms
			+ sim->config.time_to_burnout;
		db = sim->coders[b].last_compile_start_ms
			+ sim->config.time_to_burnout;
		if (da != db)
			return (da < db);
	}
	else
	{
		aa = sim->coders[a].arrival_time_ms;
		ab = sim->coders[b].arrival_time_ms;
		if (aa != ab)
			return (aa < ab);
	}
	return (sim->coders[a].id < sim->coders[b].id);
}

static int	other_sharer(t_sim *sim, t_coder *coder, t_dongle *d)
{
	int	me;
	int	other;

	me = coder->id - 1;
	other = d->id - 1;
	if (other == me)
		other = (other + 1) % (int)sim->config.num_coders;
	return (other);
}

static int	neighbor_blocks(t_sim *sim, t_coder *coder, t_dongle *d)
{
	int	other;
	int	idx;

	other = other_sharer(sim, coder, d);
	idx = coder->id - 1;
	if (sim->coders[other].heap_pos < 0)
		return (0);
	return (coder_has_priority(sim, other, idx));
}

int	can_grab(t_sim *sim, t_coder *coder, long now)
{
	t_dongle	*left;
	t_dongle	*right;

	left = coder->left;
	right = coder->right;
	if (left == right)
		return (0);
	if (left->in_use || left->available_at_ms > now)
		return (0);
	if (right->in_use || right->available_at_ms > now)
		return (0);
	if (neighbor_blocks(sim, coder, left))
		return (0);
	return (!neighbor_blocks(sim, coder, right));
}
