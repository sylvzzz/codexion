#include "codexion.h"

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

static void	check_burnout(t_sim *sim, int i, long *now, long *deadline)
{
	int	burned;

	burned = 0;
	pthread_mutex_lock(&sim->state_lock);
	*now = get_time_ms();
	*deadline = sim->coders[i].last_compile_start_ms
		+ sim->config.time_to_burnout;
	if (*now >= *deadline && sim->coders[i].state != STATE_BURNED_OUT)
	{
		sim->coders[i].state = STATE_BURNED_OUT;
		burned = 1;
	}
	pthread_mutex_unlock(&sim->state_lock);
	if (burned)
	{
		log_action(sim, i, "has burned out");
		pthread_mutex_lock(&sim->stop_lock);
		sim->stop = 1;
		pthread_mutex_unlock(&sim->stop_lock);
	}
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
			check_burnout(sim, i, &now, &deadline);
			i++;
		}
		if (all_coders_done(sim))
		{
			pthread_mutex_lock(&sim->stop_lock);
			sim->stop = 1;
			pthread_mutex_unlock(&sim->stop_lock);
		}
		usleep(1000);
	}
	return (NULL);
}