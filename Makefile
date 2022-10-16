NAME = ft_ping
SRC = ./src/ft_ping.c
OBJ	= $(SRC:.c=.o)
CFLAGS	=-Wall -Wextra -Werror
# CFLAGS	=
MAL_HEADER = ./inc
FT_LIB = ./libft/libft.a

all: $(FT_LIB) $(NAME)

$(FT_LIB): 
	@make -C  ./libft

$(NAME): $(OBJ)
	gcc $(CFLAGS) -o $(NAME) $(OBJ) $(FT_LIB)


clean:
	@make -C  ./libft clean
	-rm $(OBJ)

fclean: clean
	@make -C  ./libft fclean
	-rm $(NAME)

re: fclean all