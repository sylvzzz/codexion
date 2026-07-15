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
		log_state(sim, i, "has burned out");
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


int main(int ac, char **av)
{
	if (ac != 9)
	{
		printf("Incorrect number of arguments...\n");
		return (1);
	}
	printf("Args parsed: \n");
	printf("number_of_coders: %s\n", av[1]);
	printf("time_to_burnout: %s\n", av[2]);
	printf("time_to_compile: %s\n", av[3]);
	printf("time_to_debug: %s\n", av[4]);
	printf("time_to_refactor: %s\n", av[5]);
	printf("number_of_compiles_required: %s\n", av[6]);
	printf("dongle_cooldown: %s\n", av[7]);
	printf("scheduler: %s\n", av[8]);
}