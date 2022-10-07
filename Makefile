NAME = ft_ping
SRC = ./src/ft_ping.c
OBJ	= $(SRC:.c=.o)
# CFLAGS	=-Wall -Wextra -Werror
CFLAGS	=
MAL_HEADER = ./inc


all: $(OBJ)
	@gcc $(SRC) -I $(MAL_HEADER) -o $(NAME)

clean:
	-rm $(OBJ)

fclean: clean
	-rm $(NAME)

re: fclean all