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
		sim->coders[i].arrival_time_ms = 0;
		sim->coders[i].last_compile_start_ms = sim->start_time_ms;
		sim->coders[i].compiles_done = 0;
		sim->coders[i].state = STATE_IDLE;
		sim->coders[i].sim = sim;
		i++;
	}
}

int	init_sim(t_sim *sim, t_config config)
{
	sim->config = config;
	sim->start_time_ms = get_time_ms();
	sim->stop = 0;
	sim->coders = malloc(sizeof(t_coder) * config.num_coders);
	if (!sim->coders)
		return (1);
	sim->dongles = malloc(sizeof(t_dongle) * config.num_coders);
	if (!sim->dongles)
		return (free(sim->coders), 1);
	init_dongles(sim);
	init_coders(sim);
	if (pthread_mutex_init(&sim->log_lock, NULL))
		return (destroy_sim(sim), 1);
	if (pthread_mutex_init(&sim->stop_lock, NULL))
		return (destroy_sim(sim), 1);
	if (pthread_mutex_init(&sim->arbitration_lock, NULL))
		return (destroy_sim(sim), 1);
	if (pthread_cond_init(&sim->arbitration_cond, NULL))
		return (destroy_sim(sim), 1);
	if (pthread_mutex_init(&sim->state_lock, NULL))
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
}

int	sim_should_stop(t_sim *sim)
{
	int	stop;

	pthread_mutex_lock(&sim->stop_lock);
	stop = sim->stop;
	pthread_mutex_unlock(&sim->stop_lock);
	return (stop);
}
