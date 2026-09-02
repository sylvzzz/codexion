/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      :::::::    */
/*   phases.c                                           :+:      :+:    :+:   */
/*                                                    +:++:+:++:+:++:+   +:++ */
/*   By: dbotelho <dbotelho@student.42.fr>            +:++:+:++:+:++:+  ++:++ */
/*                                                +#+#+#+#+#+   +#+#+#+       */
/*   Created: 2026/07/31 14:18:04 by dbotelho         #+#    #+#              */
/*   Updated: 2026/07/31 14:18:04 by dbotelho        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void	compile_phase(t_sim *sim, t_coder *coder)
{
	long	now;

	log_state(sim, coder->id, "has taken a dongle");
	log_state(sim, coder->id, "has taken a dongle");
	log_state(sim, coder->id, "is compiling");
	usleep(sim->config.time_to_compile * 1000);
	now = get_time_ms();
	pthread_mutex_lock(&sim->arbitration_lock);
	coder->left->in_use = 0;
	coder->left->held_by = -1;
	coder->left->available_at_ms = now + sim->config.dongle_cooldown;
	coder->right->in_use = 0;
	coder->right->held_by = -1;
	coder->right->available_at_ms = now + sim->config.dongle_cooldown;
	pthread_mutex_lock(&sim->state_lock);
	coder->compiles_done++;
	pthread_mutex_unlock(&sim->state_lock);
	pthread_cond_broadcast(&sim->arbitration_cond);
	pthread_mutex_unlock(&sim->arbitration_lock);
}

void	debug_phase(t_sim *sim, t_coder *coder)
{
	set_state(sim, coder, STATE_DEBUGGING);
	log_state(sim, coder->id, "is debugging");
	usleep(sim->config.time_to_debug * 1000);
}

void	refactor_phase(t_sim *sim, t_coder *coder)
{
	set_state(sim, coder, STATE_REFACTORING);
	log_state(sim, coder->id, "is refactoring");
	usleep(sim->config.time_to_refactor * 1000);
}
