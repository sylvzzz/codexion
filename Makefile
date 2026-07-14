NAME = codexion

SRC =  estudo.c \
		process.c \

OFILES = $(SRC:.c=.o)

.c.o:
	cc -Wall -Werror -Wextra -c $< -o $(<:.c=.o)

$(NAME): $(OFILES)
	cc -Wall -Werror -Wextra $(OFILES) -o $(NAME)

all: $(NAME)

run:
	./codexion

clean:
	rm -f $(OFILES)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re