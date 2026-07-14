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

// reading program args.
typedef struct s_config
{
	int			num_coders;			// número de programadores/dongles
	long		time_to_burnout;	// prazo máximo sem compilar antes de esgotar
	long		time_to_compile;	// duração da fase de compilação
	long		time_to_debug;		// duração da fase de depuração
	long		time_to_refactor;	// duração da fase de refactoring
	int			compiles_required;	// nº de compilações para terminar a simulação
	long		dongle_cooldown;	// tempo de indisponibilidade após libertar um dongle
	t_scheduler	scheduler;			// política de arbitragem (fifo ou edf)
}	t_config;

typedef struct s_dongle
{
	int				id;					// identificador do dongle
	int				in_use;				// 1 se está atualmente a ser usado
	long			available_at_ms;	// timestamp em que o cooldown termina
	int				held_by;			// id do coder que o detém (0/-1 se livre)
}	t_dongle;

// Estado de um coder (thread).

typedef struct s_coder
{
	int				id;						// identificador do coder (1..N)
	pthread_t		thread;					// handle da thread deste coder
	t_dongle		*left;					// dongle à esquerda
	t_dongle		*right;					// dongle à direita
	long			arrival_time_ms;		// momento em que pediu para compilar (FIFO)
	long			last_compile_start_ms;	// início da última compilação (base do deadline EDF)
	int				compiles_done;			// número de compilações já concluídas
	t_state			state;					// estado atual do coder
	struct s_sim	*sim;					// back-reference para aceder ao contexto global
}	t_coder;

typedef struct s_sim
{
	t_config		config;				// argumentos da simulação
	t_coder			*coders;			// array de coders
	t_dongle		*dongles;			// array de dongles
	long			start_time_ms;		// timestamp de início da simulação

	pthread_mutex_t	log_lock;		    // protege printf/write

	pthread_mutex_t	stop_lock;		    // protege a flag "stop" abaixo
	int				stop;			    // 1 = simulação deve parar (burnout ou fim)

	pthread_t		monitor_thread;	    // thread que deteta o esgotamento

	pthread_mutex_t	arbitration_lock;	// protege a decisão de quem agarra que dongles
	pthread_cond_t	arbitration_cond;	// cond var onde os coders esperam a sua vez
    pthread_mutex_t	state_lock;     	// protege last_compile_start_ms e state de cada coder
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
void	error_exit(const char *msg);

/* ---- coder / monitor threads (a implementar por ti) ---- */
void	*coder_routine(void *arg);
void	*monitor_routine(void *arg);

#endif