NAME = codexion

SRC = src/main.c src/coder.c src/scheduler.c src/phases.c src/monitor.c \
	src/sim.c src/parse.c src/utils.c src/log.c src/pqueue.c

OFILES = $(SRC:.c=.o)

.c.o:
	cc -Wall -Werror -Wextra -c $< -o $(<:.c=.o)

$(NAME): $(OFILES)
	cc -Wall -Werror -Wextra $(OFILES) -o $(NAME) -pthread

all: $(NAME)

clean:
	rm -f $(OFILES)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re