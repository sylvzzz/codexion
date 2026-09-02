/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbotelho <dbotelho@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 14:18:04 by dbotelho          #+#    #+#             */
/*   Updated: 2026/09/02 23:09:12 by dbotelho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void	print_error(const char *msg)
{
	write(2, "Error: ", 7);
	write(2, msg, strlen(msg));
	write(2, "\n", 1);
}

void	log_state(t_sim *sim, int coder_id, const char *msg)
{
	pthread_mutex_lock(&sim->log_lock);
	printf("%ld %d %s\n", get_time_ms() - sim->start_time_ms, coder_id, msg);
	pthread_mutex_unlock(&sim->log_lock);
}
