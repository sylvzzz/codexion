/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbotelho <dbotelho@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 14:18:04 by dbotelho          #+#    #+#             */
/*   Updated: 2026/09/02 23:08:25 by dbotelho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

typedef enum e_scheduler
{
	SCHEDULER_FIFO,
	SCHEDULER_EDF,
}	t_scheduler;

typedef enum e_state
{
	STATE_IDLE,
	STATE_WAITING_DONGLE,
	STATE_COMPILING,
	STATE_DEBUGGING,
	STATE_REFACTORING,
	STATE_BURNED_OUT
}	t_state;

typedef struct s_config
{
	long			num_coders;
	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	long			compiles_required;
	long			dongle_cooldown;
	t_scheduler		scheduler;
}	t_config;

typedef struct s_dongle
{
	int			id;
	int			in_use;
	long		available_at_ms;
	int			held_by;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	t_dongle		*left;
	t_dongle		*right;
	long			arrival_time_ms;
	long			last_compile_start_ms;
	int				compiles_done;
	t_state			state;
	int				heap_pos;
	struct s_sim	*sim;
}	t_coder;

typedef struct s_sim
{
	t_config		config;
	t_coder			*coders;
	t_dongle		*dongles;
	int				*wait_queue;
	size_t			wait_size;
	long			start_time_ms;
	pthread_mutex_t	log_lock;
	pthread_mutex_t	stop_lock;
	int				stop;
	pthread_t		monitor_thread;
	pthread_mutex_t	arbitration_lock;
	pthread_cond_t	arbitration_cond;
	pthread_mutex_t	state_lock;
}	t_sim;

/* ---- parsing / init ---- */
int		parse_args(int argc, char **argv, t_config *config);
int		init_sim(t_sim *sim, t_config config);
void	destroy_sim(t_sim *sim);

/* ---- utils ---- */
long	get_time_ms(void);
size_t	ft_strlen(const char *s);
char	*ft_strchr(const char *s, int c);
int		ft_atoi(const char *s, long *out);
void	log_state(t_sim *sim, int coder_id, const char *msg);
void	print_error(const char *msg);
int		sim_should_stop(t_sim *sim);

/* ---- coder / monitor threads ---- */
void	*coder_routine(void *arg);
void	*monitor_routine(void *arg);
void	set_state(t_sim *sim, t_coder *coder, t_state state);
int		can_grab(t_sim *sim, t_coder *coder, long now);
int		coder_has_priority(t_sim *sim, int a, int b);
void	pq_push(t_sim *sim, int idx);
void	pq_remove(t_sim *sim, int idx);
void	compile_phase(t_sim *sim, t_coder *coder);
void	debug_phase(t_sim *sim, t_coder *coder);
void	refactor_phase(t_sim *sim, t_coder *coder);

#endif
