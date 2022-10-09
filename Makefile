NAME = ft_ping
SRC = ./src/ft_ping.c
OBJ	= $(SRC:.c=.o)
# CFLAGS	=-Wall -Wextra -Werror
CFLAGS	=
MAL_HEADER = ./inc
FT_LIB = ./libft/libft.a


all: $(OBJ)
	@make -C  ./libft
	gcc $(SRC) $(FT_LIB) -I $(MAL_HEADER) -o $(NAME)

clean:
	@make -C  ./libft clean
	-rm $(OBJ)

fclean: clean
	@make -C  ./libft fclean
	-rm $(NAME)

re: fclean all