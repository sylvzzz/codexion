#include "codexion.h"

void	log_state(t_sim *sim, int coder_id, const char *msg)
{
	pthread_mutex_lock(&sim->log_lock);
	printf("%ld %d %s\n", get_time_ms() - sim->start_time_ms, coder_id, msg);
	pthread_mutex_unlock(&sim->log_lock);
}

void	error_exit(const char *msg)
{
	write(2, "Error: ", 7);
	write(2, msg, strlen(msg));
	write(2, "\n", 1);
	exit(1);
}

static int	validate_config(t_config *config)
{
	if (config->num_coders < 1)
		return (error_exit("number_of_coders must be >= 1"), 1);
	if (config->time_to_burnout < 1)
		return (error_exit("time_to_burnout must be >= 1"), 1);
	if (config->time_to_compile < 1)
		return (error_exit("time_to_compile must be >= 1"), 1);
	if (config->time_to_debug < 1)
		return (error_exit("time_to_debug must be >= 1"), 1);
	if (config->time_to_refactor < 1)
		return (error_exit("time_to_refactor must be >= 1"), 1);
	if (config->compiles_required < 1)
		return (error_exit("compiles_required must be >= 1"), 1);
	if (config->dongle_cooldown < 0)
		return (error_exit("dongle_cooldown must be >= 0"), 1);
	return (0);
}

int	parse_args(int argc, char **argv, t_config *config)
{
	if (argc != 9)
		error_exit("Usage: ./codexion <num_coders> <burnout> <compile> "
			"<debug> <refactor> <compiles_req> <cooldown> <scheduler>");
	if (ft_atoi(argv[1], &config->num_coders)
		|| ft_atoi(argv[2], &config->time_to_burnout)
		|| ft_atoi(argv[3], &config->time_to_compile)
		|| ft_atoi(argv[4], &config->time_to_debug)
		|| ft_atoi(argv[5], &config->time_to_refactor)
		|| ft_atoi(argv[6], &config->compiles_required)
		|| ft_atoi(argv[7], &config->dongle_cooldown))
		error_exit("invalid numeric argument");
	if (strcmp(argv[8], "fifo") == 0)
		config->scheduler = SCHEDULER_FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		config->scheduler = SCHEDULER_EDF;
	else
		error_exit("scheduler must be 'fifo' or 'edf'");
	if (validate_config(config))
		return (1);
	return (0);
}
