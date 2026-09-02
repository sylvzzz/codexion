/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      :::::::    */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:++:+:++:+:++:+   +:++ */
/*   By: dbotelho <dbotelho@student.42.fr>            +:++:+:++:+:++:+  ++:++ */
/*                                                +#+#+#+#+#+   +#+#+#+       */
/*   Created: 2026/07/31 14:18:04 by dbotelho         #+#    #+#              */
/*   Updated: 2026/07/31 14:18:04 by dbotelho        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

static int	validate_config(t_config *config)
{
	if (config->num_coders < 1)
		return (print_error("number_of_coders must be >= 1"), 1);
	if (config->time_to_burnout < 1)
		return (print_error("time_to_burnout must be >= 1"), 1);
	if (config->time_to_compile < 1)
		return (print_error("time_to_compile must be >= 1"), 1);
	if (config->time_to_debug < 1)
		return (print_error("time_to_debug must be >= 1"), 1);
	if (config->time_to_refactor < 1)
		return (print_error("time_to_refactor must be >= 1"), 1);
	if (config->compiles_required < 1)
		return (print_error("compiles_required must be >= 1"), 1);
	if (config->dongle_cooldown < 0)
		return (print_error("dongle_cooldown must be >= 0"), 1);
	return (0);
}

int	parse_args(int argc, char **argv, t_config *config)
{
	if (argc != 9)
	{
		print_error("Usage: ./codexion <num_coders> <compile> "
			"<debug> <refactor> <compiles_req> <cooldown> <scheduler>");
		return (1);
	}
	if (ft_atoi(argv[1], &config->num_coders)
		|| ft_atoi(argv[2], &config->time_to_burnout)
		|| ft_atoi(argv[3], &config->time_to_compile)
		|| ft_atoi(argv[4], &config->time_to_debug)
		|| ft_atoi(argv[5], &config->time_to_refactor)
		|| ft_atoi(argv[6], &config->compiles_required)
		|| ft_atoi(argv[7], &config->dongle_cooldown))
		return (print_error("invalid numeric argument"), 1);
	if (strcmp(argv[8], "fifo") == 0)
		config->scheduler = SCHEDULER_FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		config->scheduler = SCHEDULER_EDF;
	else
		return (print_error("scheduler must be 'fifo' or 'edf'"), 1);
	if (validate_config(config))
		return (1);
	return (0);
}
