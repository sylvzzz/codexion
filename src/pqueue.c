/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      :::::::    */
/*   pqueue.c                                           :+:      :+:    :+:   */
/*                                                    +:++:+:++:+:++:+   +:++ */
/*   By: dbotelho <dbotelho@student.42.fr>            +:++:+:++:+:++:+  ++:++ */
/*                                                +#+#+#+#+#+   +#+#+#+       */
/*   Created: 2026/08/06 15:00:00 by dbotelho         #+#    #+#              */
/*   Updated: 2026/08/06 15:00:00 by dbotelho        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

static void	swap_entries(t_sim *sim, int a, int b)
{
	int	tmp;

	tmp = sim->wait_queue[a];
	sim->wait_queue[a] = sim->wait_queue[b];
	sim->wait_queue[b] = tmp;
	sim->coders[sim->wait_queue[a]].heap_pos = a;
	sim->coders[sim->wait_queue[b]].heap_pos = b;
}

static void	sift_up(t_sim *sim, int idx)
{
	int	parent;

	while (idx > 0)
	{
		parent = (idx - 1) / 2;
		if (coder_has_priority(sim, sim->wait_queue[idx],
				sim->wait_queue[parent]))
		{
			swap_entries(sim, idx, parent);
			idx = parent;
		}
		else
			break ;
	}
}

static void	sift_down(t_sim *sim, int idx)
{
	int	n;
	int	l;
	int	r;
	int	best;

	n = (int)sim->wait_size;
	while (1)
	{
		l = 2 * idx + 1;
		r = 2 * idx + 2;
		best = idx;
		if (l < n && coder_has_priority(sim, sim->wait_queue[l],
				sim->wait_queue[best]))
			best = l;
		if (r < n && coder_has_priority(sim, sim->wait_queue[r],
				sim->wait_queue[best]))
			best = r;
		if (best == idx)
			break ;
		swap_entries(sim, idx, best);
		idx = best;
	}
}

void	pq_push(t_sim *sim, int idx)
{
	int	i;

	i = (int)sim->wait_size;
	sim->wait_queue[i] = idx;
	sim->coders[idx].heap_pos = i;
	sim->wait_size++;
	sift_up(sim, i);
}

void	pq_remove(t_sim *sim, int idx)
{
	int	pos;
	int	last;

	pos = sim->coders[idx].heap_pos;
	last = (int)sim->wait_size - 1;
	sim->wait_size--;
	if (pos < last)
	{
		sim->wait_queue[pos] = sim->wait_queue[last];
		sim->coders[sim->wait_queue[pos]].heap_pos = pos;
		sift_down(sim, pos);
		sift_up(sim, pos);
	}
	sim->coders[idx].heap_pos = -1;
}
